#if !defined GENE_OPERATORS_HPP_INCLUDED
#define      GENE_OPERATORS_HPP_INCLUDED

#include "config.hpp"
#include "node.hpp"

#include <vector>
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
        };

        struct minus{
            static constexpr std::size_t arity = 2;

            template<class T>
            T operator()(T const a, T const b) const
            {
                return a - b;
            }
        };

        struct mult{
            static constexpr std::size_t arity = 2;

            template<class T>
            T operator()(T const a, T const b) const
            {
                return a * b;
            }
        };

        struct divide{
            static constexpr std::size_t arity = 2;

            template<class T>
            T operator()(T const a, T const b) const
            {
                return a / b;
            }
        };

        struct abs{
            static constexpr std::size_t arity = 1;

            template<class T>
            T operator()(T const a) const
            {
                return std::abs(a);
            }
        };

        struct sqrt{
            static constexpr std::size_t arity = 1;

            template<class T>
            T operator()(T const a) const
            {
                return std::sqrt(a);
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


        template<class V>
        class op_container{
        private:
            struct get_arity : boost::static_visitor<std::size_t>{
                template<class Operator>
                std::size_t operator()(Operator const&) const
                {
                    return Operator::arity;
                }
            };
        public:
            typedef
                boost::variant<plus, minus, mult, divide, abs, sqrt>
                operator_type;
            typedef
                std::vector<std::shared_ptr<node<V>>>
                children_type;
        private:
            operator_type const op;
            children_type children;
            std::size_t const arity;
        public:
            op_container(operator_type op_)
                : op(op_), arity(boost::apply_visitor(get_arity(), op_)), children(arity)
            {
            }

        };
    } // namespace operators

} // namespace tree

} // namespace gene
#endif    // GENE_OPERATORS_HPP_INCLUDED
