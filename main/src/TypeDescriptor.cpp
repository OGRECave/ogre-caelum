// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include "CaelumPrecompiled.h"
#include "TypeDescriptor.h"

#if CAELUM_TYPE_DESCRIPTORS

using namespace Ogre;

namespace Caelum
{
    DefaultTypeDescriptor::DefaultTypeDescriptor ()
    {
    }
    
    DefaultTypeDescriptor::~DefaultTypeDescriptor () {
        for (TypeDescriptor::PropertyMap::const_iterator it = mPropertyMap.begin(), end = mPropertyMap.end(); it != end; ++it) {
            delete it->second;
        }
    }

    const ValuePropertyDescriptor* DefaultTypeDescriptor::getPropertyDescriptor (const Ogre::String& name) const
    {
        PropertyMap::const_iterator it = mPropertyMap.find(name);
        if (it != mPropertyMap.end()) {
            return it->second;
        } else {
            return 0;
        }
    }

    const std::vector<String> DefaultTypeDescriptor::getPropertyNames () const
    {
        std::vector<String> result;
        for (TypeDescriptor::PropertyMap::const_iterator it = mPropertyMap.begin(), end = mPropertyMap.end(); it != end; ++it) {
            result.push_back(it->first);
        }
        return result;
    }

    const TypeDescriptor::PropertyMap DefaultTypeDescriptor::getFullPropertyMap () const {
        return mPropertyMap;
    }

    void DefaultTypeDescriptor::add (const Ogre::String& name, const ValuePropertyDescriptor* descriptor)
    {
        mPropertyMap.insert(make_pair(name, descriptor));
    }
}

#endif
