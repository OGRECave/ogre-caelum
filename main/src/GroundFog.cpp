// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include "CaelumPrecompiled.h"
#include "GroundFog.h"
#include "CaelumExceptions.h"
#include "InternalUtilities.h"

namespace Caelum
{
	const Ogre::String GroundFog::DEFAULT_PASS_NAME = "CaelumGroundFog";

	GroundFog::GroundFog(
			Ogre::SceneManager *sceneMgr,
			Ogre::SceneNode *caelumRootNode,
			const Ogre::String &domeMaterialName,
			const Ogre::String &domeEntityName):
			mScene(sceneMgr)
	{
        Ogre::String uniqueSuffix = InternalUtilities::pointerToString (this);

		mDomeMaterial.reset(InternalUtilities::checkLoadMaterialClone (domeMaterialName, domeMaterialName + uniqueSuffix));
        mDomeParams.setup(mDomeMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters());

		// Create dome entity, using a prefab sphere.
        // The prefab sphere has a radius of 50 units.
        // If this changes in future version of ogre this might break.
        mDomeEntity.reset (mScene->createEntity (domeEntityName, Ogre::SceneManager::PT_SPHERE));
        mDomeEntity->setMaterialName (mDomeMaterial->getName ());
        mDomeEntity->setCastShadows (false);
        mDomeEntity->setRenderQueueGroup (CAELUM_RENDER_QUEUE_GROUND_FOG);
		sceneMgr->getRenderQueue()->getQueueGroup(CAELUM_RENDER_QUEUE_GROUND_FOG)->setShadowsEnabled(false);
		
        // Create dome node
		mDomeNode.reset (caelumRootNode->createChildSceneNode ());
		mDomeNode->attachObject (mDomeEntity.get());
		
		// Initialize default fog parameters
		mDensity = 0.1;
		mVerticalDecay = 0.2;
		mGroundLevel = 5;
		mFogColour = Ogre::ColourValue::Black;

        forceUpdate();
	}

	GroundFog::~GroundFog() {
        // Disable passes.
        setDensity(0);
	}

	GroundFog::PassSet& GroundFog::getPasses() {
		return mPasses;
	}

	const GroundFog::PassSet& GroundFog::getPasses() const {
		return mPasses;
	}

	void GroundFog::findFogPassesByName (const Ogre::String& passName) {
		Ogre::MaterialManager *matManager = Ogre::MaterialManager::getSingletonPtr();
		Ogre::MaterialManager::ResourceMapIterator matIt = matManager->getResourceIterator();
		while (matIt.hasMoreElements()) {
			Ogre::MaterialPtr mat = matIt.getNext().staticCast<Ogre::Material>();
			Ogre::Material::TechniqueIterator techIt = mat->getTechniqueIterator();
			while (techIt.hasMoreElements()) {
				Ogre::Technique *tech = techIt.getNext();
				Ogre::Technique::PassIterator passIt = tech->getPassIterator();
				while (passIt.hasMoreElements()) {
					Ogre::Pass *pass = passIt.getNext();
					if (pass->getName() == passName) {
						mPasses.insert(pass);
					}
				}
			}
		}
		forceUpdate();
	}

	void GroundFog::setDensity (Ogre::Real density) {
		if (Ogre::Math::Abs(mDensity - density) > 0.000001) {
            for (PassFogParamsVector::const_iterator it = mPassFogParams.begin(), end = mPassFogParams.end(); it != end; ++it) {
			    it->fogDensity.set(it->fpParams, density);
			}
			mDensity = density;
		}
		updateSkyFogging();
	}

	Ogre::Real GroundFog::getDensity () const {
		return mDensity;
	}

	void GroundFog::setColour (const Ogre::ColourValue &colour) {
        bool different = 
                Ogre::Math::Abs(mFogColour.r - colour.r) > 0.001 ||
                Ogre::Math::Abs(mFogColour.g - colour.g) > 0.001 ||
                Ogre::Math::Abs(mFogColour.b - colour.b) > 0.001 ||
                Ogre::Math::Abs(mFogColour.a - colour.a) > 0.001;
        if (different) {
            for (PassFogParamsVector::const_iterator it = mPassFogParams.begin(), end = mPassFogParams.end(); it != end; ++it) {
			    it->fogColour.set(it->fpParams, colour);
		    }
		    mFogColour = colour;
        }
		updateSkyFogging();
	}

