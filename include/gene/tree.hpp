#if !defined GENE_TREE_HPP_INCLUDED
#define      GENE_TREE_HPP_INCLUDED

#include "config.hpp"
#include "node.hpp"
#include "operators.hpp"
#include "random_term.hpp"

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <memory>
#include <cstddef>
#include <type_traits>

#include <boost/lexical_cast.hpp>
#include <boost/variant/static_visitor.hpp>

namespace gene {

namespace tree {

    template<class Val, class RandomTermGenerator = random_term::default_random_term<Val>>
    class tree{
    public:
        typedef std::shared_ptr<node<Val>> node_ptr_type;

    private:
        std::shared_ptr<node<Val>> root;

    private:
        struct operator_to_string : boost::static_visitor<std::string>{
            std::vector<std::string> const& args;
            operator_to_string(std::vector<std::string> const& args_) : args(args_) {}
            template<class Operator>
            std::string operator()(Operator const& op) const
            {
                return op.to_string(args);
            }
        };

        std::string expression_impl(node_ptr_type const node_ptr) const
        {
            if(node_ptr->which() == 0){
                // when node has terminal value
                return boost::lexical_cast<std::string>(boost::get<Val>(*node_ptr));
            } else if(node_ptr->which() == 1){
                // when node has operator
                auto const& knot_node = boost::get<knot<Val>>(*node_ptr);
                std::vector<std::string> arg_strs(knot_node.children.size());
                std::transform(knot_node.children.begin(), knot_node.children.end(), arg_strs.begin(),
                        [&](std::shared_ptr<node<Val>> n) {
                            return this->expression_impl(n);
                        });
                return boost::apply_visitor(operator_to_string(arg_strs), knot_node.op);
            }else if(node_ptr->which() == 2){
                // when node has variable terminal
                return variable_name(boost::get<Variable>(*node_ptr));
            } else {
                throw("gene::tree::expression_impl: invalid node value.");
            }
        }

        struct operator_symbol : boost::static_visitor<std::string>{
            template<class Operator>
            std::string operator()(Operator) const
            {
                return Operator::symbol;
            }
        };

        std::string indent(int const level) const
        {
            return std::string(level * config::indent_width, ' ');
        }

        std::string to_string_impl(node_ptr_type const node_ptr, int const level) const
        {
            if(node_ptr->which() == 0){
                return indent(level) + "const: "
                    + boost::lexical_cast<std::string>(boost::get<Val>(*node_ptr)) + '\n';
            } else if(node_ptr->which() == 1){
                auto const& knot_node = boost::get<knot<Val>>(*node_ptr);
                std::string retval = indent(level) + boost::apply_visitor(operator_symbol(), knot_node.op) + ":\n";
                return std::accumulate( knot_node.children.begin(),
                                        knot_node.children.end(),
                                        retval,
                                        [&](std::string acc, std::shared_ptr<node<Val>> n) {
                                            return acc + this->to_string_impl(n, level+1) + '\n';
                                        }
                                      );
            }else if(node_ptr->which() == 2){
                return indent(level) + "var: "
                    + variable_name(boost::get<Variable>(*node_ptr));
            }else {
                throw("gene::tree::to_string_impl: invalid node value.");
            }
        }


        struct apply_operator : boost::static_visitor<Val> {
            std::vector<Val> const& args;
            apply_operator(std::vector<Val> const& args_) : args(args_) {}

            template<class Operator>
            typename std::enable_if<Operator::arity==2, Val>::type
            operator()(Operator) const
            {
                if(args.size() != Operator::arity){
                    throw("apply_operator: invalid number of arguments");
                }
                return Operator()(args[0], args[1]);
            }

            template<class Operator>
            typename std::enable_if<Operator::arity==1, Val>::type
            operator()(Operator) const
            {
                if(args.size() != Operator::arity){
                    throw("apply_operator: invalid number of arguments");
                }
                return Operator()(args[0]);
            }
        };

        template<std::size_t InputSize>
        Val value_impl(node_ptr_type const node_ptr, std::array<Val, InputSize> const& variable_values) const
        {
            if(node_ptr->which() == 0){
                return boost::get<Val>(*node_ptr);
            }else if(node_ptr->which() == 1){
                std::vector<Val> args;
                for(auto const& child : boost::get<knot<Val>>(*node_ptr).children){
                    args.push_back(value_impl(child, variable_values));
                }
                return boost::apply_visitor(apply_operator(args), boost::get<knot<Val>>(*node_ptr).op);
            }else if(node_ptr->which() == 2){
                return variable_values[boost::get<Variable>(*node_ptr)];
            }else{
                throw("gene::tree::value_impl: invalid node value.");
            }
        }

        std::size_t depth_impl(node_ptr_type const node_ptr) const
        {
            auto which = node_ptr->which();
            if(which == 1){
                auto const& children = boost::get<knot<Val>>(*node_ptr).children;
                return std::accumulate(children.begin(), children.end(), 0,
                                         [this](std::size_t acc, node_ptr_type const& rhs)
                                         {
                                            auto depth = this->depth_impl(rhs);
                                            return acc < depth ? depth : acc;
                                         }) + 1;
            }else if(which == 0 || which == 2){
                return 0;
            }else{
                throw("gene::tree::depth_impl: invalid node value.");
            }
        }

    public:
        tree() : root(nullptr) {}
        tree(node_ptr_type p) : root(p) {}

        std::string expression() const
        {
            return expression_impl(root);
        }

        std::string to_string() const
        {
            return to_string_impl(root, 0);
        }

        template<std::size_t InputSize>
        Val value(std::array<Val, InputSize> const& variable_values) const
        {
            return value_impl(root, variable_values);
        }

        std::size_t depth() const
        {
            return depth_impl(root);
        }
    };

    namespace impl {

        template<class Val, std::size_t InputSize, class Generator>
        std::shared_ptr<node<Val>> generate_random_impl(std::size_t const max_depth, std::size_t const depth)
        {
            if(depth==max_depth){
                return std::make_shared<node<Val>>(Generator::generate_term());
            }

            double const probability_to_make_operator = (max_depth - 1.0) / max_depth;
            std::bernoulli_distribution has_operator(probability_to_make_operator);
            if(has_operator(config::random_engine)){
                knot<Val> knot_node(operators::random_op());
                typename knot<Val>::children_type children_;
                for(std::size_t i=0; i < knot_node.arity; ++i){
                    children_.push_back(generate_random_impl<Val, InputSize, Generator>(max_depth, depth+1));
                }
                knot_node.children = children_;
                return std::make_shared<node<Val>>(knot_node);
            }else{
                std::bernoulli_distribution has_constant(0.50);
                if(has_constant(config::random_engine)){
                    return std::make_shared<node<Val>>(Generator::generate_term());
                }else{
                    std::uniform_int_distribution<std::size_t> variable_number(0, InputSize-1);
                    return std::make_shared<node<Val>>(variable_number(config::random_engine));
                }
            }
        }

    } // namespace impl

    template<class Val, std::size_t InputSize, class RandomTermGenerator = random_term::default_random_term<Val>>
    inline tree<Val, RandomTermGenerator> generate_random(std::size_t const max_depth)
    {
        return {impl::generate_random_impl<Val, InputSize, RandomTermGenerator>(max_depth, 0)};
    }


} // namespace tree

} // namespace gene

#endif    // GENE_TREE_HPP_INCLUDED
