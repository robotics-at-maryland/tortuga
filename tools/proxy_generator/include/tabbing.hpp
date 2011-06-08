
#ifndef TABBING_HPP
#define TABBING_HPP

#include <fstream>
#include <list>
#include <string>

class tabbing {
private:
    static const size_t indent_const;

public:
    tabbing();

    void indent(size_t amount = indent_const);

    void unindent();

    friend std::ostream& operator<<(std::ostream& os, const tabbing& tab);

private:
    size_t indentation;
    std::list<size_t> m_indents;
};

#endif // TABBING_HPP
