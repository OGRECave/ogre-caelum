// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE

// Fixed function does not always work.
// This is a the minimal compositor VP required.

#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
uniform mat4 worldviewproj_matrix;
)

MAIN_PARAMETERS
IN(vec4 vertex, POSITION)
OUT(vec2 oUv0, TEXCOORD0)
MAIN_DECLARATION
{
    // Use standard transform.
    gl_Position = mul(worldviewproj_matrix, vertex);

    // Convert to image-space
    vec2 inPos = sign(vertex.xy);
    oUv0 = (vec2(inPos.x, -inPos.y) + 1.0) * 0.5;
}
