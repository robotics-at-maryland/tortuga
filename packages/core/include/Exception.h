
#ifndef RAM_CORE_EXCEPTION_01_21_2010
#define RAM_CORE_EXCEPTION_01_21_2010

// STD Includes
#include <exception>

namespace ram {
namespace core {

class MakerNotFoundException : public std::exception
{
public:
    virtual const char* what() const throw()
    {
	return "Could not find maker";
    }
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_EXCEPTION_01_21_2010
