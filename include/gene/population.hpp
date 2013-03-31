#if !defined GENE_POPULATION_HPP_INCLUDED
#define      GENE_POPULATION_HPP_INCLUDED

#include "config.hpp"
#include "util.hpp"
#include "random_term.hpp"
#include "individual.hpp"

#include <cstddef>
#include <vector>
#include <array>
#include <tuple>

namespace gene {

template< class ValueType,
          std::size_t InputSize,
          std::size_t OutputSize,
          class RandomTermGenerator = random_term::default_random_term<ValueType> >
class population{
public:
    typedef individual::individual<ValueType, InputSize, OutputSize, RandomTermGenerator> individual_type;

private:
    std::vector< std::pair<
                    std::array<ValueType, InputSize>,
                    std::array<ValueType, OutputSize>
               > > training_data;
    std::vector<individual_type> individuals;
    std::size_t generation = 0;

private:
    template<class Tuple, std::size_t... Idx1, std::size_t... Idx2>
    void set_training_data_impl(std::vector<Tuple> const& data, util::index_tuple<Idx1...>, util::index_tuple<Idx2...>)
    {
        for(auto const& d : data){
            training_data.push_back({{std::get<Idx1>(d)...}, {std::get<Idx2>(d)...}});
        }
    }

public:
    population() : individuals(config::population_size){}

    template<class Tuple>
    void set_training_data(std::vector<Tuple> const& data)
    {
        set_training_data_impl(data, util::idx_range<0, InputSize>(), util::idx_range<InputSize, InputSize+OutputSize>());
    }

    std::size_t current_generation() const
    {
        return generation;
    }
};

} // namespace gene

#endif    // GENE_POPULATION_HPP_INCLUDED
