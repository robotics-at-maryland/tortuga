/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vision/include/TableColorFilter.h
 */

#ifndef RAM_VISION_TABLECOLORFILTER_H
#define RAM_VISION_TABLECOLORFILTER_H

// STD Includes
#include <string>

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/ImageFilter.h"
#include "core/include/Forward.h"
#include "core/include/ConfigNode.h"
#include "core/include/PropertySet.h"
#include "core/include/BitField3D.h"
#include "math/include/ImplicitSurface.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT TableColorFilter : public ImageFilter
{
public:
    TableColorFilter(std::string filepath);
    virtual ~TableColorFilter() {}

    /** Run the Filter on the input image, debug results to output Image
     *
     *  @param input   The image to run the detector on
     *  @param output  Place results, (its input if NULL)
     */
    virtual void filterImage(Image* input, Image* output = 0);
    virtual void inverseFilterImage(Image* input, Image* output = 0);
    
    static void saveLookupTable(std::string filepath, 
                                core::BitField3D &filterTable);
    static void createLookupTable(std::string filepath, 
                                  math::ImplicitSurface &iSurface);

private:
    bool loadLookupTable();

    // property set and properties
    core::BitField3D m_filterTable;
    core::PropertySetPtr m_propertySet;
    std::string m_filepath;
};
    
} // namespace vision
} // namespace ram

#endif // RAM_VISION_TABLECOLORFILTER_H
