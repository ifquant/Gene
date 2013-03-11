#if !defined GENE_RANDOM_TERM_HPP_INCLUDED
#define      GENE_RANDOM_TERM_HPP_INCLUDED

#include <type_traits>
#include <string>

namespace gene {

namespace random_term {

    template<class Term, class Enable = void>
    class default_random_term;

    template<class Term>
    class default_random_term< Term,
                               typename std::enable_if<std::is_arithmetic<Term>::value>::type
                             >{
        static Term generate_term()
        {
            return Term();
        }
    };

    template<>
    class default_random_term<std::string>{

        static std::string generate_term()
        {
            return "";
        }
    };

} // namespace random_term

} // namespace gene

#endif    // GENE_RANDOM_TERM_HPP_INCLUDED
