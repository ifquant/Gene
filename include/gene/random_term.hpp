#if !defined GENE_RANDOM_TERM_HPP_INCLUDED
#define      GENE_RANDOM_TERM_HPP_INCLUDED

#include <type_traits>
#include <string>

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
            // TODO signed or floating point
            return Term();
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
            // TODO unsigned
            return Term();
        }
    };

    template<>
    class default_random_term<std::string>{
    public:
        static std::string generate_term()
        {
            // TODO string
            return "";
        }
    };

} // namespace random_term

} // namespace gene

#endif    // GENE_RANDOM_TERM_HPP_INCLUDED
