// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#ifndef CAELUM__MOON_H
#define CAELUM__MOON_H

#include "CaelumPrerequisites.h"
#include "SkyLight.h"
#include "FastGpuParamRef.h"
#include "PrivatePtr.h"

namespace Caelum
{
    /** Class representing the moon.
     *  Drawn as a single billboard after the skydome, so that it also works in
     *  daylight; the shader keeps the disc opaque to hide the sky behind it.
     */
    class CAELUM_EXPORT Moon:
            public BaseSkyLight
    {
	public:
		/// Name of the moon material.
		static const Ogre::String MOON_MATERIAL_NAME;

	private:
        /// Material for MoonBB
		PrivateMaterialPtr mMoonMaterial;

		/// The moon sprite.
		PrivateBillboardSetPtr mMoonBB;

		/// The moon sprite visible angle
		Ogre::Degree mAngularSize;

        struct Params {
            void setup(Ogre::GpuProgramParametersSharedPtr fpParams);

            Ogre::GpuProgramParametersSharedPtr fpParams;
            FastGpuParamRef phase;
        } mParams;

	public:
		/** Constructor.
		 */
		Moon (
				Ogre::SceneManager *sceneMgr,
				Ogre::SceneNode *caelumRootNode,
				const Ogre::String& moonTextureName = "moon_disc.dds", 
				Ogre::Degree angularSize = Ogre::Degree(3.77f));

		virtual ~Moon ();

		/** Updates the moon material.
			@param textureName The new moon texture name.
		 */
		void setMoonTexture (const Ogre::String &textureName);
		
		/** Updates the moon size.
			@param moon TextureAngularSize The new moon texture angular size.
		 */
		void setMoonTextureAngularSize(const Ogre::Degree& moonTextureAngularSize);

		/** Sets the moon sphere colour.
			@param colour The colour used to draw the moon
		 */
		void setBodyColour (const Ogre::ColourValue &colour);

		/// Set the moon's phase
		void setPhase (Ogre::Real phase);
		
		/// Set Moon`s north pole direction
		void setMoonNorthPoleDirection(const Ogre::Vector3& moonNorthPoleDir);

    public:
		/// Handle camera change.
		virtual void notifyCameraChanged (Ogre::Camera *cam);

        virtual void setQueryFlags (uint flags);
        virtual uint getQueryFlags () const;
        virtual void setVisibilityFlags (uint flags);
        virtual uint getVisibilityFlags () const;
    };
}

#endif // CAELUM__MOON_H
