// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
    uniform float phase;
    uniform SAMPLER2D(moonDisc, 0);
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
    IN(vec2 uv, TEXCOORD0)
MAIN_DECLARATION
{
    gl_FragColor = tex2D(moonDisc, uv);
    float alpha = MoonPhaseFactor(uv, phase);

    // Get luminance from the texture.
    float lum = dot(gl_FragColor.rgb, vec3(0.3333, 0.3333, 0.3333));
    //float lum = dot(gl_FragColor.rgb, vec3(0.3, 0.59, 0.11));
    gl_FragColor.a = min(gl_FragColor.a, lum * alpha);
    gl_FragColor.rgb /= lum;
}