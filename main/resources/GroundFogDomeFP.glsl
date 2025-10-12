// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
#include <OgreUnifiedShader.h>

// Just like ExpGroundFog with h2 = positive infinity
// When h2 == negative infinity the value is always +1.
float ExpGroundFogInf (
    float invSinView, float h1,
    float density, float verticalDecay, float baseLevel)
{
    return 1 - exp (-density * invSinView * exp(verticalDecay * (baseLevel - h1)) * (1 / verticalDecay));
}


OGRE_UNIFORMS(
		uniform float cameraHeight
		uniform vec4 fogColour
		uniform float fogDensity
		uniform float fogVerticalDecay
		uniform float fogGroundLevel
)

MAIN_PARAMETERS
    IN(vec3 relPosition, TEXCOORD0)
MAIN_DECLARATION
{
	// Fog magic.
	float invSinView = 1 / (relPosition.y);
	float h1 = cameraHeight;
	float aFog;

    if (fogVerticalDecay < 1e-7) {
        // A value of zero of fogVerticalDecay would result in maximum (1) aFog everywhere.
        // Output 0 zero instead to disable.
        aFog = 0;
    } else {
	    if (invSinView < 0) {
		    // Gazing into the abyss
		    aFog = 1;
	    } else {
		    aFog = saturate (ExpGroundFogInf (
			        invSinView, h1,
			        fogDensity, fogVerticalDecay, fogGroundLevel));
	    }
    }
	
	gl_FragColor.a = aFog;
	gl_FragColor.rgb = fogColour.rgb;
}