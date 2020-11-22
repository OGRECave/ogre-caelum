// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#ifndef CAELUM__CAELUM_PLUGIN_H
#define CAELUM__CAELUM_PLUGIN_H

#include "CaelumPrerequisites.h"
#include "CaelumScriptTranslator.h"
#include "TypeDescriptor.h"

namespace Caelum
{
    /** Implement an Ogre::Plugin for Caelum.
     *
     *  Ogre plugins are usually loaded from config files and they register
     *  various stuff in ogre managers. But you can also just link to the
     *  library normally and call install functions manually.
     */
    class CAELUM_EXPORT CaelumPlugin: public Ogre::Singleton<CaelumPlugin>
    {
    public:
        /// Get reference to singleton instance; or crash if N/A.
        static CaelumPlugin& getSingleton(void);
        /// Get pointer to singleton instance; or pointer if N/A.
        static CaelumPlugin* getSingletonPtr(void);

        CaelumPlugin();
        ~CaelumPlugin();

        void install () { initialise(); }
        void initialise ();
        void shutdown ();
        void uninstall() { shutdown(); }

        // Determine if the plugin was installed (if install was called).
        bool isInstalled () const { return mIsInstalled; }

   private:
        bool mIsInstalled = false;

#if CAELUM_TYPE_DESCRIPTORS
   public:
        /// Get default type descriptor data for caelum components.
        CaelumDefaultTypeDescriptorData* getTypeDescriptorData () { return &mTypeDescriptorData; }

   private:
        CaelumDefaultTypeDescriptorData mTypeDescriptorData;
#endif

#if CAELUM_SCRIPT_SUPPORT
   public:
        /** Load CaelumSystem and it's components from a script file.
         *  @param sys Target CaelumSystem.
         *      This is cleared using CaelumSystem::clear before loading.
         *      If scripting data is not found then this is not modified.
         *  @param objectName Name of caelum_sky_system from *.os file.
         *  @param scriptFileGroup The group to search in (unused in Ogre 1.6)
         */
        void loadCaelumSystemFromScript (
                CaelumSystem* sys,
                const Ogre::String& objectName,
                const Ogre::String& scriptFileGroup = Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME
                );

        /// @see PropScriptResourceManager
        PropScriptResourceManager* getPropScriptResourceManager () { return &mPropScriptResourceManager; }
        CaelumScriptTranslatorManager* getScriptTranslatorManager () { return &mScriptTranslatorManager; }

   private:
        PropScriptResourceManager mPropScriptResourceManager;
        CaelumScriptTranslatorManager mScriptTranslatorManager;
#endif
    };
}

#endif // CAELUM__CAELUM_PLUGIN_H
