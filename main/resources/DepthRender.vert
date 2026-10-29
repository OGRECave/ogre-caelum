// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
uniform mat4 wvpMatrix;
)

MAIN_PARAMETERS
IN(vec4 vertex, POSITION)
#ifdef ALPHA_REJECTION
IN(vec2 uv0, TEXCOORD0)
OUT(vec2 oTexcoord, TEXCOORD0)
#endif
OUT(vec4 magic, TEXCOORD1)
MAIN_DECLARATION
{
    // Standard transform.
    gl_Position = mul(wvpMatrix, vertex);

    // Depth buffer is z/w.
    // Let the GPU lerp the components of outPos.
    magic = gl_Position;

#ifdef ALPHA_REJECTION
    oTexcoord = uv0;
#endif
}
