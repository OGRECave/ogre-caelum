// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include "CaelumDemoCommon.h"

#include "CEGUI/CEGUI.h"
#include "Ogre/Renderer.h"
#include "ExampleApplication.h"
#include "ExampleFrameListener.h"

enum SampleMaterialScheme
{
    SMS_DEFAULT = 0,
    SMS_PSSM = 1,
};

class CaelumLabFrameListener:
        public ExampleFrameListener,
        private OIS::MouseListener,
        private OIS::KeyListener,
        public OgreBites::InputListener
{
private:
    SceneManager* mSceneMgr;

    std::auto_ptr<CaelumSystem> mCaelumSystem;

    // Reversing these two causes a crash.
    CEGUI::Renderer* mGuiRenderer;
    CEGUI::System* mGuiSystem;
    bool mShutdownRequested;

    void initCaelum ();
    void destroyCaelum ();
    void initGui ();

    // Quick fetch a widget. Checks existence and type.
    // Throws on not found.
    template<class WidgetT> inline WidgetT* getWidget (const char* name) {
        return static_cast<WidgetT*>(mGuiSystem->getDefaultGUIContext().getRootWindow()->getChild(name));
    }
    inline CEGUI::Window* getWindow (const char* name) {
        return getWidget<CEGUI::Window> (name);
    }

    inline Viewport* getViewport () { return mCamera->getViewport (); }
    inline Camera* getCamera () { return mCamera; }

private:
    // Handle OIS events and send them to CEGUI.
    bool mouseMoved (const OIS::MouseEvent &arg);
    bool mousePressed (const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    bool mouseReleased (const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    bool keyPressed (const OIS::KeyEvent &arg);
    bool keyReleased (const OIS::KeyEvent &arg);

    inline bool rightMouseDown () { return mMouse->getMouseState ().buttonDown (OIS::MB_Right); }
    inline bool leftMouseDown () { return mMouse->getMouseState ().buttonDown (OIS::MB_Left); }

    bool mLastMousePositionSet;
    CEGUI::Vector2<float> mLastMousePosition;

    // CEGUI event handling.
	bool handleMouseMove (const CEGUI::EventArgs& evt);
	bool handleMouseUp (const CEGUI::EventArgs& evt);
	bool handleMouseDown (const CEGUI::EventArgs& evt);

    bool handleTimeScaleDoubleClick (const CEGUI::EventArgs& args);
    bool handleResetClick (const CEGUI::EventArgs& args);
    bool handleQuitClick (const CEGUI::EventArgs& args);
    bool handleDumpClick (const CEGUI::EventArgs& args);

    // Various obscure debug functions.
    void dumpRenderTargets ();
    void dumpRenderQueueShadows ();
    void dumpMaterialNames ();

    /** Update a scrolling tweaker.
     *  The value is only changed if the control is currently focused.
     *  @returns if the value was changed. 
     */ 
    bool updateScrollbarTweak (
            const Ogre::String& editboxName,
            const Ogre::String& scrollbarName,
            Real& value,
            Real minValue,
            Real maxValue,
            Real magicValue,
            Real magicPosition,
            Real exponent);

    /** Limited variant of the above; w/o exponential magic.
     */
    bool updateScrollbarTweak (
            const Ogre::String& editboxName,
            const Ogre::String& scrollbarName,
            Real& value, Real minValue, Real maxValue);

    // Frame updates.
    bool frameStarted (const FrameEvent& evt);
    bool updateInput (const FrameEvent& evt);
    void updateWidgets ();

    // Can be called multiple times; will reset.
    // Will also updateDepthComposerWidgets()
    void initDepthComposer ();
    void disableDepthComposer ();
    void setupDepthComposerFlags ();

    void initDepthComposerWidgets ();
    void updateDepthComposerWidgets ();

private:
    SampleMaterialScheme mMaterialScheme;
    bool mTerrainShadowsOn;

    // Ogre's default terrain scene manager does not support shadows.
    // Still it's possible to hack our way through and force the renderables
    // which make up the terrain to cast shadows.
    //
    // The terrain scene manager creates a bunch of TerrainRenderables which
    // implement Renderable and MovableObject. By default those set
    // MovableObject:mCastShadows to false but we can flip that externally.
    void setTerrainShadowsOn(bool value);
    bool getTerrainShadowsOn() { return mTerrainShadowsOn; }

    void initMaterialSchemeUI ();
    void updateMaterialSchemeUI ();
    bool handleForceTerrainShadowsChanged (const CEGUI::EventArgs& args);

#if OGRE_VERSION >= 0x010600
    void setupPSSM ();
    void updatePSSMCamera ();
#endif

public:
    CaelumLabFrameListener (RenderWindow* win, Camera* cam);
    ~CaelumLabFrameListener ();

private:
    class ScriptingUI 
    {
    public:
        ScriptingUI (CaelumLabFrameListener* parent);
        ~ScriptingUI ();

        void init ();

    private:
        struct LoadableScript {
            String objectName;
        };
        typedef std::vector<LoadableScript> LoadableScriptList;
        LoadableScriptList mLoadableScriptList;

        void initLoadableScriptList ();

    private:
        void clearSelection ();
        bool handleLoadScript (const CEGUI::EventArgs& args);

        CaelumLabFrameListener* mParent;
    };

    ScriptingUI mScriptingUI;
};

class CaelumLabApplication: public ExampleApplication
{
    Ogre::Plugin* mPlugin;
public:
    void createFrameListener ();
    void chooseSceneManager ();
    void createCamera ();
    void createScene ();

    void loadResources() {
        mPlugin = new CaelumPlugin();
        mPlugin->install();
        ExampleApplication::loadResources();
    }

private:
    /// Return height at point or -1 if nothing hit.
    Real getHeightAtPoint (Real x, Real z);
};
