#if !defined GENE_OPERATORS_HPP_INCLUDED
#define      GENE_OPERATORS_HPP_INCLUDED

#include "config.hpp"

#include <random>

#include <cstddef>
#include <cmath>
#include <cstdlib>

#include <boost/variant.hpp>

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

        enum struct opset{
            plus, minus, mult, div, abs, sqrt
        };

        boost::variant<plus, minus, mult, divide, abs, sqrt>
        op(opset sym)
        {
            switch(sym){
            case opset::plus: return plus();
            case opset::minus: return minus();
            case opset::mult: return mult();
            case opset::div: return divide();
            case opset::abs: return abs();
            case opset::sqrt: return sqrt();
            default: throw("illegal operator");
            }
        }

        boost::variant<plus, minus, mult, divide, abs, sqrt>
        random_op()
        {
            std::uniform_int_distribution<int> dst( static_cast<int>(opset::plus),
                                                    static_cast<int>(opset::sqrt));
            return op(static_cast<opset>(dst(gene::config::random_engine)));
        }

    } // namespace operators

} // namespace tree

} // namespace gene
#endif    // GENE_OPERATORS_HPP_INCLUDED
