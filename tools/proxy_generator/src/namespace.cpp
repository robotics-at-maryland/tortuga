
#include <boost/foreach.hpp>
#include "namespace.hpp"

void namespace_::enter(std::string name)
{
    m_namespaces.push_back(name);
}

void namespace_::exit()
{
    m_namespaces.pop_back();
}

std::string namespace_::current_ns()
{
    return m_namespaces.back();
}

namespace_ namespace_::parent()
{
    // Copy over the current namespace
    namespace_ ns = *this;

    // Remove the last element
    ns.m_namespaces.pop_back();
    return ns;
}

std::ostream& operator<<(std::ostream& os, const namespace_& obj)
{
    BOOST_FOREACH(const std::string& s, obj.m_namespaces)
    {
        os << s << "::";
    }
    return os;
}
