/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  packages/core/include/ToPythonConverter.h
 */

#ifndef RAM_CORE_WRAP_PYTHONCONVERTER_H_01_23_2008
#define RAM_CORE_WRAP_PYTHONCONVERTER_H_01_23_2008

// STD Includes
#include <string>
#include <map>
//#include <iostream>

// Library Includes
#include <boost/python.hpp>

namespace ram {
namespace core {

template<class T>
class ToPythonConverter 
{
public:
    virtual ~ToPythonConverter() {};

    static boost::python::object convertObject(boost::shared_ptr<T> event);
    
    typedef std::map<std::string, ToPythonConverter*> ConverterRegistry;
    
protected:
    /** Does a manual conversion with boost::static_pointer_cast */
    virtual boost::python::object forceConvert(boost::shared_ptr<T> event) = 0;

    /** Try to do the conversion with boost::dynamic_pointer_cast */
    virtual boost::python::object tryConvert(boost::shared_ptr<T> event) = 0;
    
    /** Gets the global set of event converters */
    static ConverterRegistry* getConverterRegistry();
};

template<class Derived, class Base>
class SpecificToPythonConverter : public ToPythonConverter<Base>
{
public:
    /** Registers the Derived in the converter registry */
    SpecificToPythonConverter();

protected:    
    virtual boost::python::object forceConvert(boost::shared_ptr<Base> object);
    virtual boost::python::object tryConvert(boost::shared_ptr<Base> object);
};

// ------------------------------------------------------------------------- //
//             T E M P L A T E   I M P L E M E N T A T I O N                 //
// ------------------------------------------------------------------------- // 

template<class T>
void addConverter(std::string typeName, ToPythonConverter<T>* converter)
{

}

template<class T>
boost::python::object ToPythonConverter<T>::convertObject(
    boost::shared_ptr<T> object)
{
    // Return Nxone, if we get a null pointer
    if (!object)
        return boost::python::object();
    
//    std::cout << "Conveter of type: \"" << typeid(T).name() << "\" started:  ";
    // If we already have a python object, use default converter
    if (0 != boost::get_deleter<boost::python::converter::shared_ptr_deleter>(
            object))
    {
//        std::cout << "Object of: \"" << typeid(*(object.get())).name()
//                  << "\" already from python"  <<  std::endl;
        return boost::python::object(object);
    }


    // Did not find the converter so look it up by type in our registry
    ConverterRegistry* registry = getConverterRegistry();
//    std::cout << " Looking at " << registry->size() << " converters: ";
    
    std::string typeName(typeid(*(object.get())).name());
    typename ConverterRegistry::iterator registryIter =
        registry->find(typeName);

    if (registry->end() != registryIter)
    {
        // Exact Converter Found
        return registryIter->second->forceConvert(object);
    }
    else
    {
        // Search through for a converter of a super class
        registryIter = registry->begin();
        while (registry->end() != registryIter)
        {
            boost::python::object convertedObject(
                registryIter->second->tryConvert(object));
                
            if(Py_None != convertedObject.ptr())
            {
                return convertedObject;
            }

            registryIter++;
        }
    }

//    std::cout << "Default Conversion of type: \""
//              << typeid(*(object.get())).name() << "\"" << std::endl;
    // Keeps the compiler happy
    return boost::python::object(object);
}

template<class Derived, class Base>
SpecificToPythonConverter<Derived, Base>::SpecificToPythonConverter()
{
//    std::cout << "Converter: \"" << typeid(Base).name()
//              << "\" registered for type: \"" << typeid(Derived).name()
//              << "\"" <<  std::endl;
    
    ToPythonConverter<Base>::getConverterRegistry()->insert(
        std::make_pair(std::string(typeid(Derived).name()), this));
    
    // Register these types here. This is a slight hack, but its really
    // the simplest place to put these
    boost::python::register_ptr_to_python<boost::shared_ptr<Derived> >();
    boost::python::implicitly_convertible<boost::shared_ptr<Derived>,
        boost::shared_ptr<Derived> >();
}
    
template<class Derived, class Base>
boost::python::object SpecificToPythonConverter<Derived, Base>::forceConvert(
    boost::shared_ptr<Base> object)
{
//    std::cout << "Force converted: \"" << typeid(*(object.get())).name()
//              << "\" to: \"" <<  typeid(Derived).name() << "\"" << std::endl;
    
    // We preform a static cast here, because this is only called when
    // have the proper type
    boost::shared_ptr<Derived> result =
        boost::static_pointer_cast<Derived>(object);
    
    return boost::python::object(result);
}

template<class Derived, class Base>
boost::python::object SpecificToPythonConverter<Derived, Base>::tryConvert(
    boost::shared_ptr<Base> object)
{
//    std::cout << "Try to convert: \"" << typeid(*(object.get())).name()
//              << "\" to: \"" <<  typeid(Derived).name() << "\"" << std::endl;
        
    // We preform a static cast here, because this is only called when
    // have the proper type
    boost::shared_ptr<Derived> result =
        boost::dynamic_pointer_cast<Derived>(object);
    
    return boost::python::object(result);
}

} // namespace core
} // namespace ram

#endif // RAM_CORE_WRAP_PYTHONCONVERTER_H_01_23_2008
