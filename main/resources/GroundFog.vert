// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
uniform mat4 worldViewProj;
uniform mat4 world;
)

MAIN_PARAMETERS
IN(vec4 vertex, POSITION)
OUT(f32vec4 worldPos, TEXCOORD0)
MAIN_DECLARATION
{
    gl_Position = mul(worldViewProj, vertex);
    worldPos = mul(world, vertex);
}
