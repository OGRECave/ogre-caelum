// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
uniform vec3 sunDirection;
uniform mat4 worldViewProj;
)

MAIN_PARAMETERS
IN(vec4 vertex, POSITION)
IN(vec4 normal, NORMAL)
IN(vec2 uv0, TEXCOORD0)
OUT(vec2 oUv, TEXCOORD0)
OUT(float incidenceAngleCos, TEXCOORD1)
OUT(float y, TEXCOORD2)
OUT(vec3 oNormal, TEXCOORD3)
MAIN_DECLARATION
{
    vec3 sunDir = normalize(sunDirection);
    // Note: this deliberately normalizes the 4 component normal (w = 1),
    // which the sky colours have been tuned with.
    vec4 n = normalize(normal);
    float cosine = dot(-sunDir, n.xyz);
    incidenceAngleCos = -cosine;

    y = -sunDir.y;

    gl_Position = mul(worldViewProj, vertex);
    oUv = uv0;
    oNormal = -n.xyz;
}
