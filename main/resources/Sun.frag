// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE

#include <OgreUnifiedShader.h>

SAMPLER2D(sunDisc, 0);

MAIN_PARAMETERS
IN(vec2 oTexCoord, TEXCOORD0)
IN(vec4 oColour, TEXCOORD1)
MAIN_DECLARATION
{
    vec4 sunDiscColour = texture2D(sunDisc, oTexCoord);

    // Mild limb darkening: the centre of the disc is slightly brighter than
    // the rim. The texture alpha masks the disc, so the outer transparent
    // region is not affected.
    vec2 discPos = oTexCoord * 2.0 - 1.0;
    float mu = sqrt(max(0.0, 1.0 - dot(discPos, discPos)));
    float limb = mix(0.8, 1.0, sqrt(mu));

    // The material uses `scene_blend src_alpha one`, i.e. the contribution is
    // scaled by the output alpha. This keeps the fully transparent texels of
    // sun_disc.png (which carry white RGB) invisible.
    gl_FragColor = vec4(sunDiscColour.rgb * oColour.rgb * limb, sunDiscColour.a);
}
