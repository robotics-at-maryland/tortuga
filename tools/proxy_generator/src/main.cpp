
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>

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
    parser.add_header("boost/shared_ptr.hpp");
    parser.add_header("Ice/Ice.h");
    parser.add_header("core/include/Subsystem.h");
    parser.add_header("core/include/GILock.h");
    parser.add_header("network/include/AdapterMaker.h");

    // Figure out the correct ice header based on the current one
    boost::filesystem::path header(filename);
    std::string header_file =
        boost::filesystem::change_extension(header.leaf(), ".h").string();
    parser.add_header(header_file);

    // Add any additional headers specific on the command line
    for (int i = 2; i < argc; i++) {
        parser.add_header(argv[i]);
    }

    if (parser.parse_module()) {
        parser.write_to(std::cout);
    } else {
        std::cout << "failed to parse module" << std::endl;
    }
}
