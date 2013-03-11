#if !defined GENE_OPERATORS_HPP_INCLUDED
#define      GENE_OPERATORS_HPP_INCLUDED

#include "config.hpp"

#include <vector>
#include <string>
#include <memory>
#include <random>

#include <cstddef>
#include <cmath>
#include <cstdlib>

#include <boost/variant.hpp>
#include <boost/variant/static_visitor.hpp>

namespace gene {

namespace tree {

    namespace operators {

        struct plus{
            static constexpr std::size_t arity = 2;

            template<class T>
            T operator()(T const a, T const b) const
            {
                return a + b;
            }

            std::string to_string(std::vector<std::string> const& children_strs) const
            {
                if(children_strs.size()!=arity){
                    throw("plus::to_string: children size is invalid");
                }
                return children_strs[0] + " + " + children_strs[1];
            }
        };

        struct minus{
            static constexpr std::size_t arity = 2;

            template<class T>
            T operator()(T const a, T const b) const
            {
                return a - b;
            }

            std::string to_string(std::vector<std::string> const& children_strs) const
            {
                if(children_strs.size()!=arity){
                    throw("plus::to_string: children size is invalid");
                }
                return children_strs[0] + " - " + children_strs[1];
            }
        };

        struct mult{
            static constexpr std::size_t arity = 2;

            template<class T>
            T operator()(T const a, T const b) const
            {
                return a * b;
            }

            std::string to_string(std::vector<std::string> const& children_strs) const
            {
                if(children_strs.size()!=arity){
                    throw("plus::to_string: children size is invalid");
                }
                return children_strs[0] + " * " + children_strs[1];
            }
        };

        struct divide{
            static constexpr std::size_t arity = 2;

            template<class T>
            T operator()(T const a, T const b) const
            {
                return a / b;
            }

            std::string to_string(std::vector<std::string> const& children_strs) const
            {
                if(children_strs.size()!=arity){
                    throw("plus::to_string: children size is invalid");
                }
                return children_strs[0] + " / " + children_strs[1];
            }
        };

        struct abs{
            static constexpr std::size_t arity = 1;

            template<class T>
            T operator()(T const a) const
            {
                return std::abs(a);
            }

            std::string to_string(std::vector<std::string> const& children_strs) const
            {
                if(children_strs.size()!=arity){
                    throw("plus::to_string: children size is invalid");
                }
                return "abs( " + children_strs[0] + " )";
            }
        };

        struct sqrt{
            static constexpr std::size_t arity = 1;

            template<class T>
            T operator()(T const a) const
            {
                return std::sqrt(a);
            }

            std::string to_string(std::vector<std::string> const& children_strs) const
            {
                if(children_strs.size()!=arity){
                    throw("plus::to_string: children size is invalid");
                }
                return "sqrt( " + children_strs[0] + " )";
            }
        };

        enum struct opset{
            plus, minus, mult, div, abs, sqrt
        };

        boost::variant<plus, minus, mult, divide, abs, sqrt>
        op(opset const sym)
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

        typedef
            boost::variant< operators::plus,
                            operators::minus,
                            operators::mult,
                            operators::divide,
                            operators::abs,
                            operators::sqrt >
            operator_type;

    } // namespace operators

} // namespace tree

} // namespace gene
#endif    // GENE_OPERATORS_HPP_INCLUDED
