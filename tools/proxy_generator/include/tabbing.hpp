
#ifndef TABBING_HPP
#define TABBING_HPP

#include <fstream>

class tabbing {
private:
    size_t indentation;

public:
    tabbing();

    void indent();

    void unindent();

    friend std::ostream& operator<<(std::ostream& os, const tabbing& tab);
};

#endif // TABBING_HPP
