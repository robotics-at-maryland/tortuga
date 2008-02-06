
#include "include/SharedPtrVector.h"

namespace samples
{

boost::shared_ptr<A> func()
{
    return boost::shared_ptr<A>(new A());
}

std::vector<A> funcVector()
{
    std::vector<A> items;
    items.push_back(A());
    items.push_back(A());
    return items;
}
    
std::vector<boost::shared_ptr<A> > funcVectorShared()
{
    std::vector<boost::shared_ptr<A> > items;
    items.push_back(boost::shared_ptr<A>(new A()));
    items.push_back(boost::shared_ptr<A>(new A()));
    return items;
}

}
