// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#ifndef CAELUM__DEPTH_COMPOSER_H
#define CAELUM__DEPTH_COMPOSER_H

#include "CaelumPrerequisites.h"
#include "FastGpuParamRef.h"

namespace Caelum
{
    /** Compositor-based precipitation controller.
     *  This class will add and control precipitation controllers to viewports.
     *
     *  Compositors clone the composing materials. This controller will
     *  register itself as a compositor listener and change the material in notifyMaterialSetup.
     */
	class CAELUM_EXPORT DepthComposer
	{
	private:
		Ogre::SceneManager *mSceneMgr;

        /// The compositor must not be replaced while rendering; this is done in update
        bool mCompositorChanged;
        void onCompositorMaterialChanged ();
        const String& getCompositorName ();

    protected:
        inline Ogre::SceneManager* getSceneManager() const { return mSceneMgr; }

        friend class DepthComposerInstance;

    public:
		DepthComposer(Ogre::SceneManager *sceneMgr);
		virtual ~DepthComposer();

        /// Applies changes of the enabled effects. Called by CaelumSystem::updateSubcomponents.
        void update ();

    public:
        typedef std::map<Ogre::Viewport*, DepthComposerInstance*> ViewportInstanceMap;
        ViewportInstanceMap mViewportInstanceMap;

    public:
        DepthComposerInstance* createViewportInstance(Ogre::Viewport* viewport);
        void destroyViewportInstance(Ogre::Viewport* viewport);
        DepthComposerInstance* getViewportInstance(Ogre::Viewport* viewport);
        void destroyAllViewportInstances();

    private:
        bool mDebugDepthRender;

    public:
		/// Enables drawing the depth buffer
		void setDebugDepthRender (bool value);
		bool getDebugDepthRender () const { return mDebugDepthRender; }

    private:
        bool mSkyDomeHazeEnabled;
        Ogre::Vector3 mSunDirection;
        Ogre::ColourValue mHazeColour;

    public:
		/// Enables skydome haze.
		void setSkyDomeHazeEnabled (bool value);
		bool getSkyDomeHazeEnabled () const { return mSkyDomeHazeEnabled; }

        void setSunDirection (const Ogre::Vector3& value) { mSunDirection = value; }
        const Ogre::Vector3 getSunDirection () const { return mSunDirection; }

        void setHazeColour (const Ogre::ColourValue& value) { mHazeColour = value; }
        const Ogre::ColourValue getHazeColour () const { return mHazeColour; }

    private:
        bool mGroundFogEnabled;
        Real mGroundFogDensity;
        Real mGroundFogBaseLevel;
        Real mGroundFogVerticalDecay;
        Ogre::ColourValue mGroundFogColour;

    public:
		/// Enables exponential ground fog.
		void setGroundFogEnabled (bool value);
		bool getGroundFogEnabled () const { return mGroundFogEnabled; }

		/// Sets ground fog density
		void setGroundFogDensity (Real value) { mGroundFogDensity = value; }

		/// Get ground fog density
		Real getGroundFogDensity () const { return mGroundFogDensity; }

		/// Sets ground fog level
        /// At ground level fogginess is equal to GroundFogDensity
		void setGroundFogBaseLevel (Real value) { mGroundFogBaseLevel = value; }

		/// Get ground fog density
		Real getGroundFogBaseLevel () const { return mGroundFogBaseLevel; }

		/// Sets ground fog vertical decay
		void setGroundFogVerticalDecay (Real value) { mGroundFogVerticalDecay = value; }

		/// Get ground fog density
		Real getGroundFogVerticalDecay () const { return mGroundFogVerticalDecay; }

		/// Sets ground fog colour
		void setGroundFogColour (const Ogre::ColourValue& value) { mGroundFogColour = value; }

		/// Get ground fog colour
		const Ogre::ColourValue getGroundFogColour () const { return mGroundFogColour; }
	};

    /** Per-viewport instance of @see DepthComposer
     *  This will create and control one ogre::CompositorInstance.
     */
    class CAELUM_EXPORT DepthComposerInstance: private Ogre::CompositorInstance::Listener
    {
    private:
        DepthComposer* mParent;
        Ogre::Viewport* mViewport;
        Ogre::CompositorInstance* mCompInst;

        virtual void notifyMaterialSetup(uint pass_id, Ogre::MaterialPtr &mat);
        virtual void notifyMaterialRender(uint pass_id, Ogre::MaterialPtr &mat);

        struct Params {
            void setup(Ogre::GpuProgramParametersSharedPtr params);

            Ogre::GpuProgramParametersSharedPtr fpParams;
            FastGpuParamRef invViewProjMatrix;
            FastGpuParamRef worldCameraPos;
            FastGpuParamRef groundFogDensity;
            FastGpuParamRef groundFogVerticalDecay;
            FastGpuParamRef groundFogBaseLevel;
            FastGpuParamRef groundFogColour;
            FastGpuParamRef sunDirection;
            FastGpuParamRef hazeColour;
        } mParams;

    protected:
        void addCompositor ();
        void removeCompositor ();
        bool isCompositorEnabled () { return mCompInst != 0; }

        friend class DepthComposer;

    public:
        /// Get parent DepthComposer; with all the interesting parameters.
        DepthComposer* getParent() const { return mParent; }

        /// Get the viewport this instance is attached to.
        Ogre::Viewport* getViewport() const { return mViewport; }

        /// Get compositor instance; attached to the viewport.
        Ogre::CompositorInstance* getCompositorInstance() const { return mCompInst; }

        DepthComposerInstance(DepthComposer* parent, Ogre::Viewport* view);
        virtual ~DepthComposerInstance();
    };
}

#endif // CAELUM__DEPTH_COMPOSER_H
