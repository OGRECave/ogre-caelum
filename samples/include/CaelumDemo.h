// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include "CaelumDemoCommon.h"
#include "ExampleApplication.h"

class CaelumSampleFrameListener : public OgreBites::InputListener
{
protected:
    Caelum::CaelumSystem *mCaelumSystem;
    Ogre::SceneManager *mScene;
    float mSpeedFactor;
    bool mPaused;
    float mTimeTillNextUpdate;

public:
    CaelumSampleFrameListener(RenderWindow* win, Camera* cam)
    {
        mScene = cam->getSceneManager();
        mPaused = false;

        // Pick components to create in the demo.
        // You can comment any of those and it should still work
        // Trying to disable one of these can be useful in finding problems.
        Caelum::CaelumSystem::CaelumComponent componentMask;
        componentMask = static_cast<Caelum::CaelumSystem::CaelumComponent> (
                Caelum::CaelumSystem::CAELUM_COMPONENT_SUN |				
                Caelum::CaelumSystem::CAELUM_COMPONENT_MOON |
                Caelum::CaelumSystem::CAELUM_COMPONENT_SKY_DOME |
                //Caelum::CaelumSystem::CAELUM_COMPONENT_IMAGE_STARFIELD |
                Caelum::CaelumSystem::CAELUM_COMPONENT_POINT_STARFIELD |
                Caelum::CaelumSystem::CAELUM_COMPONENT_CLOUDS |
                0);
        componentMask = CaelumSystem::CAELUM_COMPONENTS_DEFAULT;

        // Initialise CaelumSystem.
        mCaelumSystem = new Caelum::CaelumSystem (Root::getSingletonPtr(), mScene, componentMask);

        // Set time acceleration.
        mCaelumSystem->getUniversalClock ()->setTimeScale (512);

        // Register caelum as a listener.
        win->addListener (mCaelumSystem);
        Root::getSingletonPtr()->addFrameListener (mCaelumSystem);

        UpdateSpeedFactor(mCaelumSystem->getUniversalClock ()->getTimeScale ());
        mTimeTillNextUpdate = 0;
    }

    ~CaelumSampleFrameListener() {
        if (mCaelumSystem) {
            mCaelumSystem->shutdown (false);
            mCaelumSystem = 0;
        }
    }

    // Update speed factors.
    void UpdateSpeedFactor(double factor)
    {
        mSpeedFactor = factor;
        mCaelumSystem->getUniversalClock ()->setTimeScale (mPaused ? 0 : mSpeedFactor);
    }

    bool keyPressed(const OgreBites::KeyboardEvent& evt)
    {
        using namespace OgreBites;
        if (evt.keysym.sym == SDLK_ESCAPE) {
            Root::getSingletonPtr()->queueEndRendering ();
         }

        if (evt.keysym.sym == SDLK_SPACE) {
             mTimeTillNextUpdate = 1;
             mPaused = !mPaused;
             UpdateSpeedFactor(mSpeedFactor);
         }
         if (evt.keysym.sym == 'x') {
             mTimeTillNextUpdate = 0.25;
             UpdateSpeedFactor(mSpeedFactor / 2);
         }
         if (evt.keysym.sym == 'c') {
             mTimeTillNextUpdate = 0.25;
             UpdateSpeedFactor(mSpeedFactor * 2);
         }
         if (evt.keysym.sym == 'z') {
             mTimeTillNextUpdate = 1;
             UpdateSpeedFactor(mSpeedFactor * -1);
         }

        return true;
    }
};

class CaelumSampleApplication : public ExampleApplication
{
    Ogre::Plugin* mPlugin;
public:
    CaelumSampleApplication ()
    {
    }

    void loadResources() {
        mPlugin = new CaelumPlugin();
        mPlugin->install();
        ExampleApplication::loadResources();
    }

    void createFrameListener ()
    {
        mFrameListener = new CaelumSampleFrameListener (getRenderWindow(), mCamera);
        addInputListener(mFrameListener);
    }

    virtual void createCamera(void)
    {
        // Create the camera
        mCamera = mSceneMgr->createCamera("PlayerCam");

        // Start the camera on a hill in the middle of the terrain
        // looking towards Z+ (north).
        // Sun should rise in the east(left) and set in the west(right).
        mCameraNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        mCameraNode->setPosition (Vector3 (775, 100, 1997));
        mCameraNode->lookAt (Vector3 (0, 0, 0), Node::TS_PARENT);
        mCameraNode->attachObject(mCamera);

        mCamera->setNearClipDistance(5);

        // Set camera clip distances. It's important to test with
        // an infinite clip distance.
        mCamera->setFarClipDistance(0);
        //mCamera->setFarClipDistance(10000);
    }

    void createScene ()
    {
        mSceneMgr->getRootSceneNode()->attachObject(
                mSceneMgr->createEntity("House", "TudorHouse.mesh"));
        // needs porting to new terrain system
#if 0
        // Put some terrain in the scene
        std::string terrain_cfg("CaelumDemoTerrain.cfg");
        mSceneMgr->setWorldGeometry (terrain_cfg);
#endif
    }
};
