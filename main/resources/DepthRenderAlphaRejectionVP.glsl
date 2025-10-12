// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
    uniform mat4 wvpMatrix
)

MAIN_PARAMETERS
    IN(vec4 inPos, POSITION)
    IN(vec4 inTexcoord, TEXCOORD0)

    OUT(vec4 outTexcoord, TEXCOORD0)
    OUT(vec4 magic, TEXCOORD1)
MAIN_DECLARATION
{
    // Standard transform.
    gl_Position = mul(wvpMatrix, inPos);

    // Depth buffer is z/w.
    // Let the GPU lerp the components of gl_Position.
    magic = gl_Position;

    outTexcoord = inTexcoord;
}

