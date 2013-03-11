#if !defined GENE_RANDOM_TERM_HPP_INCLUDED
#define      GENE_RANDOM_TERM_HPP_INCLUDED

#include "config.hpp"

#include <array>
#include <type_traits>
#include <string>
#include <cstddef>

namespace gene {

namespace random_term {

    template<class Term, class Enable = void>
    class default_random_term;

    template<class Term>
    class default_random_term<
                Term,
                typename std::enable_if<
                        std::is_arithmetic<Term>::value &&
                        std::is_signed<Term>::value
                >::type
          > {
    public:
        static Term generate_term()
        {
            static std::array<int, 14> const cardinals = {{ -1000000,
                                                             -100000,
                                                              -10000,
                                                               -1000,
                                                                -100,
                                                                 -10,
                                                                  -1,
                                                                   1,
                                                                  10,
                                                                 100,
                                                                1000,
                                                               10000,
                                                              100000,
                                                             1000000  }};
            std::uniform_real_distribution<double> sig_dst(1.0, 10.0);
            double sig = sig_dst(config::random_engine);
            std::uniform_int_distribution<std::size_t> idx_dst(0, 13);
            std::size_t idx = idx_dst(config::random_engine);
            return static_cast<Term>(sig * cardinals[idx]);
        }
    };

    template<class Term>
    class default_random_term<
                Term,
                typename std::enable_if<
                        std::is_integral<Term>::value &&
                        std::is_unsigned<Term>::value
                >::type
          > {
    public:
        static Term generate_term()
        {
            static std::array<std::size_t, 7> const cardinals = {{       1,
                                                                        10,
                                                                       100,
                                                                      1000,
                                                                     10000,
                                                                    100000,
                                                                   1000000  }};
            std::uniform_real_distribution<double> sig_dst(1.0, 10.0);
            double sig = sig_dst(config::random_engine);
            std::uniform_int_distribution<std::size_t> idx_dst(0, 6);
            std::size_t idx = idx_dst(config::random_engine);
            return static_cast<Term>(sig * cardinals[idx]);
        }
    };

    template<>
    class default_random_term<std::string>{
    public:
        static std::string generate_term()
        {
            std::uniform_int_distribution<char> char_dst(0x20, 0x7e);
            std::uniform_int_distribution<std::size_t> size_dst(1, 1000);
            std::size_t const size = size_dst(config::random_engine);
            std::string retval;

            for(std::size_t i = 0; i < size; ++i){
                retval += char_dst(config::random_engine);
            }

            return retval;
        }
    };

} // namespace random_term

} // namespace gene

#endif    // GENE_RANDOM_TERM_HPP_INCLUDED
