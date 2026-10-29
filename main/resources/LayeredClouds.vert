// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE

#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
uniform mat4 worldViewProj;
uniform mat4 worldMatrix;
)

MAIN_PARAMETERS
IN(vec4 vertex, POSITION)
IN(vec2 uv0, TEXCOORD0)
OUT(vec2 oUv, TEXCOORD0)
OUT(f32vec4 worldPosition, TEXCOORD1)
MAIN_DECLARATION
{
    gl_Position = mul(worldViewProj, vertex);
    worldPosition = mul(worldMatrix, vertex);
    oUv = uv0;
}
