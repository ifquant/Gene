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

    template<class Value, std::size_t OutputSize, class RandomTermGenerator = random_term::default_random_term<Value>>
    class individual{
    public:
        typedef tree::tree<Value, RandomTermGenerator> tree_type;
        typedef std::array<tree_type, OutputSize> trees_type;

    private:
        trees_type trees;

    public:
        Value fitness;

    public:
        individual(trees_type const& trees_) : trees(trees_) {}

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
                        return "[tree] " + tree.to_string();
                    });
            return boost::algorithm::join(exprs, "\n");
        }

        // std::string calc_fitness() const
    };

    template<class Value, std::size_t InputSize, std::size_t OutputSize, class RandomTermGenerator = random_term::default_random_term<Value>>
    inline individual<Value, OutputSize, RandomTermGenerator> generate_random()
    {
        std::array<tree::tree<Value, RandomTermGenerator>, OutputSize> trees;
        for(auto &t : trees){
            t = tree::generate_random<Value, InputSize, RandomTermGenerator>(config::random_tree_depth);
        }
        return {trees};
    }

} // namespace individual

} // namespace gene
#endif    // GENE_INDIVIDUAL_HPP_INCLUDED
