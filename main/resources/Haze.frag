// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include "CaelumCommon.h"

SAMPLER2D(atmRelativeDepth, 0);

OGRE_UNIFORMS(
uniform vec4 fogColour;
)

MAIN_PARAMETERS
IN(float haze, TEXCOORD0)
IN(vec2 sunlight, TEXCOORD1)
MAIN_DECLARATION
{
    float incidenceAngleCos = sunlight.x;
    float y = sunlight.y;

    vec4 sunColour = vec4(3.0, 2.5, 1.0, 1.0);

    // Factor determining the amount of light lost due to absorption
    float atmLightAbsorptionFactor = 0.1;
    float fogDensity = 15.0;

    float hazeAmount = fogExp(haze * 0.005, atmLightAbsorptionFactor);

    // Haze amount calculation
    float invHazeHeight = 100.0;
    float hazeAbsorption = fogExp(pow(1.0 - y, invHazeHeight), fogDensity);

    vec4 hazeColour = fogColour;
    if (incidenceAngleCos > 0.0) {
        // Factor determining the amount of scattering for the sun light
        float sunlightScatteringFactor = 0.1;
        // Factor determining the amount of sun light intensity lost due to scattering
        float sunlightScatteringLossFactor = 0.3;

        vec4 sunlightInscatterColour = sunlightInscatter(
                sunColour,
                clamp((1.0 - texture2D(atmRelativeDepth, vec2(y, 0.5)).r) * hazeAbsorption, 0.0, 1.0),
                clamp(incidenceAngleCos, 0.0, 1.0),
                sunlightScatteringFactor) * (1.0 - sunlightScatteringLossFactor);
        hazeColour.rgb =
                hazeColour.rgb * (1.0 - sunlightInscatterColour.a) +
                sunlightInscatterColour.rgb * sunlightInscatterColour.a * hazeAmount;
    }

    gl_FragColor = vec4(hazeColour.rgb, hazeAmount);
}
