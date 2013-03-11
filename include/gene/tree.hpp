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
        double fitness;

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
                // node has terminal value
                return boost::lexical_cast<std::string>(boost::get<Val>(*node_ptr));
            } else if(node_ptr->which() == 1){
                // node has operator
                auto const& container = boost::get<op_container<Val>>(*node_ptr);
                std::vector<std::string> arg_strs(container.children.size());
                std::transform(container.children.begin(), container.children.end(), arg_strs.begin(),
                        [&](std::shared_ptr<node<Val>> n) {
                            return this->expression_impl(n);
                        });
                return boost::apply_visitor(operator_to_string(arg_strs), container.op);
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
                return indent(level) + "term: "
                    + boost::lexical_cast<std::string>(boost::get<Val>(*node_ptr)) + '\n';
            } else if(node_ptr->which() == 1){
                auto const& container = boost::get<op_container<Val>>(*node_ptr);
                std::string retval = indent(level) + boost::apply_visitor(operator_symbol(), container.op) + ":\n";
                return std::accumulate( container.children.begin(),
                                        container.children.end(),
                                        retval,
                                        [&](std::string acc, std::shared_ptr<node<Val>> n) {
                                            return acc + this->to_string_impl(n, level+1) + '\n';
                                        }
                                      );
            } else {
                throw("gene::tree::to_string_impl: invalid node value.");
            }
        }

    public:
        tree() : root(nullptr), fitness(0.0) {}
        tree(node_ptr_type p) : root(p), fitness(0.0) {}

        std::string expression() const
        {
            return expression_impl(root);
        }

        std::string to_string() const
        {
            return to_string_impl(root, 0);
        }
    };

    namespace impl {

        template<class Val, class Generator>
        std::shared_ptr<node<Val>> generate_random_impl(int const max_depth, int const depth)
        {
            if(depth==max_depth){
                return std::make_shared<node<Val>>(Generator::generate_term());
            }

            std::bernoulli_distribution has_operator(0.50);
            if(has_operator(config::random_engine)){
                op_container<Val> container(operators::random_op());
                typename op_container<Val>::children_type children_;
                for(std::size_t i=0; i < container.arity; ++i){
                    children_.push_back(generate_random_impl<Val, Generator>(max_depth, depth+1));
                }
                container.children = children_;
                return std::make_shared<node<Val>>(container);
            }else{
                return std::make_shared<node<Val>>(Generator::generate_term());
            }
        }

    } // namespace impl

    template<class Val, class RandomTermGenerator = random_term::default_random_term<Val>>
    inline tree<Val, RandomTermGenerator> generate_random(int const max_depth)
    {
        return {impl::generate_random_impl<Val, RandomTermGenerator>(max_depth, 0)};
    }


} // namespace tree

} // namespace gene

#endif    // GENE_TREE_HPP_INCLUDED
