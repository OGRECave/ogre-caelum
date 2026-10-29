// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE

#include "CaelumCommon.h"

OGRE_UNIFORMS(
uniform f32vec3 camPos;
uniform vec4 fogColour;
uniform float fogDensity;
uniform float fogVerticalDecay;
uniform float fogGroundLevel;
)

MAIN_PARAMETERS
IN(f32vec4 worldPos, TEXCOORD0)
MAIN_DECLARATION
{
    float32_t h1 = camPos.y;
    float32_t h2 = worldPos.y;
    float32_t dist = length(camPos - worldPos.xyz);
    float fog = ExpGroundFog(
            dist, h1, h2,
            fogDensity, fogVerticalDecay, fogGroundLevel);

    gl_FragColor = vec4(fogColour.rgb, fog);
}
