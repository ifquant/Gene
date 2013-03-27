#if !defined GENE_POPULATION_HPP_INCLUDED
#define      GENE_POPULATION_HPP_INCLUDED

#include "config.hpp"
#include "util.hpp"
#include "individual.hpp"

#include <cstddef>
#include <vector>
#include <array>
#include <tuple>

namespace gene {

template< class Output, class Input>
class population{
public:
    typedef typename Output::value_type output_type;
    typedef typename Input::value_type input_type;
    typedef individual::individual<output_type, Output::size, Input::size> individual_type;

private:
    std::vector< std::pair<
                    std::array<output_type, Output::size>,
                    std::array<input_type, Input::size>
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
        set_training_data_impl(data, util::idx_range<0, Output::size>(), util::idx_range<Output::size, Output::size+Input::size>());
    }

    std::size_t current_generation() const
    {
        return generation;
    }
};

} // namespace gene

#endif    // GENE_POPULATION_HPP_INCLUDED
