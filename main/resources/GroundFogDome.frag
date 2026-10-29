// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include "CaelumCommon.h"

OGRE_UNIFORMS(
uniform float cameraHeight;
uniform vec4 fogColour;
uniform float fogDensity;
uniform float fogVerticalDecay;
uniform float fogGroundLevel;
)

MAIN_PARAMETERS
IN(vec3 relPosition, TEXCOORD0)
MAIN_DECLARATION
{
    // Fog magic.
    float invSinView = 1.0 / (relPosition.y);
    float h1 = cameraHeight;
    float aFog;

    if (fogVerticalDecay < 1e-7) {
        // A value of zero of fogVerticalDecay would result in maximum (1) aFog everywhere.
        // Output 0 zero instead to disable.
        aFog = 0.0;
    } else {
        if (invSinView < 0.0) {
            // Gazing into the abyss
            aFog = 1.0;
        } else {
            aFog = saturate(ExpGroundFogInf(
                    invSinView, h1,
                    fogDensity, fogVerticalDecay, fogGroundLevel));
        }
    }

    gl_FragColor = vec4(fogColour.rgb, aFog);
}
