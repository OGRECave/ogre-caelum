// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE

#include <OgreUnifiedShader.h>

SAMPLER2D(moonDisc, 0);

OGRE_UNIFORMS(
uniform float phase;
)

// Faint reflected light from the Earth on the night side of the moon.
#define EARTHSHINE 0.03

MAIN_PARAMETERS
IN(vec2 oTexCoord, TEXCOORD0)
IN(vec4 oColour, TEXCOORD1)
MAIN_DECLARATION
{
    vec4 moonDiscColour = texture2D(moonDisc, oTexCoord);

    // Interpret the billboard coordinates as points on a sphere. z points
    // towards the observer, so the normal runs from 1 in the centre of the
    // disc to 0 at its edge.
    vec2 discPos = oTexCoord * 2.0 - 1.0;
    float z = sqrt(max(0.0, 1.0 - dot(discPos, discPos)));
    vec3 normal = vec3(discPos, z);

    // Phase 0 is a full moon and 0.5 a new moon; the light comes from the
    // direction matching that phase in the plane of the billboard.
    float sunAngle = 6.2831853 * phase;
    vec3 lightDirection = vec3(-sin(sunAngle), 0.0, cos(sunAngle));

    // Soft day/night boundary (penumbra) instead of a hard-edged terminator.
    float day = smoothstep(-0.03, 0.03, dot(normal, lightDirection));

    // The moon scatters light closer to a Lommel-Seeliger law than to a
    // Lambertian one; flatten the limb so a full moon does not look like a
    // shaded ball.
    float limb = mix(0.85, 1.0, sqrt(z));

    vec3 colour = moonDiscColour.rgb * oColour.rgb *
            (day * limb + (1.0 - day) * EARTHSHINE);

    // The disc stays opaque (alpha from the texture) so that the moon hides
    // the stars and the sky dome behind it, on both the lit and the night side.
    gl_FragColor = vec4(colour, moonDiscColour.a);
}
