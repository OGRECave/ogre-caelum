// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE

#include "CaelumCommon.h"

SAMPLER2D(gradientsMap, 0);
SAMPLER2D(atmRelativeDepth, 1);

OGRE_UNIFORMS(
uniform vec4 hazeColour;
uniform float offset;
)

MAIN_PARAMETERS
IN(vec2 oUv, TEXCOORD0)
IN(float incidenceAngleCos, TEXCOORD1)
IN(float y, TEXCOORD2)
IN(vec3 oNormal, TEXCOORD3)
MAIN_DECLARATION
{
    vec4 sunColour = vec4(3.0, 3.0, 3.0, 1.0);

#ifdef HAZE
    float fogDensity = 15.0;
    // Haze amount calculation
    float invHazeHeight = 100.0;
    float haze = fogExp(pow(clamp(1.0 - oNormal.y, 0.0, 1.0), invHazeHeight), fogDensity);
#endif // HAZE

    // Pass the colour
    vec4 oCol = texture2D(gradientsMap, oUv + vec2(offset, 0.0));

    // Sunlight inscatter
    if (incidenceAngleCos > 0.0)
    {
        float sunlightScatteringFactor = 0.05;
        float sunlightScatteringLossFactor = 0.1;
        float atmLightAbsorptionFactor = 0.1;

        oCol.rgb += sunlightInscatter(
                sunColour,
                clamp(atmLightAbsorptionFactor * (1.0 - texture2D(atmRelativeDepth, vec2(y, 0.5)).r), 0.0, 1.0),
                clamp(incidenceAngleCos, 0.0, 1.0),
                sunlightScatteringFactor).rgb * (1.0 - sunlightScatteringLossFactor);
    }

#ifdef HAZE
    // Haze pass
    oCol = oCol * (1.0 - haze) + vec4(hazeColour.rgb, 1.0) * haze;
#endif // HAZE

    gl_FragColor = oCol;
}
