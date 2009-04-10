/*
-----------------------------------------------------------------------------
This source file is part of wxOgreMVC

Copyright (c) 2007 Torus Knot Software Ltd, all rights reserved

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

-----------------------------------------------------------------------------
*/
#include "OgreMVCStableHeaders.h"
#include "OgreMVCProperty.h"

#include "OgreStringConverter.h"

#include <istream>

namespace OgreMVC
{
	//---------------------------------------------------------------------
	PropertySet::PropertySet()
	{

	}
	//---------------------------------------------------------------------
	PropertySet::~PropertySet()
	{
		for (PropertyMap::iterator i = mPropertyMap.begin(); i != mPropertyMap.end(); ++i)
		{
			delete i->second;
		}
		mPropertyMap.clear();
	}
	//---------------------------------------------------------------------
	void PropertySet::addProperty(PropertyBase* prop)
	{
		std::pair<PropertyMap::iterator, bool> retPair = 
			mPropertyMap.insert(PropertyMap::value_type(prop->getName(), prop));
		if (!retPair.second)
			throw std::exception("Duplicate property entry!");
	}
	//---------------------------------------------------------------------
	bool PropertySet::hasProperty(const std::string& name) const
	{
		PropertyMap::const_iterator i = mPropertyMap.find(name);
		return i != mPropertyMap.end();
	}
	//---------------------------------------------------------------------
	PropertyBase* PropertySet::getProperty(const std::string& name) const
	{
		PropertyMap::const_iterator i = mPropertyMap.find(name);
		if (i != mPropertyMap.end())
			return i->second;
		else
			throw std::exception("Property not found");

	}
	//---------------------------------------------------------------------
	PropertySet::PropertyIterator PropertySet::getPropertyIterator()
	{
		return PropertyIterator(mPropertyMap.begin(), mPropertyMap.end());
	}
	//---------------------------------------------------------------------
	PropertyValueMap PropertySet::getValueMap() const
	{
		PropertyValueMap ret;
		for (PropertyMap::const_iterator i = mPropertyMap.begin(); i != mPropertyMap.end(); ++i)
		{
			PropertyValue val;
			val.propType = i->second->getType();
			switch(val.propType)
			{
			case PT_INT:
				val.val = Ogre::Any(static_cast<Property<int>*>(i->second)->get());
				break;
			case PT_FLOAT:
				val.val = Ogre::Any(static_cast<Property<float>*>(i->second)->get());
				break;
			case PT_STRING:
				val.val = Ogre::Any(static_cast<Property<std::string>*>(i->second)->get());
				break;
			case PT_VECTOR2:
				val.val = Ogre::Any(static_cast<Property<Ogre::Vector2>*>(i->second)->get());
				break;
			case PT_VECTOR3:
				val.val = Ogre::Any(static_cast<Property<Ogre::Vector3>*>(i->second)->get());
				break;
			case PT_VECTOR4:
				val.val = Ogre::Any(static_cast<Property<Ogre::Vector4>*>(i->second)->get());
				break;
			case PT_COLOUR:
				val.val = Ogre::Any(static_cast<Property<Ogre::ColourValue>*>(i->second)->get());
				break;
			case PT_BOOL:
				val.val = Ogre::Any(static_cast<Property<bool>*>(i->second)->get());
				break;
			case PT_QUATERNION:
				val.val = Ogre::Any(static_cast<Property<Ogre::Quaternion>*>(i->second)->get());
				break;

			};
			ret[i->second->getName()] = val;
		}

		return ret;


	}
	//---------------------------------------------------------------------
	void PropertySet::setValueMap(const PropertyValueMap& values)
	{
		for (PropertyValueMap::const_iterator i = values.begin(); i != values.end(); ++i)
		{
			PropertyMap::iterator j = mPropertyMap.find(i->first);
			if (j != mPropertyMap.end())
			{
				// matching properties
				// check type
				if (j->second->getType() != i->second.propType)
				{
					throw std::exception("Non-matching type provided");
				}
				switch(i->second.propType)
				{
				case PT_INT:
					static_cast<Property<int>*>(j->second)->set(Ogre::any_cast<int>(i->second.val));
					break;
				case PT_FLOAT:
					static_cast<Property<float>*>(j->second)->set(Ogre::any_cast<float>(i->second.val));
					break;
				case PT_STRING:
					static_cast<Property<std::string>*>(j->second)->set(Ogre::any_cast<std::string>(i->second.val));
					break;
				case PT_VECTOR2:
					static_cast<Property<Ogre::Vector2>*>(j->second)->set(Ogre::any_cast<Ogre::Vector2>(i->second.val));
					break;
				case PT_VECTOR3:
					static_cast<Property<Ogre::Vector3>*>(j->second)->set(Ogre::any_cast<Ogre::Vector3>(i->second.val));
					break;
				case PT_VECTOR4:
					static_cast<Property<Ogre::Vector4>*>(j->second)->set(Ogre::any_cast<Ogre::Vector4>(i->second.val));
					break;
				case PT_COLOUR:
					static_cast<Property<Ogre::ColourValue>*>(j->second)->set(Ogre::any_cast<Ogre::ColourValue>(i->second.val));
					break;
				case PT_BOOL:
					static_cast<Property<bool>*>(j->second)->set(Ogre::any_cast<bool>(i->second.val));
					break;
				case PT_QUATERNION:
					static_cast<Property<Ogre::Quaternion>*>(j->second)->set(Ogre::any_cast<Ogre::Quaternion>(i->second.val));
					break;

				};


			}
		}


	}
	//---------------------------------------------------------------------
	void PropertySet::save(std::ostream& s)
	{
		s << "PROPBEGIN";

		for (PropertyMap::iterator i = mPropertyMap.begin(); i != mPropertyMap.end(); ++i)
		{
			s << i->second->getName();
			s << (int)i->second->getType();
			s << i->second->getValue();
		}

		s << "PROPEND";
	}
	//---------------------------------------------------------------------
	void PropertySet::load(std::istream& s)
	{
		std::istream::pos_type startpos = s.tellg();
		std::string tst;
		// check sentinel
		s >> tst;
		if (tst != "PROPBEGIN")
		{
			s.seekg(startpos);
			throw std::exception("Deserialise - not a property section");
		}

		while (tst != "PROPEND")
		{
			PropertyType ptype;
			int tmp;
			s >> tmp;
			ptype = (PropertyType)tmp;
			switch(ptype)
			{
			case PT_INT:
				{
					int i;
					s >> i;
					set(tst, i);
				}
				break;
			case PT_FLOAT:
				{
					float f;
					s >> f;
					set(tst, f);
				}
				break;
			case PT_STRING:
				{
					std::string v;
					s >> v;
					set(tst, v);
				}
				break;
			case PT_VECTOR2:
				{
					std::string v;
					s >> v;
					set(tst, Ogre::StringConverter::parseVector2(v));
				}
				break;
			case PT_VECTOR3:
				{
					std::string v;
					s >> v;
					set(tst, Ogre::StringConverter::parseVector3(v));
				}
				break;
			case PT_VECTOR4:
				{
					std::string v;
					s >> v;
					set(tst, Ogre::StringConverter::parseVector4(v));
				}
				break;
			case PT_COLOUR:
				{
					std::string v;
					s >> v;
					set(tst, Ogre::StringConverter::parseColourValue(v));
				}
				break;

			}


		}


	}

	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	// Can't really template these without getting potentially nasty casting errors
	// when assigning convertible types (e.g. int / float) because type is not
	// static
	//---------------------------------------------------------------------
	int PropertySet::getInt(const std::string& name) const
	{
		int ret;
		getPropertyImpl(name, ret, PT_INT);
		return ret;
	}
	//---------------------------------------------------------------------
	float PropertySet::getFloat(const std::string& name) const
	{
		float ret;
		getPropertyImpl(name, ret, PT_FLOAT);
		return ret;
	}
	//---------------------------------------------------------------------
	std::string PropertySet::getString(const std::string& name) const
	{
		std::string ret;
		getPropertyImpl(name, ret, PT_STRING);
		return ret;
	}
	//---------------------------------------------------------------------
	Ogre::Vector2 PropertySet::getVector2(const std::string& name) const
	{
		Ogre::Vector2 ret;
		getPropertyImpl(name, ret, PT_VECTOR2);
		return ret;

	}
	//---------------------------------------------------------------------
	Ogre::Vector3 PropertySet::getVector3(const std::string& name) const
	{
		Ogre::Vector3 ret;
		getPropertyImpl(name, ret, PT_VECTOR3);
		return ret;
	}
	//---------------------------------------------------------------------
	Ogre::Vector4 PropertySet::getVector4(const std::string& name) const
	{
		Ogre::Vector4 ret;
		getPropertyImpl(name, ret, PT_VECTOR4);
		return ret;
	}
	//---------------------------------------------------------------------
	Ogre::ColourValue PropertySet::getColour(const std::string& name) const
	{
		Ogre::ColourValue ret;
		getPropertyImpl(name, ret, PT_COLOUR);
		return ret;

	}
	//---------------------------------------------------------------------
	bool PropertySet::getBool(const std::string& name) const
	{
		bool ret;
		getPropertyImpl(name, ret, PT_BOOL);
		return ret;
	}
	//---------------------------------------------------------------------
	Ogre::Quaternion PropertySet::getQuat(const std::string& name) const
	{
		Ogre::Quaternion ret;
		getPropertyImpl(name, ret, PT_QUATERNION);
		return ret;

	}
	//---------------------------------------------------------------------
	void PropertySet::set(const std::string& name, int val)
	{
		setPropertyImpl(name, val, PT_INT);
	}
	//---------------------------------------------------------------------
	void PropertySet::set(const std::string& name, float val)
	{
		setPropertyImpl(name, val, PT_FLOAT);
	}
	//---------------------------------------------------------------------
	void PropertySet::set(const std::string& name, const std::string& val)
	{
		setPropertyImpl(name, val, PT_STRING);
	}
	//---------------------------------------------------------------------
	void PropertySet::set(const std::string& name, const char* val)
	{
		setPropertyImpl(name, std::string(val), PT_STRING);
	}
	//---------------------------------------------------------------------
	void PropertySet::set(const std::string& name, const Ogre::Vector2& val)
	{
		setPropertyImpl(name, val, PT_VECTOR2);
	}
	//---------------------------------------------------------------------
	void PropertySet::set(const std::string& name, const Ogre::Vector3& val)
	{
		setPropertyImpl(name, val, PT_VECTOR3);
	}
	//---------------------------------------------------------------------
	void PropertySet::set(const std::string& name, const Ogre::Vector4& val)
	{
		setPropertyImpl(name, val, PT_VECTOR4);
	}
	//---------------------------------------------------------------------
	void PropertySet::set(const std::string& name, const Ogre::ColourValue& val)
	{
		setPropertyImpl(name, val, PT_COLOUR);
	}
	//---------------------------------------------------------------------
	void PropertySet::set(const std::string& name, bool val)
	{
		setPropertyImpl(name, val, PT_BOOL);
	}
	//---------------------------------------------------------------------
	void PropertySet::set(const std::string& name, const Ogre::Quaternion& val)
	{
		setPropertyImpl(name, val, PT_QUATERNION);
	}
	//---------------------------------------------------------------------

}

