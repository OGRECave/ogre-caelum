// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#ifndef CAELUM__IMAGE_STARFIELD_H
#define CAELUM__IMAGE_STARFIELD_H

#include "CaelumPrerequisites.h"
#include "CameraBoundElement.h"
#include "PrivatePtr.h"

namespace Caelum
{
    /** Image-based starfield class.
     *  This class implements a starfield based on mapping a single large
     *  texture on a sphere. @see PointStarfield for a better solution.
     */
    class CAELUM_EXPORT ImageStarfield : public CameraBoundElement
    {
	protected:
		/// Reference to the dome node.
		PrivateSceneNodePtr mNode;

		/// Reference to the (cloned) starfield material.
		PrivateMaterialPtr mStarfieldMaterial;

		/// Reference to the dome entity.
		PrivateEntityPtr mEntity;

		/// Name of the spheric dome resource.
		static const Ogre::String STARFIELD_DOME_NAME;

		/// Name of the starfield material.
		static const Ogre::String STARFIELD_MATERIAL_NAME;

		/** Inclination of the starfield.
		 */
		Ogre::Degree mInclination;

	public:
        static const String DEFAULT_TEXTURE_NAME;

		/** Constructor.
			@param sceneMgr The scene manager this dome will belong to.
		 */
		ImageStarfield (
                Ogre::SceneManager *sceneMgr,
				Ogre::SceneNode *caelumRootNode,
                const Ogre::String &textureName = DEFAULT_TEXTURE_NAME);

		/** Destructor.
		 */
		virtual ~ImageStarfield ();

		/** Sets the starfield inclination. This inclination is the angle between the starfield rotation axis and the horizon plane.
			@param inc The starfield inclination in degrees. It`s equal to observer latitude taken with opposite sign.
		 */
		void setInclination (Ogre::Degree inc);

		/** Updates the starfield position/orientation.
			@param time Local time in [0, 1] range.
		 */
		void update (const float time);

		/** Updates the starfield material.
			@param mapName The new starfield texture map name.
		 */
		void setTexture (const Ogre::String &mapName);

    public:
		/// Handle camera change.
		virtual void notifyCameraChanged (Ogre::Camera *cam);

    protected:
        /// Handle far radius.
	    virtual void setFarRadius (Ogre::Real radius);

    public:
        void setQueryFlags (uint flags) { mEntity->setQueryFlags (flags); }
        uint getQueryFlags () const { return mEntity->getQueryFlags (); }
        void setVisibilityFlags (uint flags) { mEntity->setVisibilityFlags (flags); }
        uint getVisibilityFlags () const { return mEntity->getVisibilityFlags (); }
    };
}

#endif // CAELUM__IMAGE_STARFIELD_H
