// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE

#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
uniform mat4 worldViewProj;
)

MAIN_PARAMETERS
IN(vec4 vertex, POSITION)
OUT(vec3 relPosition, TEXCOORD0)
MAIN_DECLARATION
{
    gl_Position = mul(worldViewProj, vertex);
    relPosition = normalize(vertex.xyz);
}
