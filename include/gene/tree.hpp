#if !defined GENE_TREE_HPP_INCLUDED
#define      GENE_TREE_HPP_INCLUDED

#include "config.hpp"
#include "util.hpp"
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

    template<class ValueType, class RandomTermGenerator = random_term::default_random_term<ValueType>>
    class tree{
    public:
        typedef std::shared_ptr<node<ValueType>> node_ptr_type;

    private:
        std::shared_ptr<node<ValueType>> root;

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
            if(node_ptr->which() == constant_value){
                // when node has terminal value
                return boost::lexical_cast<std::string>(boost::get<ValueType>(*node_ptr));
            } else if(node_ptr->which() == knot_value){
                // when node has operator
                auto const& knot_node = boost::get<knot<ValueType>>(*node_ptr);
                std::vector<std::string> arg_strs(knot_node.children.size());
                std::transform(knot_node.children.begin(), knot_node.children.end(), arg_strs.begin(),
                        [&](std::shared_ptr<node<ValueType>> n) {
                            return this->expression_impl(n);
                        });
                return boost::apply_visitor(operator_to_string(arg_strs), knot_node.op);
            }else if(node_ptr->which() == variable_value){
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
            if(node_ptr->which() == constant_value){
                return indent(level) + "const: "
                    + boost::lexical_cast<std::string>(boost::get<ValueType>(*node_ptr)) + '\n';
            } else if(node_ptr->which() == knot_value){
                auto const& knot_node = boost::get<knot<ValueType>>(*node_ptr);
                std::string retval = indent(level) + boost::apply_visitor(operator_symbol(), knot_node.op) + ":\n";
                return std::accumulate( knot_node.children.begin(),
                                        knot_node.children.end(),
                                        retval,
                                        [&](std::string acc, std::shared_ptr<node<ValueType>> n) {
                                            return acc + this->to_string_impl(n, level+1) + '\n';
                                        }
                                      );
            }else if(node_ptr->which() == variable_value){
                return indent(level) + "var: "
                    + variable_name(boost::get<Variable>(*node_ptr));
            }else {
                throw("gene::tree::to_string_impl: invalid node value.");
            }
        }


        struct apply_operator : boost::static_visitor<ValueType> {
            std::vector<ValueType> const& args;
            apply_operator(std::vector<ValueType> const& args_) : args(args_) {}

            template<class Operator>
            typename std::enable_if<Operator::arity==2, ValueType>::type
            operator()(Operator) const
            {
                if(args.size() != Operator::arity){
                    throw("apply_operator: invalid number of arguments");
                }
                return Operator()(args[0], args[1]);
            }

            template<class Operator>
            typename std::enable_if<Operator::arity==1, ValueType>::type
            operator()(Operator) const
            {
                if(args.size() != Operator::arity){
                    throw("apply_operator: invalid number of arguments");
                }
                return Operator()(args[0]);
            }
        };

        template<std::size_t InputSize>
        ValueType value_impl(node_ptr_type const node_ptr, std::array<ValueType, InputSize> const& variable_values) const
        {
            if(node_ptr->which() == constant_value){
                return boost::get<ValueType>(*node_ptr);
            }else if(node_ptr->which() == knot_value){
                std::vector<ValueType> args;
                for(auto const& child : boost::get<knot<ValueType>>(*node_ptr).children){
                    args.push_back(value_impl(child, variable_values));
                }
                return boost::apply_visitor(apply_operator(args), boost::get<knot<ValueType>>(*node_ptr).op);
            }else if(node_ptr->which() == variable_value){
                return variable_values[boost::get<Variable>(*node_ptr)];
            }else{
                throw("gene::tree::value_impl: invalid node value.");
            }
        }

        std::size_t depth_impl(node_ptr_type const node_ptr) const
        {
            auto which = node_ptr->which();
            if(which == knot_value){
                auto const& children = boost::get<knot<ValueType>>(*node_ptr).children;
                return std::accumulate(children.begin(), children.end(), 0,
                                         [this](std::size_t acc, node_ptr_type const& rhs)
                                         {
                                            auto depth = this->depth_impl(rhs);
                                            return acc < depth ? depth : acc;
                                         }) + 1;
            }else if(which == constant_value || which == variable_value){
                return 0;
            }else{
                throw("gene::tree::depth_impl: invalid node value.");
            }
        }

        node_ptr_type anywhere_impl(node_ptr_type node, std::size_t const depth) const
        {
            double const probability_to_decide_here = 1.0 / depth;
            std::bernoulli_distribution return_here(probability_to_decide_here);
            if(return_here(config::random_engine)){
                return node;
            }else{
                auto which = node->which();
                if(which == knot_value){
                    return anywhere_impl(util::sample(boost::get<knot<ValueType>>(*node).children), depth);
                }else if(which == constant_value || which == variable_value){
                    return node;
                }else{
                    throw("gene::tree::anywhere_impl: invalid node value.");
                }
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
        ValueType value(std::array<ValueType, InputSize> const& variable_values) const
        {
            return value_impl(root, variable_values);
        }

        std::size_t depth() const
        {
            return depth_impl(root);
        }

        node_ptr_type anywhere() const
        {
            return anywhere_impl(root, depth());
        }

        
    };

    namespace impl {

        template<class ValueType, std::size_t InputSize, class Generator>
        std::shared_ptr<node<ValueType>> random_partial_tree(std::size_t const max_depth, std::size_t const depth)
        {
            if(depth==max_depth){
                return std::make_shared<node<ValueType>>(Generator::generate_term());
            }

            double const probability_to_make_operator = (max_depth - 1.0) / max_depth;
            std::bernoulli_distribution has_operator(probability_to_make_operator);
            if(has_operator(config::random_engine)){
                knot<ValueType> knot_node(operators::random_op());
                typename knot<ValueType>::children_type children_;
                for(std::size_t i=0; i < knot_node.arity; ++i){
                    children_.push_back(random_partial_tree<ValueType, InputSize, Generator>(max_depth, depth+1));
                }
                knot_node.children = children_;
                return std::make_shared<node<ValueType>>(knot_node);
            }else{
                std::bernoulli_distribution has_constant(0.50);
                if(has_constant(config::random_engine)){
                    return std::make_shared<node<ValueType>>(Generator::generate_term());
                }else{
                    std::uniform_int_distribution<std::size_t> variable_number(0, InputSize-1);
                    return std::make_shared<node<ValueType>>(variable_number(config::random_engine));
                }
            }
        }

    } // namespace impl

    template<class ValueType, std::size_t InputSize, class RandomTermGenerator = random_term::default_random_term<ValueType>>
    inline tree<ValueType, RandomTermGenerator> generate_random(std::size_t const max_depth)
    {
        return {impl::random_partial_tree<ValueType, InputSize, RandomTermGenerator>(max_depth, 0)};
    }

    template<std::size_t InputSize, class ValueType, class RandomTermGen>
    void mutation(tree<ValueType, RandomTermGen> &t)
    {
        auto anywhere_ptr = t.anywhere();
        auto new_partial_tree = impl::random_partial_tree<ValueType, InputSize, RandomTermGen>(config::random_tree_depth, 0);
        *anywhere_ptr = *new_partial_tree;
    }

    template<class ValueType, class RandomTermGen>
    void crossover(tree<ValueType, RandomTermGen> &lhs, tree<ValueType, RandomTermGen> &rhs)
    {
        auto lhs_anywhere = lhs.anywhere();
        auto rhs_anywhere = rhs.anywhere();
        lhs_anywhere->swap(*rhs_anywhere);
    }

} // namespace tree

} // namespace gene

#endif    // GENE_TREE_HPP_INCLUDED
