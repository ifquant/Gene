#if !defined GENE_TREE_HPP_INCLUDED
#define      GENE_TREE_HPP_INCLUDED

#include "config.hpp"
#include "node.hpp"
#include "operators.hpp"

#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <memory>
#include <cstddef>

#include <boost/lexical_cast.hpp>
#include <boost/variant/static_visitor.hpp>

namespace gene {

namespace tree {

    template<class Val>
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

        std::string to_string_impl(node_ptr_type const node_ptr, int const level) const
        {
            if(node_ptr->which() == 0){
                // node has terminal value
                return std::string(level * config::indent_width, ' ') + boost::lexical_cast<std::string>(boost::get<Val>(*node_ptr));
            } else if(node_ptr->which() == 1){
                // node has operator
                auto const& container = boost::get<op_container<Val>>(*node_ptr);
                std::vector<std::string> arg_strs(container.children.size());
                std::transform(container.children.begin(), container.children.end(), arg_strs.begin(),
                        [&](std::shared_ptr<node<Val>> n) {
                            return this->to_string_impl(n, level + 1);
                        });
                return std::string(level * config::indent_width, ' ') + boost::apply_visitor(operator_to_string(arg_strs), container.op);
            } else {
                throw("gene::tree::to_string_impl: invalid node value.");
            }
        }

    public:
        tree() : root(nullptr), fitness(0.0) {}
        tree(node_ptr_type p) : root(p), fitness(0.0) {}

        std::string to_string() const
        {
            return to_string_impl(root, 0);
        }
    };

    namespace impl {

        template<class Val>
        std::shared_ptr<node<Val>> generate_random_impl(int const max_depth, int const depth)
        {
            if(depth==max_depth){
                return std::make_shared<node<Val>>(Val());
            }

            std::bernoulli_distribution has_operator(0.50);
            if(has_operator(config::random_engine)){
                op_container<Val> container(operators::random_op());
                typename op_container<Val>::children_type children_;
                for(std::size_t i=0; i < container.arity; ++i){
                    children_.push_back(generate_random_impl<Val>(max_depth, depth+1));
                }
                container.children = children_;
                return std::make_shared<node<Val>>(container);
            }else{
                return std::make_shared<node<Val>>(Val());
            }
        }

    } // namespace impl

    template<class Val>
    inline tree<Val> generate_random(int const max_depth)
    {
        return {impl::generate_random_impl<Val>(max_depth, 0)};
    }


} // namespace tree

} // namespace gene

#endif    // GENE_TREE_HPP_INCLUDED