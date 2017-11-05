// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include "CaelumPrecompiled.h"
#include "CaelumPlugin.h"

// ms_Singleton was renamed to msSingleton in ogre 1.8
#if OGRE_VERSION_MINOR < 8
	#define SINGLETON_MEMBER ms_Singleton
#else
	#define SINGLETON_MEMBER msSingleton
#endif

#if OGRE_VERSION_MINOR >= 9
    namespace Ogre
    {
        template<> Caelum::CaelumPlugin* Ogre::Singleton<Caelum::CaelumPlugin>::SINGLETON_MEMBER = 0;
    }
#else
    template<> Caelum::CaelumPlugin* Ogre::Singleton<Caelum::CaelumPlugin>::SINGLETON_MEMBER = 0;
#endif

namespace Caelum
{
	CaelumPlugin* CaelumPlugin::getSingletonPtr () {
        return SINGLETON_MEMBER;
    }

    CaelumPlugin& CaelumPlugin::getSingleton () {  
        assert (SINGLETON_MEMBER);
        return *SINGLETON_MEMBER;  
    }

    extern "C" void CAELUM_EXPORT dllStartPlugin () {
        assert (CaelumPlugin::getSingletonPtr () == 0);
        CaelumPlugin* plugin = new CaelumPlugin();
        assert (CaelumPlugin::getSingletonPtr () == plugin);
        Ogre::Root::getSingleton ().installPlugin (CaelumPlugin::getSingletonPtr ());
    }

    extern "C" void CAELUM_EXPORT dllStopPlugin () {
        assert (CaelumPlugin::getSingletonPtr () != 0);
        Ogre::Root::getSingleton ().uninstallPlugin (CaelumPlugin::getSingletonPtr ());
        delete CaelumPlugin::getSingletonPtr ();
        assert (CaelumPlugin::getSingletonPtr () == 0);
    }

#if CAELUM_SCRIPT_SUPPORT
    CaelumPlugin::CaelumPlugin(): mScriptTranslatorManager(&mTypeDescriptorData)
#else
    CaelumPlugin::CaelumPlugin()
#endif
    {
        mIsInstalled = false;
    }

    CaelumPlugin::~CaelumPlugin() {
    }

    const Ogre::String CaelumPlugin::CAELUM_PLUGIN_NAME = "Caelum";

    const Ogre::String& CaelumPlugin::getName () const {
        return CAELUM_PLUGIN_NAME;
    }

    void CaelumPlugin::install ()
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

    void CaelumPlugin::initialise () {
    }

    void CaelumPlugin::shutdown () {
    }

    void CaelumPlugin::uninstall ()
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

        // Fetch raw resource ptr. Attempt to support explicit resource groups currently in Ogre trunk.
#if OGRE_VERSION >= 0x00010900
	Ogre::ResourcePtr res = getPropScriptResourceManager ()->createOrRetrieve (objectName, groupName).first;
#elif OGRE_VERSION >= 0x00010700
        Ogre::ResourcePtr res = getPropScriptResourceManager ()->getByName (objectName, groupName);
#else
        Ogre::ResourcePtr res = getPropScriptResourceManager ()->getByName (objectName);
#endif

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
                scriptFileName, scriptFileGroup, false);

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
