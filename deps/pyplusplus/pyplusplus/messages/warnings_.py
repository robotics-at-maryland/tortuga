# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""This package defines all user messages (warnings + errors), which will be 
reported to user.
"""

W1000 = 'Py++, by default, does not expose internal compilers declarations. '\
        'Names of those declarations usually start with "__".'

W1001 = 'Py++, by default, does not expose internal declarations. ' \
        'GCC-XML reports that these declaration belong to "<internal>" header.'

W1002 = 'Py++, by default, does not expose compiler generated declarations.'

W1003 = 'Virtual functions that returns const reference cannot be overridden from Python. ' \
        'Reason: boost::python::override::operator()(...) saves the result of the marshaling ' \
        '(from Python to C++) on the stack. Thus operator() returns reference ' \
        'to a temporary variable. Consider to use "Function Transformation" functionality ' \
        'to solve the problem.'

W1004 = 'Boost.Python library can not expose function, which takes as argument/returns ' \
        'pointer to function. ' \
        ' See http://www.boost.org/libs/python/doc/v2/faq.html#funcptr for more information.'

W1005 = 'Py++ cannot expose function that takes as argument/returns instance of non-public class. ' \
        'Generated code will not compile.'

W1006 = 'Py++ need your help to expose function that takes as argument/returns C++ arrays. ' \
        'Take a look on "Function Transformation" functionality and define the transformation.'

W1007 = 'The function has more than %d arguments ( %d ). ' \
        'You should adjust BOOST_PYTHON_MAX_ARITY macro. ' \
        'For more information see: http://www.boost.org/libs/python/doc/v2/configuration.html'

W1008 = 'The function returns non-const reference to "Python immutable" type. ' \
        'The value cannot be modified from Python. '

W1009 = 'The function takes as argument (name=%s, pos=%d) non-const reference ' \
        'to Python immutable type - function could not be called from Python. ' \
         'Take a look on "Function Transformation" functionality and define the transformation.'

W1010 = 'The function introduces registration order problem. ' \
        'For more information about the problem read next document: ' \
        'http://language-binding.net/pyplusplus/documentation/functions/registration_order.html ' \
        'Problematic functions list: %s'

W1011 = "Py++ doesn't export private not virtual functions."

W1012 = 'Py++ does not exports compiler generated constructors.'

W1013 = "Py++ doesn't export private constructor."        

W1014 = '"%s" is not supported. ' \
        'See Boost.Python documentation: http://www.boost.org/libs/python/doc/v2/operators.html#introduction.'

W1015 = "Py++ doesn't export private operators."        

W1016 = 'Py++ does not exports non-const casting operators with user defined type as return value. ' \
        'This could be change in future.'

W1017 = "Py++ doesn't export non-public casting operators."                    

W1018 = 'Py++ can not expose unnamed classes.'

W1019 = 'Py++ can not expose private class.'

W1020 = "Py++ will generate class wrapper - hand written code should be added to the wrapper class"

W1021 = "Py++ will generate class wrapper - hand written code should be added to the wrapper class null constructor body" 

W1022 = "Py++ will generate class wrapper - hand written code should be added to the wrapper class copy constructor body"

W1023 = "Py++ will generate class wrapper - there are few functions that should be redefined in class wrapper"

W1024 = "Py++ will generate class wrapper - class contains bit field member variable"

W1025 = "Py++ will generate class wrapper - class contains T* member variable"

W1026 = "Py++ will generate class wrapper - class contains T& member variable"

W1027 = "Py++ will generate class wrapper - class contains array member variable"

W1028 = "Py++ will generate class wrapper - class contains definition of nested class that requires wrapper class"

W1029 = "Py++ will generate class wrapper - hand written code should be added to the wrapper class constructor body"

W1030 = "Py++ will generate class wrapper - class contains definition of virtual or pure virtual member function"

W1031 = "Py++ will generate class wrapper - user asked to expose non - public member function"

W1032 = "Boost.Python library does not support enums with duplicate values. " \
        "You can read more about this here: " \
        "http://boost.org/libs/python/todo.html#support-for-enums-with-duplicate-values . " \
        "The quick work around is to add new class variable to the exported enum, from Python. "

W1033 = "Py++ can not expose unnamed variables"

W1034 = "Py++ can not expose alignment bit."

W1035 = "Py++ can not expose static pointer member variables. This could be changed in future."

W1036 = "Py++ can not expose pointer to Python immutable member variables. This could be changed in future."

W1037 = "Boost.Python library can not expose variables, which are pointer to function." \
        " See http://www.boost.org/libs/python/doc/v2/faq.html#funcptr for more information."

W1038 = "Py++ can not expose variables of with unnamed type."

W1039 = "Py++ doesn't expose private or protected member variables."

W1040 = 'The declaration is unexposed, but there are other declarations, which refer to it. ' \
        'This could cause "no to_python converter found" run time error. ' \
        'Declarations: %s'

W1041 = 'Property "%s" could not be created. There is another exposed declaration with the same name( alias )." ' \
        'The property will make it inaccessible.'

W1042 = 'Py++ can not find out container value_type( mapped_type ). ' \
        'The container class is template instantiation declaration and not definition. ' \
        'This container class will be exported, but there is a possibility, that ' \
        'generated code will not compile or will lack some functionality. ' \
        'The solution to the problem is to create a variable of the class.' 

W1043 = 'Py++ created an ugly alias ("%s") for template instantiated class.'

W1044 = 'Py++ created an ugly alias ("%s") for function wrapper.'

warnings = globals()

for identifier, explanation in warnings.items():
    if len( identifier ) != 5:
        continue
    if identifier[0] != 'W':
        continue
    try:
        int( identifier[1:] )
    except:
        continue
    
    globals()[ identifier ] = 'warning %s: %s' % ( identifier, explanation )

del warnings
del identifier
del explanation




