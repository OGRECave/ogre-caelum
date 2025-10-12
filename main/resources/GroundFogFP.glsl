// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
#include <OgreUnifiedShader.h>

// Returns (exp(x) - 1) / x; avoiding division by 0.
// lim when x -> 0 is 1.
float expdiv(float x) {
    if (abs(x) < 0.0001) {
        return 1;
    } else {
        return (exp(x) - 1) / x;
    }
}

// Return fogging through a layer of fog which drops exponentially by height.
//
// Standard exp fog with constant density would return (1 - exp(-density * dist)).
// This function assumes a variable density vd = exp(-verticalDecay * h - baseLevel)
// Full computation is exp(density * dist / (h2 - h1) * int(h1, h2, exp(-verticalDecay * (h2 - h1)))).
//
// This will gracefully degrade to standard exp fog in verticalDecay is 0; without throwing NaNs.
float ExpGroundFog (
    float dist, float h1, float h2,
    float density, float verticalDecay, float baseLevel)
{
    float deltaH = (h2 - h1);
    return 1 - exp (-density * dist * exp(verticalDecay * (baseLevel - h1)) * expdiv(-verticalDecay * deltaH));
}

OGRE_UNIFORMS(
		uniform vec3 camPos
		uniform vec4 fogColour
		uniform float fogDensity
		uniform float fogVerticalDecay
		uniform float fogGroundLevel
)

MAIN_PARAMETERS
    IN(vec3 worldPos, TEXCOORD0)
MAIN_DECLARATION
{
	float h1 = camPos.y;
	float h2 = worldPos.y;
	float dist = length(camPos - worldPos);
	float fog = ExpGroundFog(
	        dist, h1, h2,
	        fogDensity, fogVerticalDecay, fogGroundLevel);

	gl_FragColor.rgb = fogColour.rgb;
	gl_FragColor.a = fog;
}