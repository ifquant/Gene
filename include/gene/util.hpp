#if !defined GENE_UTIL_HPP_INCLUDED
#define      GENE_UTIL_HPP_INCLUDED

#include "config.hpp"

#include <random>
#include <vector>

namespace gene {
namespace util {

    template< std::size_t... Indices >
    struct index_tuple{};

    template < std::size_t Start,
               std::size_t Last,
               std::size_t Step = 1,
               class Acc = index_tuple<>,
               bool Finish = (Start>=Last) >
    struct index_range{
        typedef Acc type;
    };

    template < std::size_t Start,
               std::size_t Last,
               std::size_t Step,
               std::size_t... Indices >
    struct index_range< Start, Last, Step, index_tuple<Indices...>, false >
             : index_range<Start+Step, Last, Step, index_tuple<Indices..., Start>>
    {};

    template < std::size_t Start, std::size_t Last, std::size_t Step = 1 >
    using idx_range = typename index_range< Start, Last, Step >::type;

    template<class T>
    T sample(std::vector<T> const& v)
    {
        std::uniform_int_distribution<std::size_t> random_index(0, v.size()-1);
        return v[random_index(config::random_engine)];
    }
} // namespace util
} // namespace gene
#endif    // GENE_UTIL_HPP_INCLUDED
