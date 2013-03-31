#if !defined GENE_INDIVIDUAL_HPP_INCLUDED
#define      GENE_INDIVIDUAL_HPP_INCLUDED

#include "config.hpp"
#include "tree.hpp"

#include <array>
#include <string>
#include <algorithm>

#include <boost/algorithm/string/join.hpp>

namespace gene {
namespace individual {

    template< class ValueType,
              std::size_t InputSize,
              std::size_t ValueSize,
              class RandomTermGenerator = random_term::default_random_term<ValueType> >
    class individual{

        template< class V, std::size_t In, std::size_t Out, class Rand>
        friend void mutation(individual<V, In, Out, Rand> ind);


    public:
        typedef tree::tree<ValueType, RandomTermGenerator> tree_type;
        typedef std::array<tree_type, ValueSize> trees_type;

    private:
        trees_type trees;

    public:
        ValueType fitness;

    public:
        individual(trees_type const& trees_) : trees(trees_), fitness() {}
        individual() : fitness()
        {
            for(auto &t : trees)
            {
                t = tree::generate_random<ValueType, InputSize, RandomTermGenerator>(config::random_tree_depth);
            }
        }

        std::string expressions() const
        {
            std::array<std::string, ValueSize> exprs;
            std::transform(trees.begin(), trees.end(), exprs.begin(),
                    [](tree_type const& tree){
                        return "[expr] " + tree.expression();
                    });
            return boost::algorithm::join(exprs, "\n");
        }

        std::string to_string() const
        {
            std::array<std::string, ValueSize> exprs;
            std::transform(trees.begin(), trees.end(), exprs.begin(),
                    [](tree_type const& tree){
                        return "[tree]\n" + tree.to_string();
                    });
            return boost::algorithm::join(exprs, "\n");
        }

        template<class Result = std::array<ValueType, ValueSize>>
        Result value(std::array<ValueType, InputSize> const& variable_values) const
        {
            Result values;
            std::transform(trees.begin(), trees.end(), values.begin(),
                    [&](tree_type t){
                        return t.value(variable_values);
                    });
            return values;
        }

        // TODO
        ValueType calc_fitness()
        {
            return 0.0;
        }
    };

    template< class ValueType,
              std::size_t InputSize,
              std::size_t ValueSize,
              class RandomTermGenerator = random_term::default_random_term<ValueType> >
    inline individual<ValueType, InputSize, ValueSize, RandomTermGenerator> generate_random()
    {
        std::array<tree::tree<ValueType, RandomTermGenerator>, ValueSize> trees;
        for(auto &t : trees){

            t = tree::generate_random<ValueType, InputSize, RandomTermGenerator>(config::random_tree_depth);
        }
        return {trees};
    }

    template< class ValueType,
              std::size_t InputSize,
              std::size_t ValueSize,
              class RandomTermGenerator >
    void mutation(individual<ValueType, InputSize, ValueSize, RandomTermGenerator> ind)
    {
        for( auto &t : ind.trees ){
            tree::mutation<InputSize>(t);
        }
    }

} // namespace individual

} // namespace gene
#endif    // GENE_INDIVIDUAL_HPP_INCLUDED
