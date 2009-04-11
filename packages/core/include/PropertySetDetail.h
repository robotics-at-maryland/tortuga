/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/PropertySetDetail.h
 */

#ifndef RAM_CORE_PROPERTYSETDETAIL_H_04_11_2009
#define RAM_CORE_PROPERTYSETDETAIL_H_04_11_2009

namespace ram {
namespace core {
    
template <typename T>
void PropertySet::addProperty(const std::string& name, const std::string& desc, 
                              T defaultValue, T* valuePtr)
{
    addProperty(core::PropertyPtr(
        new core::VariableProperty<T>(name, desc, defaultValue, valuePtr)));
}
    
template <typename T>
void PropertySet::addProperty(const std::string& name, const std::string& desc, 
                              T defaultValue,
                              typename FunctionProperty<T>::GetterFunc getter,
                              typename FunctionProperty<T>::SetterFunc setter)
{
    addProperty(core::PropertyPtr(
        new core::FunctionProperty<T>(name, desc, defaultValue, getter,
                                      setter)));

}

template <typename T>
void PropertySet::addProperty(core::ConfigNode config, bool requireInConfig,
                              const std::string& name, const std::string& desc, 
                              T defaultValue, T* valuePtr)
{
    core::PropertyPtr prop(
        new core::VariableProperty<T>(name, desc, defaultValue, valuePtr));
    addProperty(prop);
    loadValueFromConfig<T>(config, prop, requireInConfig);
}
    
template <typename T>
void PropertySet::addProperty(core::ConfigNode config, bool requireInConfig,
                              const std::string& name, const std::string& desc,
                              T defaultValue,
                              typename FunctionProperty<T>::GetterFunc getter,
                              typename FunctionProperty<T>::SetterFunc setter)
{
    core::PropertyPtr prop(
        new core::FunctionProperty<T>(name, desc, defaultValue, getter,
                                      setter));
    addProperty(prop);
    loadValueFromConfig<T>(config, prop, requireInConfig);
}

template <typename T>
void PropertySet::loadValueFromConfig(core::ConfigNode config,
                                      core::PropertyPtr prop,
                                      bool requireInConfig)
{
    std::string name(prop->getName());
    if (requireInConfig)
        assert(config.exists(name) && "Value does not exist in config file");
    core::ConfigNode valueNode(config[name]);

    loadValueFromNode<T>(config, valueNode, prop);
}

template <typename T>
void PropertySet::loadValueFromNode(core::ConfigNode config,
                                    core::ConfigNode valueNode,
                                    core::PropertyPtr prop)
{
    // You need a specific specialization for this type
    BOOST_STATIC_ASSERT(sizeof(T) == 0);
}

// See PropertySet.cpp for the specialization implementations
template <>
void PropertySet::loadValueFromNode<int>(core::ConfigNode config,
                                         core::ConfigNode valueNode,
                                         core::PropertyPtr prop);

template <>
void PropertySet::loadValueFromNode<double>(core::ConfigNode config,
                                            core::ConfigNode valueNode,
                                            core::PropertyPtr prop);

template <>
void PropertySet::loadValueFromNode<bool>(core::ConfigNode config,
                                          core::ConfigNode valueNode,
                                          core::PropertyPtr prop);

#ifdef RAM_WITH_MATH
    
template <>
void PropertySet::loadValueFromNode<math::Vector2>(core::ConfigNode config,
                                                   core::ConfigNode valueNode,
                                                   core::PropertyPtr prop);

template <>
void PropertySet::loadValueFromNode<math::Vector3>(core::ConfigNode config,
                                                   core::ConfigNode valueNode,
                                                   core::PropertyPtr prop);

template <>
void PropertySet::loadValueFromNode<math::Quaternion>(
    core::ConfigNode config,
    core::ConfigNode valueNode,
    core::PropertyPtr prop);

#endif // RAM_WITH_MATH

} // namespace core
} // namespace ram

#endif // RAM_CORE_PROPERTYSETDETAIL_H_04_11_2009
