#ifndef __mailing_list_help_hpp__
#define __mailing_list_help_hpp__

#include <string>

class foo
{
public:

    foo() {std::strcpy(buffer, "good bye cruel world!\0\0");}
    char buffer[24];
};

class bar
{
public:
    foo f;
};

#endif//__mailing_list_help_hpp__
