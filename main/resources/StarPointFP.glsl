// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
    uniform mat4 wvpMatrix;
)

MAIN_PARAMETERS
	IN(vec4 in_color, COLOR)
	IN(vec2 in_texcoord, TEXCOORD0)
MAIN_DECLARATION
{
    gl_FragColor = in_color;
    float sqlen = dot(in_texcoord, in_texcoord);

    // A gaussian bell of sorts.
    gl_FragColor.a *= 1.5 * exp(-(sqlen * 8));
}
