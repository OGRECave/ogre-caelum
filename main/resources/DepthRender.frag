// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include <OgreUnifiedShader.h>

#ifdef ALPHA_REJECTION
SAMPLER2D(mainTex, 0);
#endif

MAIN_PARAMETERS
#ifdef ALPHA_REJECTION
IN(vec2 oTexcoord, TEXCOORD0)
#endif
IN(vec4 magic, TEXCOORD1)
MAIN_DECLARATION
{
    float depth = magic.z / magic.w;
#ifdef ALPHA_REJECTION
    gl_FragColor = vec4(depth, depth, depth, texture2D(mainTex, oTexcoord).a);
#else
    gl_FragColor = vec4(depth, depth, depth, depth);
#endif
}
