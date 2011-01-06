
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/algorithm/string/split.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/program_options.hpp>

#include "module_parser.hpp"

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    po::options_description desc("Allowed options");
    po::positional_options_description p;
    po::variables_map vm;

    std::string filename;
    std::string headers;
    std::string output;

    try
    {
        p.add("filename", 1);

        desc.add_options()
            ("help", "Produce help message")
            ("filename,f", po::value<std::string>(&filename),
             "Input file")
            ("headers,h", po::value<std::string>(&headers),
             "Additional header files")
            ("output,o", po::value<std::string>(&output),
             "Output source file")
            ;

        po::store(po::command_line_parser(argc, argv).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);
    }
    catch(std::exception& e)
    {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }

    if (vm.count("help"))
    {
        std::cout << "Usage: proxy_generator <input> [options]"
                  << std::endl << desc << std::endl;
        return 1;
    }

    if (filename == "") {
        std::cerr << "no input file" << std::endl;
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
    std::vector<std::string> additional_headers;
    boost::algorithm::split(additional_headers, headers, ';' == boost::lambda::_1);

    BOOST_FOREACH(const std::string& h, additional_headers)
    {
        parser.add_header(h);
    }

    if (parser.parse_module()) {
        parser.write_to(std::cout);
        return 0;
    } else {
        std::cout << "failed to parse module" << std::endl;
        return 1;
    }
}
