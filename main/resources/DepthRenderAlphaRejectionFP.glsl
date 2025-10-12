// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
    uniform SAMPLER2D(mainTex, 0);
)

MAIN_PARAMETERS
    IN(vec4 texcoord, TEXCOORD0)
    IN(vec4 magic, TEXCOORD1)
)
MAIN_DECLARATION
{
    vec4 texvalue = tex2D(mainTex, texcoord.xy);
//    texvalue.a = sin(100 * texcoord.x) + sin(100 * texcoord.y);
    gl_FragColor = vec4(vec3(magic.z / magic.w), texvalue.a);
}