
#ifndef SLICE_TYPES_HPP
#define SLICE_TYPES_HPP

#include <fstream>
#include <string>
#include <vector>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>

struct program_node;
struct module_node;
struct interface_node;
struct function_node;

typedef boost::variant< boost::recursive_wrapper<module_node>,
                        boost::recursive_wrapper<interface_node>,
                        boost::recursive_wrapper<function_node>
                        > node;

struct program_node {
    std::vector<std::string> headers;
    std::vector<node> children;

    friend std::ostream& operator<<(std::ostream &os, const program_node& obj);
};

BOOST_FUSION_ADAPT_STRUCT(
    program_node,
    (std::vector<std::string>, headers)
    (std::vector<node>, children)
)

struct module_node {
    std::string name;
    std::vector<node> children;

    friend std::ostream& operator<<(std::ostream &os, const module_node& obj);
};

BOOST_FUSION_ADAPT_STRUCT(
    module_node,
    (std::string, name)
    (std::vector<node>, children)
)

struct interface_node {
    std::string name;
    std::vector<node> children;

    friend std::ostream& operator<<(std::ostream &os, const interface_node& obj);
};

BOOST_FUSION_ADAPT_STRUCT(
    interface_node,
    (std::string, name)
    (std::vector<node>, children)
)

struct argument {
    std::string type;
    std::string name;

    friend std::ostream& operator<<(std::ostream &os, const argument& obj);
};
typedef std::vector<argument> argument_list;

BOOST_FUSION_ADAPT_STRUCT(
    argument,
    (std::string, type)
    (std::string, name)
)

struct function_node {
    std::string type;
    std::string name;
    argument_list arguments;

    friend std::ostream& operator<<(std::ostream &os, const function_node& obj);
};

BOOST_FUSION_ADAPT_STRUCT(
    function_node,
    (std::string, type)
    (std::string, name)
    (argument_list, arguments)
)

#endif // SLICE_TYPES_HPP
