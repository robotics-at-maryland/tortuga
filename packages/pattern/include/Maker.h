/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/pattern/include/Maker.h
 */

#ifndef RAM_PATTERN_MAKER_H_10_06_2007
#define RAM_PATTERN_MAKER_H_10_06_2007

// STD Includes
#include <map>
#include <utility>
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>

// Project Includes
#include "core/include/Platform.h"

/*
 This pattern is based on the  "Industrial Strength Pluggable Factories"
 article by Timothy R. Culp (with an added dose of policy based design)
*/

namespace ram {
namespace pattern {

/** Default MakerLookup policy.  Returns first maker which maps to the given key.
 */
struct DefaultMakerLookup
{
    template<class MapType>
    static typename MapType::mapped_type lookupMaker(MapType* registry,
        typename MapType::key_type key)
    {
        typename MapType::iterator iter = registry->find(key);
        assert(iter != registry->end() && "Could not find maker");
        return iter->second;
    }
};

/** Default ObjectMaker policy. Uses virtual makeObject method on maker objects.
 */
template<class Object, class Param>
struct DefaultObjectMaker
{
    virtual ~DefaultObjectMaker() {};

    /** Implemented by the subclasses of the Maker, creates the actual object */
    virtual Object makeObject(Param param) = 0;
    
    template<class Maker>
    static typename Maker::ObjectType createObject(Maker* maker,
        typename Maker::ParamType param)
    {
        return maker->makeObject(param);
    }
};


/** Policy Based Object Creation Template
 *
 *  This creates objects based on based in parameters use Maker objects
 *  registered to a specific key.  It is templated based on the type of the
 *  object, parameter, and key.  It also uses policies for extracting the key
 *  from the given parameters, finding the maker based from the multimap give
 *  the key, and finially creating the object using the found maker and the
 *  given parameter. @par
 *
 *  Makers are registered in there constructor by calling the super class
 *  constrcutor with the desired key. Example:
 *  @code

struct StreamKeyExtractor
{
    static std::string extractKey(std::iostream& param)
    {
        std::string key;
        param >> key;
        return key;
    }
};
 
class Number
{
public:
    virtual ~Number() {};
};
    
class Int : public Number
{
public:
    Int(int _val) : value(_val) {};
    
    int value;
};

typedef Maker<Number*,            // The type of object created by the maker
              std::iostream&,     // The parameter used to create the object
              std::string,        // The type of key used to register makers
              StreamKeyExtractor> // Gets the key from the paramters
NumberMaker;


class IntMaker : public NumberMaker
{
public:
    IntMaker() : NumberMaker("Int") {};
    virtual Number* makeObject(std::iostream& param)
    {
        int value;
        param >> value;
        return new Int(value);
    }
private:
    // Calls constructor to register the maker
    static IntMaker registerThis;
};

// Needs to be in a cpp file to actually invoke the constructor
IntMaker IntMaker::registerThis;

// This also needs to be in a cpp file, it makes sure there is only on instance
// of the static registry.  Having this in a header, will created multiple
// instances, and much hair pulling.
namespace ram {
namespace patter {

static NumberMaker::MakerMap* getRegistry();
{
    static NumberMaker::MakerMap* reg = new NumberMaker::MakerMap();
    return reg;
}

} // namespace pattern
} // namespace ram


};

 *  @endcode
 */

template<class Object, class Param, class Key,
         class KeyExtract,
         class MakerLookup = DefaultMakerLookup,
         class ObjectCreate = DefaultObjectMaker<Object, Param> >

class Maker : public KeyExtract,
              public MakerLookup,
              public ObjectCreate
{
public:    
    typedef std::multimap<Key, Maker*> MakerMap;
    typedef typename MakerMap::const_iterator MakerMapIter;
    
    typedef Object ObjectType;
    typedef Param ParamType;
    typedef Key MappedType;
    
private:
    Key m_key;

    /** Returns the global registry for this maker
     *
     *  You must implment this method once in a cpp file (not a header) for
     *  each version of this template you create.
     *
     *  @code
// Assumes MakerType is a typedef of this template
namespace ram {
namespace patter {

static MakerType::MakerMap* getRegistry();
{
    static MakerType::MakerMap* reg = new MakerType::MakerMap();
    return reg;
}

} // namespace pattern
} // namespace ram
     *  @endcode
     */
     static MakerMap* getRegistry();
   
public:

    /** This constructor which registers the maker class in the registry
     *
     *  @remarks
     *      When you subclass this class you should place a static instance
     *      of the class in its own definition. Then just make sure to call
     *      the super classes default constructor with the name of the class.
     *
     *  @param key
     *      The object that identifies the class in the registry.
     */
    Maker(Key key) :
        m_key(key)
    {
        // Grab the registry (first use creates it on the heap)
        MakerMap* registry = getRegistry();

        // Register Self
        registry->insert(make_pair(key, this));
    }

    /** Unregisters the Maker, and cleans up registry if necessary
     *
     *  @remarks
     *      If this is the last class in the registry, it will delete the
     *      global registry.
     */
    virtual ~Maker()
    {
        MakerMap* registry = getRegistry();

        // Remove self from registry
        registry->erase(m_key);

        // Free up registry if we are the last in the registry
        if (0 == registry->size())
            delete registry;
    }

    /** Returns the sorted set of Keys for all registered makers
     *
     *  This function is for debugging purposes.  It lets you see what is
     *  really regiserted inside the Maker.
     *
     *  @return The set of keys sorted in alphabetical order
     */
    static std::vector<Key> getRegisteredKeys()
    {
        MakerMap* registry = getRegistry();
        MakerMapIter iter = registry->begin();
        std::vector<Key> results;
        
        while(registry->end() != iter)
        {
            results.push_back(iter->first);
            iter++;
        }

        // Sort results
        std::sort(results.begin(), results.end());
        
        return results;
    }
    
    /** Creates a new object based on the given parameters
     *
     *  This uses the KeyExtract policy to pull the key from the parameters.
     *  Then it looks up the proper maker with the lookupMaker policy using that
     *  key.  The default one uses the first maker which maps to the key. Then
     *  it creates the object by using the ObjectCreat Policy.  The defualt
     *  policy just uses the virtual makeObject function.
     */
    static Object newObject(Param param)
    {
        Key key(KeyExtract::extractKey(param));
        Maker* maker = MakerLookup::lookupMaker(getRegistry(), key);

        // TODO: raise an exception here
        assert(maker && "Could not find object to match given key");
        return ObjectCreate::createObject(maker, param);
    }
};

} // namespace pattern
} // namespace ram

#endif // RAM_PATTERN_MAKER_H_10_06_2007
