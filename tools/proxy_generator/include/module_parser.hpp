
#ifndef MODULE_PARSER_HPP
#define MODULE_PARSER_HPP

#include <fstream>
#include <string>
#include "slice_types.hpp"

class module_parser {
public:
    module_parser(std::string filename);

    void add_header(std::string header);

    bool parse_module();

    void write_to(std::ostream& os);

private:
    program_node main;
    std::string _filename;
};

#endif // MODULE_PARSER_HPP
