// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE

#include <OgreUnifiedShader.h>

MAIN_PARAMETERS
IN(vec2 oTexCoord, TEXCOORD0)
IN(vec4 oColour, TEXCOORD1)
MAIN_DECLARATION
{
    vec4 colour = oColour;
    float sqlen = dot(oTexCoord, oTexCoord);

    // A gaussian bell of sorts.
    colour.a *= 1.5 * exp(-(sqlen * 8.0));

    gl_FragColor = colour;
}
