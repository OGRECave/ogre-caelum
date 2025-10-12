// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
#include <OgreUnifiedShader.h>
#include <SkyDomeFPCommon.h>

OGRE_UNIFORMS(
    uniform SAMPLER2D(gradientsMap, 0);
    uniform SAMPLER2D(atmRelativeDepth, 1); ~ changed from sampler1D
    uniform vec4 hazeColour;
    uniform float offset;
)

MAIN_PARAMETERS
    IN(vec4 col, COLOR)
    IN(float2 uv, TEXCOORD0)
    IN(float incidenceAngleCos, TEXCOORD1)
    IN(float y, TEXCOORD2)
    IN(vec3 normal, TEXCOORD3)
MAIN_DECLARATION
{
	vec4 sunColour = vec4 (3, 3, 3, 1);

#ifdef HAZE
	float fogDensity = 15;
	// Haze amount calculation
	float invHazeHeight = 100;
	float haze = fogExp (pow (clamp (1 - normal.y, 0, 1), invHazeHeight), fogDensity);
#endif // HAZE

	// Pass the colour
	oCol = tex2D (gradientsMap, uv + float2 (offset, 0)) * col;

	// Sunlight inscatter
	if (incidenceAngleCos > 0)
    {
		float sunlightScatteringFactor = 0.05;
		float sunlightScatteringLossFactor = 0.1;
		float atmLightAbsorptionFactor = 0.1;
		
		oCol.rgb += sunlightInscatter (
                sunColour, 
                clamp (atmLightAbsorptionFactor * (1 - tex1D (atmRelativeDepth, y).r), 0, 1), 
                clamp (incidenceAngleCos, 0, 1), 
                sunlightScatteringFactor).rgb * (1 - sunlightScatteringLossFactor);
	}

#ifdef HAZE
	// Haze pass
	hazeColour.a = 1;
	oCol = oCol * (1 - haze) + hazeColour * haze;
#endif // HAZE
}