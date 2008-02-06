#include <iostream>

int main()
{
    std::cout << "Bye World" << std::endl;
    
#ifdef _DEBUG
    std::cout << "I am beging debugged" << std::endl;
#endif
    
    return 0;
}