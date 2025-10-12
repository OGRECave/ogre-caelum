// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
#include <OgreUnifiedShader.h>

MAIN_PARAMETERS
    IN(vec4 magic, TEXCOORD0)
MAIN_DECLARATION
{
    gl_FragColor = vec4(magic.z / magic.w);
    //output = vec4(magic.xy / magic.w, 1, 1);
}

