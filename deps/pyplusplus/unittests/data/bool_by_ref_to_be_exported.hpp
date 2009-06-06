#include <stdexcept>
#include <iostream>

namespace tests{

class listener {
    public:
        virtual void listen(int id, const std::string& name, int& skip) { 
            throw std::runtime_error ( std::string ("Virtual function listener::listen called!") ); 
            }
};

bool callListener ( listener* myListener) {
    int skip = 10;
    std::cout << "C++: Calling myListener->listen\n";
    myListener->listen(100, "test", skip);
    std::cout << "C++: Called OK " << skip <<"\n";
    return skip;
}

}
