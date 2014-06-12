
#ifndef NAMESPACE_HPP
#define NAMESPACE_HPP

#include <fstream>
#include <list>
#include <string>

class namespace_ {
public:
    /** Enters a new namespace of <name> */
    void enter(std::string name);

    /** Exits the current namespace */
    void exit();

    std::string current_ns();

    namespace_ parent();

    friend std::ostream& operator<<(std::ostream& os, const namespace_& obj);

private:
    std::list<std::string> m_namespaces;
};

#endif // NAMESPACE_HPP
