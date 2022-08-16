// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#ifndef CAELUM__SKYDOME_H
#define CAELUM__SKYDOME_H

#include "CaelumPrerequisites.h"
#include "CameraBoundElement.h"
#include "FastGpuParamRef.h"
#include "PrivatePtr.h"

namespace Caelum
{
    /** A sky dome element.
     */
    class CAELUM_EXPORT SkyDome : public CameraBoundElement
    {
	private:
		/** Name of the spheric dome resource.
		 */
		static const Ogre::String SPHERIC_DOME_NAME;

		/** Name of the dome material.
		 */
		static const Ogre::String SKY_DOME_MATERIAL_NAME;

		/// Control scene node.
		PrivateSceneNodePtr mNode;

		/// Sky dome material.
		PrivateMaterialPtr mMaterial;

        /// Sky dome entity.
        PrivateEntityPtr mEntity;

    private:
		/// True if selected technique has shaders.
		bool mShadersEnabled;

        /// If haze is enabled.
		bool mHazeEnabled;

	public:
		/** Constructor
         *  This will setup some nice defaults.
		 *  @param sceneMgr The scene manager where this sky dome will be created.
		 */
		SkyDome (Ogre::SceneManager *sceneMgr, Ogre::SceneNode *caelumRootNode);

		/** Destructor
		 */
		virtual ~SkyDome ();

		/** Sets the sun direction.
			@param dir The sun light direction.
		 */
		void setSunDirection (const Ogre::Vector3& dir);

        /// Explicit haze colour.
        void setHazeColour (const Ogre::ColourValue& hazeColour);

        /// Set the sky color gradients image.
        void setSkyGradientsImage (const Ogre::String& gradients);

        /// Set the atmosphere depthh gradient image.
        void setAtmosphereDepthImage (const Ogre::String& gradients);

        /** Enable or disable skydome haze. This makes the sky darker.
         *  By default haze is disabled.
         */
        void setHazeEnabled (bool value);

        /// If skydome haze is enabled.
        bool getHazeEnabled () const;

        void setQueryFlags (uint flags) { mEntity->setQueryFlags (flags); }
        uint getQueryFlags () const { return mEntity->getQueryFlags (); }
        void setVisibilityFlags (uint flags) { mEntity->setVisibilityFlags (flags); }
        uint getVisibilityFlags () const { return mEntity->getVisibilityFlags (); }

    public:
		/// Handle camera change.
		virtual void notifyCameraChanged (Ogre::Camera *cam);

    protected:
        /// Handle far radius.
	    virtual void setFarRadius (Ogre::Real radius);

    private:
        struct Params {
            void setup(Ogre::GpuProgramParametersSharedPtr vpParams, Ogre::GpuProgramParametersSharedPtr fpParams);

            Ogre::GpuProgramParametersSharedPtr vpParams;
            Ogre::GpuProgramParametersSharedPtr fpParams;
            FastGpuParamRef sunDirection;
            FastGpuParamRef offset;
            FastGpuParamRef hazeColour;
        } mParams;
    };
}

#endif //CAELUM__SKYDOME_H
