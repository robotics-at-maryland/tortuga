
#include <string>
#include "tabbing.hpp"

static const size_t indent_const = 4;

tabbing::tabbing()
    : indentation(0)
{
}

void tabbing::indent()
{
    indentation += indent_const;
}

void tabbing::unindent()
{
    if (indent_const > indentation)
        indentation = 0;
    else
        indentation -= indent_const;
}

std::ostream& operator<<(std::ostream& os, const tabbing& tab)
{
    os << std::string(tab.indentation, ' ');
    return os;
}
