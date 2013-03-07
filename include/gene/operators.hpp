#if !defined GENE_OPERATORS_HPP_INCLUDED
#define      GENE_OPERATORS_HPP_INCLUDED
#include <cstddef>
#include <cmath>
#include <cstdlib>

namespace gene {

namespace tree {

    namespace operators {
        struct plus{
            static constexpr std::size_t arity = 2;

            template<class T>
            T operator()(T a, T b)
            {
                return a + b;
            }
        };

        struct minus{
            static constexpr std::size_t arity = 2;
            
            template<class T>
            T operator()(T a, T b)
            {
                return a - b;
            }
        };

        struct mult{
            static constexpr std::size_t arity = 2;
            
            template<class T>
            T operator()(T a, T b)
            {
                return a * b;
            }
        };

        struct divide{
            static constexpr std::size_t arity = 2;
            
            template<class T>
            T operator()(T a, T b)
            {
                return a / b;
            }
        };

        struct abs{
            static constexpr std::size_t arity = 1;
            
            template<class T>
            T operator()(T a)
            {
                return std::abs(a);
            }
        };

        struct sqrt{
            static constexpr std::size_t arity = 1;
            
            template<class T>
            T operator()(T a)
            {
                return std::sqrt(a);
            }
        };
    } // namespace operators

} // namespace tree

} // namespace gene
#endif    // GENE_OPERATORS_HPP_INCLUDED
