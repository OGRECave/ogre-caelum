// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include "CaelumPrecompiled.h"
#include "CaelumPlugin.h"

namespace Ogre
{
    template<> Caelum::CaelumPlugin* Ogre::Singleton<Caelum::CaelumPlugin>::msSingleton = 0;
}

namespace Caelum
{
	CaelumPlugin* CaelumPlugin::getSingletonPtr () {
        return msSingleton;
    }

    CaelumPlugin& CaelumPlugin::getSingleton () {  
        assert (msSingleton);
        return *msSingleton;
    }

#if CAELUM_SCRIPT_SUPPORT
    CaelumPlugin::CaelumPlugin(): mScriptTranslatorManager(&mTypeDescriptorData)
#else
    CaelumPlugin::CaelumPlugin()
#endif
    {
    }

    CaelumPlugin::~CaelumPlugin() {
    }

    void CaelumPlugin::initialise ()
    {
        assert(!mIsInstalled && "Already installed");

        Ogre::LogManager::getSingleton ().logMessage("Caelum plugin version " + 
                Ogre::StringConverter::toString (CAELUM_VERSION_MAIN) + "." +
                Ogre::StringConverter::toString (CAELUM_VERSION_SEC) + "." +
                Ogre::StringConverter::toString (CAELUM_VERSION_TER) + " "
                "installed");

#if CAELUM_SCRIPT_SUPPORT
        Ogre::ScriptCompilerManager::getSingleton ().addTranslatorManager (
                getScriptTranslatorManager ());
        Ogre::ResourceGroupManager::getSingleton()._registerResourceManager (
                getPropScriptResourceManager ()->getResourceType (),
                getPropScriptResourceManager ());

        getScriptTranslatorManager()->_setPropScriptResourceManager (
                &mPropScriptResourceManager);
#endif // CAELUM_SCRIPT_SUPPORT
	    
	 mIsInstalled = true;   
    }

    void CaelumPlugin::shutdown ()
    {
        assert(mIsInstalled && "Not installed");

#if CAELUM_SCRIPT_SUPPORT
        getScriptTranslatorManager()->_setPropScriptResourceManager (0);

        Ogre::ResourceGroupManager::getSingleton ()._unregisterResourceManager (
                getPropScriptResourceManager ()->getResourceType ());
        Ogre::ScriptCompilerManager::getSingleton ().removeTranslatorManager (
                getScriptTranslatorManager ());
#endif // CAELUM_SCRIPT_SUPPORT

        Ogre::LogManager::getSingleton ().logMessage("Caelum plugin uninstalled");

        mIsInstalled = false;
    }

#if CAELUM_SCRIPT_SUPPORT
    void CaelumPlugin::loadCaelumSystemFromScript (
            CaelumSystem* sys,
            const Ogre::String& objectName,
            const Ogre::String& groupName)
    {
        assert (sys);
        assert (this->isInstalled () && "Must install CaelumPlugin before loading scripts");

        // Fetch raw resource ptr.
        Ogre::ResourcePtr res = getPropScriptResourceManager ()->createOrRetrieve (objectName, groupName).first;


        // Check a PropScriptResource was found.
        PropScriptResource* propRes = static_cast<PropScriptResource*> (res.get ());
        if (!propRes) {
            OGRE_EXCEPT (Ogre::Exception::ERR_ITEM_NOT_FOUND,
                    "Could not find caelum_sky_system " + objectName,
                    "CaelumPlugin::loadCaelumSystemFromScript");
        }

        // Fetch the resource stream. Look in the actual group of the resource!
        const Ogre::String& scriptFileName = propRes->getOrigin();
        const Ogre::String& scriptFileGroup = propRes->getGroup();
        Ogre::DataStreamPtr streamPtr = Ogre::ResourceGroupManager::getSingleton ().openResource (
                scriptFileName, scriptFileGroup);

        // Feed it into the compiler.
        this->getScriptTranslatorManager()->getCaelumSystemTranslator()->setTranslationTarget (sys, objectName);
        Ogre::ScriptCompilerManager::getSingleton ().parseScript (streamPtr, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        bool found = this->getScriptTranslatorManager()->getCaelumSystemTranslator()->foundTranslationTarget ();

        // This shouldn't normally happen.
        if (!found) {
            OGRE_EXCEPT (Ogre::Exception::ERR_ITEM_NOT_FOUND,
                    "Could not find caelum_sky_system " + objectName + " in file " + scriptFileName + " on reparsing. "
                    "Perhaps information in PropScriptResourceManager is out of date?",
                    "CaelumPlugin::loadCaelumSystemFromScript");
        }
        this->getScriptTranslatorManager()->getCaelumSystemTranslator()->clearTranslationTarget ();
    }
#endif // CAELUM_SCRIPT_SUPPORT
}
