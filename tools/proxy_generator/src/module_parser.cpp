
#include <iostream>

#include <boost/spirit/include/qi.hpp>

#include "module_parser.hpp"
#include "slice_grammar.hpp"

module_parser::module_parser(const char *filename)
    : _filename(filename)
{
}

void module_parser::add_header(std::string header)
{
    main.headers.push_back(header);
}

bool module_parser::parse_module()
{
    std::ifstream in(_filename.c_str(), std::ios_base::in);
    if (!in)
    {
        std::cerr << "Error: Could not open input file: "
                  << _filename << std::endl;
    }

    std::string storage;
    in.unsetf(std::ios::skipws);
    std::copy(
        std::istream_iterator<char>(in),
        std::istream_iterator<char>(),
        std::back_inserter(storage));

    typedef std::string::const_iterator iterator;
    typedef slice_grammar<iterator> slice_parser;
    slice_parser parser;

    using boost::spirit::ascii::space;
    iterator iter = storage.begin();
    iterator end = storage.end();
    bool r = phrase_parse(iter, end, parser, space, main);

    if (r && iter == end) {
        return true;
    } else {
        std::cout << "failed to parse module" << std::endl;
        std::cout << std::string(iter, iter+30) << std::endl;
    }
    return r && iter == end;
}

void module_parser::write_to(std::ostream& os)
{
    os << main;
}
