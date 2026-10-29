// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

// Helper functions shared by the Caelum shaders.

#include <OgreUnifiedShader.h>

#if !defined(OGRE_HLSL) && !defined(OGRE_CG)
#define atan2 atan
#endif

// Returns (exp(x) - 1) / x; avoiding division by 0.
// lim when x -> 0 is 1.
float32_t expdiv(float32_t x)
{
    if (abs(x) < 0.0001) {
        return 1.0;
    } else {
        return (exp(x) - 1.0) / x;
    }
}

// Return fogging through a layer of fog which drops exponentially by height.
//
// Standard exp fog with constant density would return (1 - exp(-density * dist)).
// This function assumes a variable density vd = exp(-verticalDecay * h - baseLevel)
// Full computation is exp(density * dist / (h2 - h1) * int(h1, h2, exp(-verticalDecay * (h2 - h1)))).
//
// This will gracefully degrade to standard exp fog in verticalDecay is 0; without throwing NaNs.
float ExpGroundFog(
    float32_t dist, float32_t h1, float32_t h2,
    float density, float verticalDecay, float baseLevel)
{
    float32_t deltaH = (h2 - h1);
    return 1.0 - exp(-density * dist * exp(verticalDecay * (baseLevel - h1)) * expdiv(-verticalDecay * deltaH));
}

// Just like ExpGroundFog with h2 = positive infinity
// When h2 == negative infinity the value is always +1.
float ExpGroundFogInf(
    float32_t invSinView, float32_t h1,
    float density, float verticalDecay, float baseLevel)
{
    return 1.0 - exp(-density * invSinView * exp(verticalDecay * (baseLevel - h1)) * (1.0 / verticalDecay));
}

float bias(float b, float x)
{
    return pow(x, log(b) / log(0.5));
}

vec4 sunlightInscatter(
    vec4 sunColour,
    float absorption,
    float incidenceAngleCos,
    float sunlightScatteringFactor)
{
    float scatteredSunlight = bias(sunlightScatteringFactor * 0.5, incidenceAngleCos);

    sunColour = sunColour * (1.0 - absorption) * vec4(0.9, 0.5, 0.09, 1.0);

    return sunColour * scatteredSunlight;
}

float fogExp(float32_t z, float density)
{
    return 1.0 - clamp(pow(2.71828, -z * density), 0.0, 1.0);
}
