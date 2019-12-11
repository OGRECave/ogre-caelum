/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
LGPL like the rest of the engine.
-----------------------------------------------------------------------------
*/
/*
-----------------------------------------------------------------------------
Filename:    ExampleApplication.h
Description: Base class for all the OGRE examples
-----------------------------------------------------------------------------
*/

#ifndef __ExampleApplication_H__
#define __ExampleApplication_H__

#include "Ogre.h"
#include "OgreConfigFile.h"
#include <OgreInput.h>
#include <OgreApplicationContext.h>

using namespace Ogre;

/** Base class which manages the standard startup of an Ogre application.
    Designed to be subclassed for specific examples if required.
*/
class ExampleApplication : public OgreBites::ApplicationContext
{
public:
    /// Standard constructor
    ExampleApplication() : OgreBites::ApplicationContext()
    {
        mFrameListener = 0;
    }
    /// Standard destructor
    void shutdown()
    {
        if (mFrameListener)
            delete mFrameListener;

        OgreBites::ApplicationContext::shutdown();
    }

    /// Start the example
    virtual void go(void)
    {
        initApp();

        getRoot()->startRendering();

        // clean up
        destroyScene();
        closeApp();
    }

protected:
    Camera* mCamera;
    SceneNode* mCameraNode;
    SceneManager* mSceneMgr;
    OgreBites::InputListener* mFrameListener;

    // These internal methods package up the stages in the startup process
    /** Sets up the application - returns false if the user chooses to abandon configuration. */
    virtual void setup(void)
    {
		OgreBites::ApplicationContext::setup();

        chooseSceneManager();
        createCamera();
        createViewports();

		// Create the scene
        createScene();

        createFrameListener();
    }

    virtual void chooseSceneManager(void)
    {
        // Create the SceneManager, in this case a generic one
        mSceneMgr = mRoot->createSceneManager("DefaultSceneManager", "ExampleSMInstance");
    }
    virtual void createCamera(void)
    {
        // Create the camera
        mCamera = mSceneMgr->createCamera("PlayerCam");

        mCameraNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        mCameraNode->attachObject(mCamera);

        // Position it at 500 in Z direction
        mCameraNode->setPosition(Vector3(0,0,500));
        // Look back along -Z
        mCameraNode->lookAt(Vector3(0,0,-300), Node::TS_PARENT);
        mCamera->setNearClipDistance(5);
    }
    virtual void createFrameListener(void)
    {
        //mFrameListener= new ExampleFrameListener(mWindow, mCamera);
        //mFrameListener->showDebugOverlay(true);
        //mRoot->addFrameListener(mFrameListener);
    }

    virtual void createScene(void) = 0;    // pure virtual - this has to be overridden

    virtual void destroyScene(void){}    // Optional to override this

    virtual void createViewports(void)
    {
        // Create one viewport, entire window
        Viewport* vp = getRenderWindow()->addViewport(mCamera);
        vp->setBackgroundColour(ColourValue(0,0,0));

        // Alter the camera aspect ratio to match the viewport
        mCamera->setAspectRatio(
            Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
    }

	/// Optional override method where you can create resource listeners (e.g. for loading screens)
	virtual void createResourceListener(void)
	{

	}

};


#endif
