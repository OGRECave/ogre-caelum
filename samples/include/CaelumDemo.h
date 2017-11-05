// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include "CaelumDemoCommon.h"
#include "ExampleApplication.h"

class CaelumSampleFrameListener : public ExampleFrameListener
{
protected:
    Caelum::CaelumSystem *mCaelumSystem;
    Ogre::SceneManager *mScene;
    float mSpeedFactor;
    bool mPaused;
    float mTimeTillNextUpdate;

public:
    CaelumSampleFrameListener(RenderWindow* win, Camera* cam): ExampleFrameListener(win, cam)
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
        mWindow->addListener (mCaelumSystem);
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

    bool frameEnded(const FrameEvent& evt)
    {
        if (!ExampleFrameListener::frameEnded(evt)) {
            return false;
        }

        // Stop key repeat for these keys.
        mTimeTillNextUpdate -= evt.timeSinceLastFrame;
        if (mTimeTillNextUpdate <= 0) {
            if (mKeyboard->isKeyDown (OIS::KC_SPACE)) {
                mTimeTillNextUpdate = 1;
                mPaused = !mPaused;
                UpdateSpeedFactor(mSpeedFactor);
            }
            if (mKeyboard->isKeyDown (OIS::KC_X)) {
                mTimeTillNextUpdate = 0.25;
                UpdateSpeedFactor(mSpeedFactor / 2);
            }
            if (mKeyboard->isKeyDown (OIS::KC_C)) {
                mTimeTillNextUpdate = 0.25;
                UpdateSpeedFactor(mSpeedFactor * 2);
            }
            if (mKeyboard->isKeyDown (OIS::KC_Z)) {
                mTimeTillNextUpdate = 1;
                UpdateSpeedFactor(mSpeedFactor * -1);
            }
        }

        return true;
    }
};

class CaelumSampleApplication : public ExampleApplication
{
public:
    CaelumSampleApplication ()
    {
    }

    ~CaelumSampleApplication ()
    {
    }

    void createFrameListener ()
    {
        mFrameListener = new CaelumSampleFrameListener (mWindow, mCamera);
        mRoot->addFrameListener (mFrameListener);
    }

    virtual void chooseSceneManager(void)
    {
        mSceneMgr = mRoot->createSceneManager("TerrainSceneManager");
    }

    virtual void createCamera(void)
    {
        // Create the camera
        mCamera = mSceneMgr->createCamera("PlayerCam");

        // Start the camera on a hill in the middle of the terrain
        // looking towards Z+ (north).
        // Sun should rise in the east(left) and set in the west(right).
        mCamera->setPosition (Vector3 (775, 100, 997));
        mCamera->lookAt (Vector3 (775, 100, 1000));

        mCamera->setNearClipDistance(5);

        // Set camera clip distances. It's important to test with
        // an infinite clip distance.
        mCamera->setFarClipDistance(0);
        //mCamera->setFarClipDistance(10000);
    }

    void createScene ()
    {
        // Put some terrain in the scene
        std::string terrain_cfg("CaelumDemoTerrain.cfg");
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
        terrain_cfg = mResourcePath + terrain_cfg;
#endif
        mSceneMgr->setWorldGeometry (terrain_cfg);
    }
};
