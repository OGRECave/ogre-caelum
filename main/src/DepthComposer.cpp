// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include "CaelumPrecompiled.h"
#include "CaelumExceptions.h"
#include "DepthComposer.h"

using namespace Ogre;

namespace Caelum
{
	DepthComposer::DepthComposer
    (
        Ogre::SceneManager *sceneMgr
    ):
		mSceneMgr (sceneMgr),
        mCompositorChanged (false),
        mDebugDepthRender (false),
        mSkyDomeHazeEnabled (false),
        mGroundFogEnabled (false),
        mGroundFogDensity (0.1),
        mGroundFogBaseLevel (5),
        mGroundFogVerticalDecay (0.2),
        mGroundFogColour (ColourValue::Black)
    {
	}

	DepthComposer::~DepthComposer()
    {
        destroyAllViewportInstances();
	}

    void DepthComposer::setDebugDepthRender (bool value)
    {
        if (mDebugDepthRender == value) {
            return;
        }
        mDebugDepthRender = value;
        onCompositorMaterialChanged ();
    }

    void DepthComposer::setSkyDomeHazeEnabled (bool value)
    {
        if (mSkyDomeHazeEnabled == value) {
            return;
        }
        mSkyDomeHazeEnabled = value;
        onCompositorMaterialChanged ();
    }

    void DepthComposer::setGroundFogEnabled (bool value)
    {
        if (mGroundFogEnabled == value) {
            return;
        }
        mGroundFogEnabled = value;
        onCompositorMaterialChanged ();
    }

    const String& DepthComposer::getCompositorName ()
    {
        // Constant Ogre::Strings for names.
        static const Ogre::String CompositorName_DebugDepthRender =
                          "Caelum/DepthComposer_DebugDepthRender";
        static const Ogre::String CompositorName_Dummy =
                          "Caelum/DepthComposer_Dummy";
        static const Ogre::String CompositorName_ExpGroundFog =
                          "Caelum/DepthComposer_ExpGroundFog";
        static const Ogre::String CompositorName_SkyDomeHaze =
                          "Caelum/DepthComposer_SkyDomeHaze";
        static const Ogre::String CompositorName_SkyDomeHaze_ExpGroundFog =
                          "Caelum/DepthComposer_SkyDomeHaze_ExpGroundFog";

        // Should probably build materials and compositors by hand.
        if (mDebugDepthRender) {
            return CompositorName_DebugDepthRender;
        } else if (mSkyDomeHazeEnabled == false && mGroundFogEnabled == false) {
            return CompositorName_Dummy;
        } else if (mSkyDomeHazeEnabled == false && mGroundFogEnabled == true) {
            return CompositorName_ExpGroundFog;
        } else if (mSkyDomeHazeEnabled == true && mGroundFogEnabled == false) {
            return CompositorName_SkyDomeHaze;
        } else if (mSkyDomeHazeEnabled == true && mGroundFogEnabled == true) {
            return CompositorName_SkyDomeHaze_ExpGroundFog;
        } else {
            assert (0);
            return CompositorName_Dummy;
        }
    }

    void DepthComposer::onCompositorMaterialChanged ()
    {
        mCompositorChanged = true;
    }

    void DepthComposer::update ()
    {
        if (!mCompositorChanged) {
            return;
        }
        mCompositorChanged = false;

        for (auto& it : mViewportInstanceMap) {
            it.second->removeCompositor ();
            it.second->addCompositor ();
        }
    }

    DepthComposerInstance::DepthComposerInstance
    (
        DepthComposer* parent,
        Ogre::Viewport* viewport
    ):
        mParent(parent),
        mViewport(viewport),
        mCompInst(0)
    {
        LogManager::getSingleton().logMessage (
                "Caelum::DepthComposer: Attaching screen-space fog instance"
                " to viewport \'" + StringConverter::toString (reinterpret_cast<size_t>(getViewport ())) + "\'"
                " of render target \'" + getViewport()->getTarget ()->getName () + "\'");

        addCompositor ();
    }

    DepthComposerInstance::~DepthComposerInstance()
    {
        removeCompositor ();

        LogManager::getSingleton().logMessage (
                "Caelum::DepthComposer: Detached screen-space fog instance"
                " from viewport \'" + StringConverter::toString (reinterpret_cast<size_t>(getViewport ())) + "\'"
                " of render target \'" + getViewport()->getTarget ()->getName () + "\'");
    }

    void DepthComposerInstance::addCompositor ()
    {
        CompositorManager* compMgr = CompositorManager::getSingletonPtr();

        const String& compositorName = getParent ()->getCompositorName ();
        // First in the chain, as the depth of the original scene is needed
        mCompInst = compMgr->addCompositor(mViewport, compositorName, 0);
        if (!mCompInst) {
            CAELUM_THROW_UNSUPPORTED_EXCEPTION (
                    "Can't add \'" + compositorName + "\' compositor.",
                    "DepthComposer");
        }
        assert(mCompInst);
		mCompInst->setEnabled (true);
		mCompInst->addListener (this);
    }

