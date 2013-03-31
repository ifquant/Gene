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

    template< class OutputType,
              std::size_t InputSize,
              std::size_t OutputSize,
              class RandomTermGenerator = random_term::default_random_term<OutputType> >
    class individual{

        template< class V, std::size_t In, std::size_t Out, class Rand>
        friend void mutation(individual<V, In, Out, Rand> ind);


    public:
        typedef tree::tree<OutputType, RandomTermGenerator> tree_type;
        typedef std::array<tree_type, OutputSize> trees_type;

    private:
        trees_type trees;

    public:
        OutputType fitness;

    public:
        individual(trees_type const& trees_) : trees(trees_), fitness() {}
        individual() : fitness()
        {
            for(auto &t : trees)
            {
                t = tree::generate_random<OutputType, InputSize, RandomTermGenerator>(config::random_tree_depth);
            }
        }

        std::string expressions() const
        {
            std::array<std::string, OutputSize> exprs;
            std::transform(trees.begin(), trees.end(), exprs.begin(),
                    [](tree_type const& tree){
                        return "[expr] " + tree.expression();
                    });
            return boost::algorithm::join(exprs, "\n");
        }

        std::string to_string() const
        {
            std::array<std::string, OutputSize> exprs;
            std::transform(trees.begin(), trees.end(), exprs.begin(),
                    [](tree_type const& tree){
                        return "[tree]\n" + tree.to_string();
                    });
            return boost::algorithm::join(exprs, "\n");
        }

        template<class Result = std::array<OutputType, OutputSize>>
        Result value(std::array<OutputType, InputSize> const& variable_values) const
        {
            Result values;
            std::transform(trees.begin(), trees.end(), values.begin(),
                    [&](tree_type t){
                        return t.value(variable_values);
                    });
            return values;
        }

        // TODO
        OutputType calc_fitness()
        {
            return 0.0;
        }
    };

    template< class OutputType,
              std::size_t InputSize,
              std::size_t OutputSize,
              class RandomTermGenerator = random_term::default_random_term<OutputType> >
    inline individual<OutputType, InputSize, OutputSize, RandomTermGenerator> generate_random()
    {
        std::array<tree::tree<OutputType, RandomTermGenerator>, OutputSize> trees;
        for(auto &t : trees){

            t = tree::generate_random<OutputType, InputSize, RandomTermGenerator>(config::random_tree_depth);
        }
        return {trees};
    }

    template< class OutputType,
              std::size_t InputSize,
              std::size_t OutputSize,
              class RandomTermGenerator >
    void mutation(individual<OutputType, InputSize, OutputSize, RandomTermGenerator> ind)
    {
        for( auto &t : ind.trees ){
            tree::mutation<InputSize>(t);
        }
    }

} // namespace individual

} // namespace gene
#endif    // GENE_INDIVIDUAL_HPP_INCLUDED
