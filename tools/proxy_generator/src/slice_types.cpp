
#include <sstream>
#include <boost/foreach.hpp>
#include "slice_types.hpp"
#include "namespace.hpp"
#include "tabbing.hpp"

static tabbing tab;
static namespace_ ns;

std::ostream& operator<<(std::ostream& os, const module_node& obj) {
    os << tab << "namespace " << obj.name << " {\n";
    
    // Emit internal children
    tab.indent();
    ns.enter(obj.name);
    BOOST_FOREACH(const node& b, obj.children)
    {
        os << b;
    }
    ns.exit();
    tab.unindent();
    
    os << tab << "}\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const interface_node& obj) {
    os << tab << "struct " << obj.name << "Proxy"
       << " : virtual public " << obj.name
       << " {\n";

    tab.indent();

    // Find the implementation class
    namespace_ parent = ns.parent();
    std::stringstream impl;
    impl << parent << obj.name;

    // Generate constructor
    os << tab << obj.name << "Proxy(ram::core::SubsystemPtr impl)\n"
       << tab << "  : m_impl(boost::dynamic_pointer_cast<"
       << impl.str() << ">(impl))\n"
       << tab << "{\n"
       << tab << "}\n\n";

    BOOST_FOREACH(const node& b, obj.children)
    {
        os << b << "\n";
    }

    // Private members
    os << tab << "boost::shared_ptr<" << impl.str() << "> m_impl;\n";

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

    os << "const Ice::Current &) {\n";

    tab.indent();
    os << tab << "ram::core::ScopedGILock lock;\n"
       << tab << "m_impl->" << obj.name << "(";

    argument_list::const_iterator iter = obj.arguments.begin();
    argument_list::const_iterator end = obj.arguments.end();

    if (iter != end) {
        os << iter->name;
    }

    for ( ; iter != end; iter++) {
        os << ", " << iter->name;
    }
    os << ");\n";

    tab.unindent();
    os << tab << "}\n";

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
