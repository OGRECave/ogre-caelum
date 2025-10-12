// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
    uniform mat4 worldviewproj_matrix;
)

MAIN_PARAMETERS
    IN(vec4 iPosition, POSITION)
    IN(vec2 iTexCoord, TEXCOORD0)
    
    OUT(vec2 oTexCoord, TEXCOORD0)
MAIN_DECLARATION
{
    gl_Position = mul(worldviewproj_matrix, iPosition);
    oTexCoord = iTexCoord;
}