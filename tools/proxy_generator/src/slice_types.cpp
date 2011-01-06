
#include <boost/foreach.hpp>
#include "slice_types.hpp"
#include "tabbing.hpp"

static tabbing tab;

std::ostream& operator<<(std::ostream& os, const module_node& obj) {
    os << tab << "namespace " << obj.name << " {\n";
    
    // Emit internal children
    tab.indent();
    BOOST_FOREACH(const node& b, obj.children)
    {
        os << b;
    }
    tab.unindent();
    
    os << tab << "}\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const interface_node& obj) {
    os << tab << "struct " << obj.name << " {\n";

    tab.indent();
    BOOST_FOREACH(const node& b, obj.children)
    {
        os << b;
    }
    tab.unindent();

    os << tab << "};\n";
    return os;
}

std::ostream& operator<<(std::ostream &os, const argument& obj) {
    os << obj.type << " " << obj.name;
    return os;
}

std::ostream& operator<<(std::ostream& os, const function_node& obj) {
    os << tab
       << obj.type
       << " "
       << obj.name
       << "(";

    BOOST_FOREACH(const argument& arg, obj.arguments)
    {
        os << arg << ", ";
    }

    os << "Ice::Current&);\n";
    return os;
}

std::ostream& operator<<(std::ostream &os, const program_node& obj) {
    BOOST_FOREACH(const std::string& file, obj.headers)
    {
        os << "#include \"" << file << "\"\n";
    }

    os << "\n";

    BOOST_FOREACH(const node& n, obj.children)
    {
        os << n << "\n";
    }
    return os;
}
