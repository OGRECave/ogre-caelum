// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include "CaelumPrecompiled.h"
#include "CloudSystem.h"
#include "FlatCloudLayer.h"

using namespace Ogre;

namespace Caelum
{
    CloudSystem::CloudSystem(
		    Ogre::SceneManager *sceneMgr,
		    Ogre::SceneNode *cloudRoot)
    {
        mSceneMgr = sceneMgr;
        mCloudRoot = cloudRoot;
    }

    FlatCloudLayer* CloudSystem::createLayerAtHeight(Ogre::Real height)
    {
        FlatCloudLayer* layer = this->createLayer ();
        layer->setHeight(height);
        return layer;
    }

    FlatCloudLayer* CloudSystem::createLayer()
    {
        std::auto_ptr<FlatCloudLayer> layer(new FlatCloudLayer(mSceneMgr, mCloudRoot));
        mLayers.push_back(layer.get());
        return layer.release();
    }

    void CloudSystem::addLayer(FlatCloudLayer* layer)
    {
        assert(layer != NULL);
        mLayers.push_back(layer);
    }

    void CloudSystem::clearLayers()
    {
	    for (unsigned i = 0; i < mLayers.size(); i++)
        {
		    delete mLayers[i];
		    mLayers[i] = 0;
	    }
    }

    CloudSystem::~CloudSystem()
    {
	    clearLayers ();
    }

    void CloudSystem::update(
		    Ogre::Real timePassed,
		    const Ogre::Vector3 &sunDirection,
		    const Ogre::ColourValue &sunLightColour,
		    const Ogre::ColourValue &fogColour,
		    const Ogre::ColourValue &sunSphereColour)
    {
	    for (uint i = 0; i < mLayers.size(); i++) {
            assert(mLayers[i] != NULL);
		    mLayers[i]->update(timePassed, sunDirection, sunLightColour, fogColour, sunSphereColour);
	    }
    }

    void CloudSystem::forceLayerQueryFlags (uint flags) {
	    for (uint i = 0; i < mLayers.size(); i++) {
		    mLayers[i]->setQueryFlags (flags);
	    }
    }

    void CloudSystem::forceLayerVisibilityFlags (uint flags) {
	    for (uint i = 0; i < mLayers.size(); i++) {
		    mLayers[i]->setVisibilityFlags (flags);
	    }
    }
}
