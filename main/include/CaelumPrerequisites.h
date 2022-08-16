// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#ifndef CAELUM__CAELUM_PREREQUISITES_H
#define CAELUM__CAELUM_PREREQUISITES_H

// Include external headers
#ifdef __APPLE__
#include "Ogre/Ogre.h"
#else
#include "Ogre.h"
#include "OgreBuildSettings.h"
#endif

#include "CaelumConfig.h"

#include <memory>

// Define the dll export qualifier if compiling for Windows
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	#ifdef CAELUM_LIB
		#define CAELUM_EXPORT __declspec (dllexport)
	#else
		#ifdef __MINGW32__
			#define CAELUM_EXPORT
		#else
			#define CAELUM_EXPORT __declspec (dllimport)
		#endif
	#endif
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	#define CAELUM_EXPORT __attribute__ ((visibility("default")))
#else
	#define CAELUM_EXPORT
#endif

// By default only compile type descriptors for scripting.
#ifndef CAELUM_TYPE_DESCRIPTORS
    #define CAELUM_TYPE_DESCRIPTORS 1
#endif

// Scripting support requires Ogre 1.6
// Can be also configured on compiler command line
#ifndef CAELUM_SCRIPT_SUPPORT
    #define CAELUM_SCRIPT_SUPPORT 1
#endif

#if CAELUM_SCRIPT_SUPPORT
    #if !(CAELUM_TYPE_DESCRIPTORS)
        #error "Caelum script support also requires type descriptors."
    #endif
#endif


/** Caelum namespace
 *
 *  All of %Caelum is inside this namespace (except for macros).
 *
 *  @note: This was caelum with a lowercase 'c' in version 0.3
 */
namespace Caelum
{
    // Caelum needs a lot of precission for astronomical calculations.
    // Very few calculations use it, and the precission IS required.
    typedef double LongReal;

    // Use some ogre types.
    using Ogre::uint8;
    using Ogre::uint16;
    using Ogre::ushort;
    using Ogre::uint32;
    using Ogre::uint;

    using Ogre::Real;
    using Ogre::String;

    /// Resource group name for caelum resources.
    static const String RESOURCE_GROUP_NAME = "Caelum";

    // Render group for caelum stuff
    // It's best to have them all together
    enum CaelumRenderQueueGroupId
    {
        CAELUM_RENDER_QUEUE_STARFIELD       = Ogre::RENDER_QUEUE_SKIES_EARLY + 0,
		CAELUM_RENDER_QUEUE_MOON_BACKGROUND = Ogre::RENDER_QUEUE_SKIES_EARLY + 1,
        CAELUM_RENDER_QUEUE_SKYDOME         = Ogre::RENDER_QUEUE_SKIES_EARLY + 2,
		CAELUM_RENDER_QUEUE_MOON            = Ogre::RENDER_QUEUE_SKIES_EARLY + 3,
        CAELUM_RENDER_QUEUE_SUN             = Ogre::RENDER_QUEUE_SKIES_EARLY + 4,
        CAELUM_RENDER_QUEUE_CLOUDS          = Ogre::RENDER_QUEUE_SKIES_EARLY + 5,
        CAELUM_RENDER_QUEUE_GROUND_FOG      = Ogre::RENDER_QUEUE_SKIES_EARLY + 6,
    };

    // Forward declarations
    class UniversalClock;
    class SkyDome;
    class BaseSkyLight;
    class Moon;
    class SphereSun;
    class SpriteSun;
    class ImageStarfield;
    class PointStarfield;
    class CloudSystem;
    class CaelumSystem;
	class FlatCloudLayer;
    class PrecipitationController;
    class PrecipitationInstance;
    class GroundFog;
    class DepthComposer;
    class DepthComposerInstance;
    class DepthRenderer;
}

#endif // CAELUM__CAELUM_PREREQUISITES_H
