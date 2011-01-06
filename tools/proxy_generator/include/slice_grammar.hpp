
#ifndef SLICE_GRAMMAR_HPP
#define SLICE_GRAMMAR_HPP

#include <string>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include "slice_types.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template <typename Iterator>
struct slice_grammar : qi::grammar<Iterator, program_node(), ascii::space_type>
{
    slice_grammar() : slice_grammar::base_type(program)
    {
        using ascii::char_;
        using ascii::string;

        using qi::lit;
        using qi::eps;
        using qi::_val;
        using qi::_1;
        using qi::lexeme;
        using qi::raw;

        using phoenix::at_c;
        using phoenix::push_back;

        program = *stmt[push_back(at_c<1>(_val), _1)];
        id_ %= +(char_('a', 'z') | char_('A', 'Z'));
        stmt = module_[_val = _1] | interface_[_val = _1];
        module_ = "module"
            >> id_[at_c<0>(_val) = _1]
            >> '{'
            >> *stmt[push_back(at_c<1>(_val), _1)]
            >> "};"
            ;
        interface_ = "interface"
            >> id_[at_c<0>(_val) = _1]
            >> '{'
            >> *interface_stmt[push_back(at_c<1>(_val), _1)]
            >> "};"
            ;
        interface_stmt = function_[_val = _1];
        function_ = type_[at_c<0>(_val) = _1]
            >> id_[at_c<1>(_val) = _1]
            >> '('
            >> arg_list[at_c<2>(_val) = _1]
            >> ')'
            >> ';';
        arg_list = arg[push_back(_val,  _1)] % ',';
        arg = function_type_[at_c<0>(_val) = _1] >> id_[at_c<1>(_val) = _1];
        type_ %= lexeme["void"] | function_type_;
        function_type_ = lexeme["int"][_val = "int"] | lexeme["string"][_val = "std::string"];
    }

    qi::rule<Iterator, program_node(), ascii::space_type> program;
    qi::rule<Iterator, node(), ascii::space_type> stmt;
    qi::rule<Iterator, module_node(), ascii::space_type> module_;
    qi::rule<Iterator, interface_node(), ascii::space_type> interface_;
    qi::rule<Iterator, std::string(), ascii::space_type> id_;
    qi::rule<Iterator, node(), ascii::space_type> interface_stmt;
    qi::rule<Iterator, function_node(), ascii::space_type> function_;
    qi::rule<Iterator, argument_list(), ascii::space_type> arg_list;
    qi::rule<Iterator, argument(), ascii::space_type> arg;
    qi::rule<Iterator, std::string(), ascii::space_type> type_;
    qi::rule<Iterator, std::string(), ascii::space_type> function_type_;
};

#endif // SLICE_GRAMMAR_HPP
