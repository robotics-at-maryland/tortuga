
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "module_parser.hpp"

int main(int argc, char* argv[])
{
    const char *filename;
    if (argc > 1) {
        filename = argv[1];
    } else {
        std::cerr << "Error: No input file provided." << std::endl;
        return 1;
    }

    module_parser parser(filename);
    parser.add_header("Ice/Ice.h");

    if (parser.parse_module()) {
        parser.write_to(std::cout);
    }
}
