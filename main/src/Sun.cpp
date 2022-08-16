// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include "CaelumPrecompiled.h"
#include "Sun.h"
#include "InternalUtilities.h"

namespace Caelum
{
    const Ogre::String SphereSun::SUN_MATERIAL_NAME = "CaelumSphereSun";

    SphereSun::SphereSun
    (
        Ogre::SceneManager *sceneMgr,
        Ogre::SceneNode *caelumRootNode,
        const Ogre::String &meshName
    ):
        BaseSkyLight(sceneMgr, caelumRootNode)
    {
        Ogre::String uniqueSuffix = "/" + InternalUtilities::pointerToString (this);
        mSunMaterial.reset (InternalUtilities::checkLoadMaterialClone (SUN_MATERIAL_NAME, SUN_MATERIAL_NAME + uniqueSuffix));

        mSunEntity.reset (sceneMgr->createEntity ("Caelum/SphereSun" + uniqueSuffix, meshName));
        mSunEntity->setMaterialName (mSunMaterial->getName ());
        mSunEntity->setCastShadows (false);
        mSunEntity->setRenderQueueGroup (CAELUM_RENDER_QUEUE_SUN);

        mNode->attachObject (mSunEntity.get ());
    }

    SphereSun::~SphereSun () { }

    void SphereSun::setBodyColour (const Ogre::ColourValue &colour) {
        BaseSkyLight::setBodyColour(colour);

        // Set sun material colour.
        mSunMaterial->setSelfIllumination (colour);
    }

    void SphereSun::notifyCameraChanged (Ogre::Camera *cam) {
        // This calls setFarRadius
        CameraBoundElement::notifyCameraChanged(cam);

        // Set sun position.
        Ogre::Real sunDistance = mRadius - mRadius * Ogre::Math::Tan (Ogre::Degree (0.01));
        mNode->setPosition(-mDirection * sunDistance);

        // Set sun scaling  in [1.6(6) ~ 2.0] range.
        float factor = 2 - mBodyColour.b / 3;
        float scale = factor * sunDistance * Ogre::Math::Tan (Ogre::Degree (0.01));
        mNode->setScale (Ogre::Vector3::UNIT_SCALE * scale);
    }

    const Ogre::String SpriteSun::SUN_MATERIAL_NAME = "CaelumSpriteSun";

    SpriteSun::SpriteSun (
            Ogre::SceneManager *sceneMgr,
            Ogre::SceneNode *caelumRootNode, 
            const Ogre::String &sunTextureName,
            const Ogre::Degree& sunTextureAngularSize
    ):
        BaseSkyLight(sceneMgr, caelumRootNode),
        mSunTextureAngularSize(sunTextureAngularSize)
    {
        Ogre::String uniqueSuffix = "/" + InternalUtilities::pointerToString (this);

        mSunMaterial.reset (InternalUtilities::checkLoadMaterialClone (SUN_MATERIAL_NAME, SUN_MATERIAL_NAME + uniqueSuffix));
        setSunTexture (sunTextureName);

        mSunBillboardSet.reset (sceneMgr->createBillboardSet ("Caelum/SpriteSun" + uniqueSuffix, 2));
        mSunBillboardSet->setMaterialName (mSunMaterial->getName());
        mSunBillboardSet->setCastShadows (false);
        mSunBillboardSet->setRenderQueueGroup (CAELUM_RENDER_QUEUE_SUN);
        mSunBillboardSet->setDefaultDimensions (1.0f, 1.0f);
        mSunBillboardSet->createBillboard (Ogre::Vector3::ZERO);

        mNode->attachObject (mSunBillboardSet.get ());
    }

    SpriteSun::~SpriteSun () { }

    void SpriteSun::setBodyColour (const Ogre::ColourValue &colour) {
        BaseSkyLight::setBodyColour (colour);

        // Set sun material colour.
        mSunBillboardSet->getBillboard (0)->setColour (colour);
    }

    void SpriteSun::setSunTexture (const Ogre::String &textureName) {
        // Update the sun material
        assert(mSunMaterial->getBestTechnique ());
        assert(mSunMaterial->getBestTechnique ()->getPass (0));
        assert(mSunMaterial->getBestTechnique ()->getPass (0)->getTextureUnitState (0));
        mSunMaterial->getBestTechnique ()->getPass (0)->getTextureUnitState (0)->setTextureName (textureName);
    }

    void SpriteSun::setSunTextureAngularSize(const Ogre::Degree& sunTextureAngularSize){
        mSunTextureAngularSize = sunTextureAngularSize;
    }

    void SpriteSun::notifyCameraChanged (Ogre::Camera *cam) {
        // This calls setFarRadius
        BaseSkyLight::notifyCameraChanged(cam);

        // Set sun position.
        Ogre::Real sunDistance = mRadius - mRadius * Ogre::Math::Tan(mSunTextureAngularSize);
        mNode->setPosition (-mDirection * sunDistance);

        // Set sun scaling in [1.0 ~ 1.2] range
        float factor = 1.2f - mBodyColour.b * 0.2f;
        float scale = factor * sunDistance * Ogre::Math::Tan(mSunTextureAngularSize);
        mNode->setScale (Ogre::Vector3::UNIT_SCALE * scale);
    }
}
