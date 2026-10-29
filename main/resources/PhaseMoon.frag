// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include <OgreUnifiedShader.h>

SAMPLER2D(moonDisc, 0);

OGRE_UNIFORMS(
uniform float phase;
)

// Get how much of a certain point on the moon is seen (or not) because of the phase.
// uv is the rect position on moon; as seen from the earth.
// phase ranges from 0 (full moon) to 1 (again fool moon)
float MoonPhaseFactor(vec2 uv, float phase)
{
    // 1. In phase interval [0..1/2) day-to-night terminator appeared on the right side of the moon and moves to the left by cosine law
    // 2. In phase interval [1/2..1) night-to-day terminator appeared on the right side of the moon and moves to the left by cosine law
    // but if we swapped for simplicity left and right sides of the moon for the second interval than
    // 2' In phase interval [1/2..1) night-to-day terminator appeared on the left side of the moon and moves to the right by cosine law
    // therefore in such coord system terminator would move from right to left and to the right again, and picture would be like this:
    // Moon => (day | night)

    float cY = uv.y - 0.5;                             // signed
    float cX = sqrt(0.25 - cY * cY);                   // positive, half of disk chord

    float termX = cX * cos((2.0 * 3.1416) * phase);    // determine terminator position in our tweaked coord system
    float refX = (uv.x - 0.5) * sign(0.5 - phase);     // reverse X axis for phase interval [1/2..1)

    return 0.5 * sign(termX - refX) + 0.5;             // day if refX < termX
}

MAIN_PARAMETERS
IN(vec2 oTexCoord, TEXCOORD0)
MAIN_DECLARATION
{
    vec4 outcol = texture2D(moonDisc, oTexCoord);
    float alpha = MoonPhaseFactor(oTexCoord, phase);

    // Get luminance from the texture.
    float lum = dot(outcol.rgb, vec3(0.3333, 0.3333, 0.3333));
    //float lum = dot(outcol.rgb, vec3(0.3, 0.59, 0.11));
    outcol.a = min(outcol.a, lum * alpha);
    outcol.rgb /= lum;

    gl_FragColor = outcol;
}
