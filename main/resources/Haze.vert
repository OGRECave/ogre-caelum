// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
uniform vec3 camPos;
uniform vec3 sunDirection;
uniform mat4 worldViewProj;
)

MAIN_PARAMETERS
IN(vec4 vertex, POSITION)
OUT(float haze, TEXCOORD0)
OUT(vec2 sunlight, TEXCOORD1)
MAIN_DECLARATION
{
    vec3 sunDir = normalize(sunDirection);
    gl_Position = mul(worldViewProj, vertex);
    haze = length(camPos - vertex.xyz);
    sunlight.x = dot(-sunDir, normalize(vertex.xyz - camPos));
    sunlight.y = -sunDir.y;
}
