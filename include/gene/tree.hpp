#if !defined GENE_TREE_HPP_INCLUDED
#define      GENE_TREE_HPP_INCLUDED

#include "config.hpp"
#include "node.hpp"
#include "operators.hpp"

#include <random>
#include <memory>

namespace gene {

namespace tree {

    template<class Val>
    class tree{
    public:
        typedef std::shared_ptr<node<Val>> node_ptr_type;

    private:
        std::shared_ptr<node<Val>> root;
        double fitness;

    public:
        tree() : root(nullptr), fitness(0.0) {}
        tree(node_ptr_type p) : root(p), fitness(0.0) {}

    };

    namespace impl {

        template<class Val>
        std::shared_ptr<node<Val>> generate_random_impl(int const max_depth, int const depth)
        {
            if(depth==max_depth){
                return std::make_shared(Val());
            }

            std::bernoulli_distribution has_operator(0.50);
            if(has_operator(config::random_engine)){
                op_container<Val> container(operators::random_op());
                typename op_container<Val>::children_type children_;
                for(int i=0; i < container.arity; ++i){
                    children_.push_back(generate_random_impl<Val>(max_depth, depth+1));
                }
                container.children = children_;
                return std::make_shared(container);
            }else{
                return std::make_shared(Val());
            }
        }

    } // namespace impl

    template<class Val>
    std::shared_ptr<node<Val>> generate_random(int const max_depth)
    {
        return impl::generate_random_impl<Val>(max_depth, 0);
    }


} // namespace tree

} // namespace gene

#endif    // GENE_TREE_HPP_INCLUDED
