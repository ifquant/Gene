#if !defined GENE_NODE_HPP_INCLUDED
#define      GENE_NODE_HPP_INCLUDED

#include "operators.hpp"

#include <memory>
#include <vector>
#include <cstddef>

#include <boost/variant.hpp>

namespace gene {

namespace tree {

    typedef std::size_t Variable;

    std::string variable_name(Variable v)
    {
        return "x" + std::to_string(v);
    }

    template<class V>
    class knot;

    template<class Val>
    using node = boost::variant<Val, knot<Val>, Variable>;

    template<class V>
    class knot{
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
            typename operators::operator_type
            operator_type;
        typedef
            std::vector<std::shared_ptr<node<V>>>
            children_type;

    public:
        operator_type const op;
        std::size_t const arity;
        children_type children;

    public:
        knot(operator_type op_)
            : op(op_), arity(boost::apply_visitor(get_arity(), op_)), children()
        {}

        template<class... Args>
        void set_children(Args... children_ptrs)
        {
            if(sizeof...(Args) != arity){
                throw("number of children is invalid");
            }
            for(auto child_ptr : {children_ptrs...}){
                children.push_back(child_ptr);
            }
        }

    };

} // namespace tree

} // namespace gene

#endif    // GENE_NODE_HPP_INCLUDED
