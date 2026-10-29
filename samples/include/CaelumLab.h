// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include "CaelumDemoCommon.h"
#include "ExampleApplication.h"

#include <OgreCameraMan.h>
#include <OgreImGuiOverlay.h>
#include <OgreTerrainGroup.h>

#include <functional>

/** Passes input to the camera man; mouse look only while the right button is pressed.
 */
class CaelumLabInputListener : public OgreBites::InputListener
{
    OgreBites::ApplicationContext* mApp;
    OgreBites::CameraMan* mCameraMan;
    bool mMouseLook;

public:
    CaelumLabInputListener (OgreBites::ApplicationContext* app, OgreBites::CameraMan* cameraMan):
            mApp (app), mCameraMan (cameraMan), mMouseLook (false)
    {
    }

    void frameRendered (const Ogre::FrameEvent& evt) { mCameraMan->frameRendered (evt); }
    bool keyPressed (const OgreBites::KeyboardEvent& evt);
    bool keyReleased (const OgreBites::KeyboardEvent& evt) { return mCameraMan->keyReleased (evt); }
    bool mouseMoved (const OgreBites::MouseMotionEvent& evt);
    bool mousePressed (const OgreBites::MouseButtonEvent& evt);
    bool mouseReleased (const OgreBites::MouseButtonEvent& evt);
};

class CaelumLabApplication : public ExampleApplication, public Ogre::RenderTargetListener
{
public:
    CaelumLabApplication ();

private:
    std::unique_ptr<CaelumSystem> mCaelumSystem;

    std::unique_ptr<TerrainGlobalOptions> mTerrainGlobals;
    std::unique_ptr<TerrainGroup> mTerrainGroup;

    std::unique_ptr<OgreBites::CameraMan> mCameraMan;
    std::unique_ptr<CaelumLabInputListener> mLabInputListener;

    // Changes to the scene that must not happen while rendering a viewport.
    std::vector<std::function<void ()>> mDeferredActions;

    bool mFollowSun;
    bool mFollowMoon;
    bool mShadowsEnabled;
    ShadowCameraSetupPtr mPSSMSetup;

    void loadResources ();
    void createCamera ();
    void createScene ();
    void createFrameListener ();
    void destroyScene ();

    bool frameEnded (const Ogre::FrameEvent& evt);

    void initCaelum ();
    void destroyCaelum ();

    void initDepthComposer ();

    void setShadowsEnabled (bool enabled);
    void updatePSSMSplitPoints ();

    // Draw the ImGui windows; once per frame.
    void preViewportUpdate (const Ogre::RenderTargetViewportEvent& evt);
    void drawUI ();
    void drawTimeUI ();
    void drawCameraUI ();
    void drawSunMoonUI ();
    void drawCloudsUI ();
    void drawStarfieldUI ();
    void drawPrecipitationUI ();
    void drawDepthComposerUI ();
    void drawShadowsUI ();
    void drawScriptsUI ();
    void drawDebugUI ();

    // Various obscure debug functions.
    void dumpRenderTargets ();
    void dumpMaterialNames ();

    Viewport* getViewport () { return getRenderWindow ()->getViewport (0); }
};
