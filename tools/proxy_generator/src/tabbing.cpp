
#include <string>
#include "tabbing.hpp"

const size_t tabbing::indent_const = 4;

tabbing::tabbing()
    : indentation(0)
{
}

void tabbing::indent(size_t amount)
{
    indentation += amount;
    m_indents.push_back(amount);
}

void tabbing::unindent()
{
    if (!m_indents.empty()) {
        indentation -= m_indents.back();
        m_indents.pop_back();
    }
}

std::ostream& operator<<(std::ostream& os, const tabbing& tab)
{
    os << std::string(tab.indentation, ' ');
    return os;
}
