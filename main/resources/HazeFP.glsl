// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
#include <OgreUnifiedShader.h>
#include <SkyDomeFPCommon.h>

OGRE_UNIFORMS(
        uniform SAMPLER2D(atmRelativeDepth, 0); //  ~ changed from sampler1D
        uniform SAMPLER2D(gradientsMap, 1);
        uniform vec4 fogColour;
)

MAIN_PARAMETERS
        IN(float haze, TEXCOORD0)
        IN(vec2 sunlight, TEXCOORD1)
MAIN_DECLARATION
{
	float incidenceAngleCos = sunlight.x;
	float y = sunlight.y;

	vec4 sunColour = vec4 (3, 2.5, 1, 1);

    // Factor determining the amount of light lost due to absorption
	float atmLightAbsorptionFactor = 0.1; 
	float fogDensity = 15;

	haze = fogExp (haze * 0.005, atmLightAbsorptionFactor);

	// Haze amount calculation
	float invHazeHeight = 100;
	float hazeAbsorption = fogExp (pow (1 - y, invHazeHeight), fogDensity);

	vec4 hazeColour;
	hazeColour = fogColour;
	if (incidenceAngleCos > 0) {
        // Factor determining the amount of scattering for the sun light
		float sunlightScatteringFactor = 0.1;
        // Factor determining the amount of sun light intensity lost due to scattering
		float sunlightScatteringLossFactor = 0.3;	

		vec4 sunlightInscatterColour = sunlightInscatter (
                sunColour, 
                clamp ((1 - tex2D (atmRelativeDepth, y).r) * hazeAbsorption, 0, 1), 
                clamp (incidenceAngleCos, 0, 1), 
                sunlightScatteringFactor) * (1 - sunlightScatteringLossFactor);
		hazeColour.rgb =
                hazeColour.rgb * (1 - sunlightInscatterColour.a) +
                sunlightInscatterColour.rgb * sunlightInscatterColour.a * haze;
	}

	gl_FragColor = hazeColour;
	gl_FragColor.a = haze;
}