    void DepthComposerInstance::removeCompositor ()
    {
        CompositorManager* compMgr = CompositorManager::getSingletonPtr();
        compMgr->removeCompositor (mViewport, mCompInst->getCompositor ()->getName ());
        mCompInst = 0;
    }

	void DepthComposerInstance::notifyMaterialSetup(uint pass_id, Ogre::MaterialPtr &mat)
	{
        //LogManager::getSingleton ().logMessage (
        //            "Caelum::DepthComposer: Material setup");

        Pass* pass = mat->getBestTechnique ()->getPass (0);
        mParams.setup(pass->getFragmentProgramParameters ());
	}

    void DepthComposerInstance::Params::setup(Ogre::GpuProgramParametersSharedPtr fpParams)
    {
        this->fpParams = fpParams;
        invViewProjMatrix.bind(fpParams, "invViewProjMatrix");
        worldCameraPos.bind(fpParams, "worldCameraPos");
        groundFogDensity.bind(fpParams, "groundFogDensity");
        groundFogVerticalDecay.bind(fpParams, "groundFogVerticalDecay");
        groundFogBaseLevel.bind(fpParams, "groundFogBaseLevel");
        groundFogColour.bind(fpParams, "groundFogColour");
        sunDirection.bind(fpParams, "sunDirection");
        hazeColour.bind(fpParams, "hazeColour");
    }

	void DepthComposerInstance::notifyMaterialRender(uint pass_id, Ogre::MaterialPtr &mat)
	{
        Camera* camera = getViewport ()->getCamera ();

        assert(mParams.fpParams == mat->getBestTechnique ()->getPass (0)->getFragmentProgramParameters ());

        // Auto param in a compositor does not use the external camera.
        // This means that sending matrices as auto_param will not work as expected.
        // Do it manually instead.
        Matrix4 projMatrix = camera->getProjectionMatrixWithRSDepth();
        Matrix4 viewMatrix = camera->getViewMatrix();

        mParams.invViewProjMatrix.set(mParams.fpParams, (projMatrix * viewMatrix).inverse());

        mParams.worldCameraPos.set(mParams.fpParams, camera->getDerivedPosition ());

        mParams.groundFogDensity.set(mParams.fpParams, getParent ()->getGroundFogDensity ());
        mParams.groundFogVerticalDecay.set(mParams.fpParams, getParent ()->getGroundFogVerticalDecay ());
        mParams.groundFogBaseLevel.set(mParams.fpParams, getParent ()->getGroundFogBaseLevel ());
        mParams.groundFogColour.set(mParams.fpParams, getParent ()->getGroundFogColour ());

        mParams.sunDirection.set(mParams.fpParams, getParent ()->getSunDirection ());
        mParams.hazeColour.set(mParams.fpParams, getParent ()->getHazeColour ());
	}

    DepthComposerInstance* DepthComposer::createViewportInstance(Ogre::Viewport* vp)
    {
        ViewportInstanceMap::const_iterator it = mViewportInstanceMap.find(vp);
        if (it == mViewportInstanceMap.end()) {
            std::unique_ptr<DepthComposerInstance> inst(new DepthComposerInstance(this, vp));
            mViewportInstanceMap.insert(std::make_pair(vp, inst.get()));
            // hold instance until successfully added to map.
            return inst.release();
        } else {
            return it->second;
        }
    }

    DepthComposerInstance* DepthComposer::getViewportInstance(Ogre::Viewport* vp) {
        ViewportInstanceMap::iterator it = mViewportInstanceMap.find(vp);
        if (it != mViewportInstanceMap.end()) {
            return it->second;
        } else {
            return 0;
        }
    }

    void DepthComposer::destroyViewportInstance(Viewport* vp)
    {
        ViewportInstanceMap::iterator it = mViewportInstanceMap.find(vp);
        if (it != mViewportInstanceMap.end()) {
            DepthComposerInstance* inst = it->second;
            delete inst;
            mViewportInstanceMap.erase(it);
        }
    }

    void DepthComposer::destroyAllViewportInstances() {
        ViewportInstanceMap::const_iterator it;
        ViewportInstanceMap::const_iterator begin = mViewportInstanceMap.begin();
        ViewportInstanceMap::const_iterator end = mViewportInstanceMap.end();
        for (it = begin; it != end; ++it) {
            assert(it->first == it->second->getViewport());
            delete it->second;
        }
        mViewportInstanceMap.clear();
    }
}