	const Ogre::ColourValue GroundFog::getColour () const {
        return mFogColour;
	}

	void GroundFog::setVerticalDecay (Ogre::Real verticalDecay) {
		if (Ogre::Math::Abs(mVerticalDecay - verticalDecay) > 0.000001) {
            for (PassFogParamsVector::const_iterator it = mPassFogParams.begin(), end = mPassFogParams.end(); it != end; ++it) {
			    it->fogVerticalDecay.set(it->fpParams, verticalDecay);
			}
			mVerticalDecay = verticalDecay;
		}
		updateSkyFogging();
	}

	Ogre::Real GroundFog::getVerticalDecay () const {
		return mVerticalDecay;
	}

	void GroundFog::setGroundLevel (Ogre::Real groundLevel) {
		if (Ogre::Math::Abs(mGroundLevel - groundLevel) > 0.000001) {
            for (PassFogParamsVector::const_iterator it = mPassFogParams.begin(), end = mPassFogParams.end(); it != end; ++it) {
			    it->fogGroundLevel.set(it->fpParams, groundLevel);
			}
			mGroundLevel = groundLevel;
		}
		updateSkyFogging();
	}

	Ogre::Real GroundFog::getGroundLevel () const {
		return mGroundLevel;
	}

	void GroundFog::updateSkyFogging() {
        mDomeParams.fogDensity.set(mDomeParams.fpParams, mDensity);
		mDomeParams.fogVerticalDecay.set(mDomeParams.fpParams, mVerticalDecay);
		mDomeParams.fogGroundLevel.set(mDomeParams.fpParams, mGroundLevel);
		mDomeParams.fogColour.set(mDomeParams.fpParams, mFogColour);
	}

	void GroundFog::forceUpdate ()
    {
        updatePassFogParams();
        for (PassFogParamsVector::const_iterator it = mPassFogParams.begin(), end = mPassFogParams.end(); it != end; ++it) {
            const PassFogParams& params = *it;
            params.fogDensity.set(params.fpParams, mDensity);
			params.fogVerticalDecay.set(params.fpParams, mVerticalDecay);
			params.fogGroundLevel.set(params.fpParams, mGroundLevel);
			params.fogColour.set(params.fpParams, mFogColour);
		}
		updateSkyFogging();
	}

	void GroundFog::updatePassFogParams ()
    {
        mPassFogParams.clear();
        for (PassSet::const_iterator it = mPasses.begin(), end = mPasses.end(); it != end; ++it) {
            mPassFogParams.push_back(PassFogParams((*it)->getFragmentProgramParameters()));
        }
        std::sort(mPassFogParams.begin(), mPassFogParams.end(), PassFogParams::lessThanByParams);
        mPassFogParams.erase(std::unique(mPassFogParams.begin(), mPassFogParams.end(), PassFogParams::equalByParams), mPassFogParams.end());
    }

    void GroundFog::FogParamsBase::setup(Ogre::GpuProgramParametersSharedPtr fpParams) {
        this->fpParams = fpParams;
        fogDensity.bind(fpParams, "fogDensity");
		fogVerticalDecay.bind(fpParams, "fogVerticalDecay");
		fogGroundLevel.bind(fpParams, "fogGroundLevel");
		fogColour.bind(fpParams, "fogColour");
    }

    void GroundFog::DomeFogParams::setup(Ogre::GpuProgramParametersSharedPtr fpParams) {
        FogParamsBase::setup(fpParams);
		cameraHeight.bind(fpParams, "cameraHeight");
    }

	void GroundFog::notifyCameraChanged (Ogre::Camera *cam)
	{
        CameraBoundElement::notifyCameraChanged (cam);

		// Send camera height to shader.
        float cameraHeight = cam->getDerivedPosition().dotProduct(mDomeNode->_getDerivedOrientation() * Ogre::Vector3::UNIT_Y);
        mDomeParams.cameraHeight.set(mDomeParams.fpParams, cameraHeight);
    }

    void GroundFog::setFarRadius (Ogre::Real radius)
    {
        CameraBoundElement::setFarRadius(radius);
        // Adjust for radius 50.
        mDomeNode->setScale(Ogre::Vector3::UNIT_SCALE * radius / 50.0);
	}
}
