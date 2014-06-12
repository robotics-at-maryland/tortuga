
// STD Includes
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>
#include <string.h>
#include <time.h>

// Project Includes
// For access to private lookup table
#define private public
#include "vision/include/LCHConverter.h"
#undef private

using namespace ram::vision;

inline bool verify(const unsigned char &expected,
            const unsigned char &actual,
            std::string message,
            bool crash = true);

bool verify(const unsigned char &expected,
            const unsigned char &actual,
            std::string message,
            bool crash)
{
    if (expected == actual) {
        return true;
    } else {
        std::cout << "\nFound " << (int) actual << " and expected "
                  << (int) expected << std::endl << message << std::endl;
        assert(!crash);
        return false;
    }
}

std::vector<std::string> split(std::string input, std::string delim)
{
    std::vector<std::string> strings;
    size_t pos = 0;
    size_t back = 0;
    while (std::string::npos != (back = input.find(delim, pos)))
    {
        strings.push_back(input.substr(pos, back-pos));
        pos = back + delim.size();
    }
    strings.push_back(input.substr(pos, std::string::npos));

    return strings;
}

int main(int argc, char* argv[])
{
    if (argc < 2 || (strcmp("-h", argv[1]) == 0 ||
                     strcmp("--help", argv[1]) == 0)) {
      help:
        std::cout << "Generates and tests the LCH lookup table\n\n"
            "\t--help,-h    \t\tDisplays this message\n"
            "\t-g,--generate\t\tGenerates the lookup table\n"
            "\t-t,--test    \t\tTests the lookup table loading\n"
            "\t-v,--verify  \t\tVerifies the lookup table\n"
            "\t-c,--convert \t\tConvert a single pixel (r,g,b)" << std::endl;
        return 1;
    }

    if (strcmp("-g", argv[1]) == 0 || strcmp("--generate", argv[1]) == 0) {
        std::cout << "Creating lookup table..." << std::endl
                  << "This will take awhile." << std::endl;
        LCHConverter::createLookupTable(true);
        std::cout << "Finished!" << std::endl;
    } else if (strcmp("-t", argv[1]) == 0 || strcmp("--test", argv[1]) == 0) {
        // Load the lookup table and time it
        std::cout << "Testing lookup table loading speed..." << std::endl;
        clock_t start = clock();
        LCHConverter::loadLookupTable();
        clock_t end = clock();
        std::cout << "Finished! Took " << (end - start)/(double)CLOCKS_PER_SEC
                  << " seconds" << std::endl;        
    } else if (strcmp("-v", argv[1]) == 0 || strcmp("--verify", argv[1]) == 0) {
        int counter = 0, size = 256 * 256 * 256;
        std::cout << "Loading lookup table from disk..." << std::endl;
        LCHConverter::loadLookupTable();
        std::cout << "Verifying lookup table..." << std::endl
                  << "This will take awhile." << std::endl;
        for (int ch1=0; ch1 < 256; ch1++) {
            for (int ch2=0; ch2 < 256; ch2++) {
                for (int ch3=0; ch3 < 256; ch3++) {
                    unsigned char r = ch1, g = ch2, b = ch3;
                    LCHConverter::convertPixel(r, g, b);

                    unsigned char *tablePos =
                        LCHConverter::rgb2lchLookup[ch1][ch2][ch3];

                    // Verify
                    verify(tablePos[0], r, "Incorrect conversion on channel 1");
                    verify(tablePos[1], g, "Incorrect conversion on channel 2");
                    verify(tablePos[2], b, "Incorrect conversion on channel 3");
                }
                std::cout << "\r" << counter << " / " << size;
                counter += 256;
            }
        }
        std::cout << std::endl;
    } else if (strcmp("-c", argv[1]) == 0 ||
               strcmp("--convert", argv[1]) == 0) {
        if (argc < 3) {
            std::cout << "Must specify an argument [r,g,b]" << std::endl;
            return 1;
        }
        std::vector<std::string> values = split(argv[2], ",");

        if (values.size() != 3) {
            std::cout << "Incorrect number of values!"
                " Must enter 3 numbers, seperated by commas" << std::endl;
            return 1;
        }

        unsigned char r = (unsigned char) atoi(values[0].c_str());
        unsigned char g = (unsigned char) atoi(values[1].c_str());
        unsigned char b = (unsigned char) atoi(values[2].c_str());
        
        LCHConverter::convertPixel(r, g, b);
        std::cout << (int) r << "," << (int) g << "," << (int) b << std::endl;
    } else {
        std::cout << "Invalid option: " << argv[1] << std::endl;
        goto help;
    }
    return 0;
}
