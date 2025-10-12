// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
    uniform SAMPLER2D(screenTexture, 0);
    uniform SAMPLER2D(depthTexture, 1);
    uniform SAMPLER2D(atmRelativeDepth, 2); // ~ changed from sampler1D
    
    uniform mat4 invViewProjMatrix;
    uniform vec4 worldCameraPos;

#if EXP_GROUND_FOG
    uniform float groundFogDensity;
    uniform float groundFogVerticalDecay;
    uniform float groundFogBaseLevel;
    uniform vec4 groundFogColour;
#endif // EXP_GROUND_FOG

#if SKY_DOME_HAZE
    uniform vec3 hazeColour;
    uniform vec3 sunDirection;
#endif // SKY_DOME_HAZE    
)

#ifdef EXP_GROUND_FOG

// Returns (exp(x) - 1) / x; avoiding division by 0.
// lim when x -> 0 is 1.
float expdiv(float x) {
    if (abs(x) < 0.0001) {
        return 1;
    } else {
        return (exp(x) - 1) / x;
    }
}

// Return fogging through a layer of fog which drops exponentially by height.
//
// Standard exp fog with constant density would return (1 - exp(-density * dist)).
// This function assumes a variable density vd = exp(-verticalDecay * h - baseLevel)
// Full computation is exp(density * dist / (h2 - h1) * int(h1, h2, exp(-verticalDecay * (h2 - h1)))).
//
// This will gracefully degrade to standard exp fog in verticalDecay is 0; without throwing NaNs.
float ExpGroundFog (
    float dist, float h1, float h2,
    float density, float verticalDecay, float baseLevel)
{
    float deltaH = (h2 - h1);
    return 1 - exp (-density * dist * exp(verticalDecay * (baseLevel - h1)) * expdiv(-verticalDecay * deltaH));
}

#endif // EXP_GROUND_FOG

#ifdef SKY_DOME_HAZE

float bias (float b, float x)
{
    return pow (x, log (b) / log (0.5));
}

vec4 sunlightInscatter
(
    vec4 sunColour,
    float absorption,
    float incidenceAngleCos,
    float sunlightScatteringFactor
)
{
    float scatteredSunlight = bias (sunlightScatteringFactor * 0.5, incidenceAngleCos);

    sunColour = sunColour * (1 - absorption) * vec4 (0.9, 0.5, 0.09, 1);
    
    return sunColour * scatteredSunlight;
}

float fogExp (float z, float density) {
    return 1 - clamp (pow (2.71828, -z * density), 0, 1);
}

vec4 CalcHaze
(
        vec3 worldPos,
        vec3 worldCamPos,
        vec3 hazeColour,
        vec3 sunDirection
)
{
    float haze = length (worldCamPos - worldPos);
    float incidenceAngleCos = dot (-sunDirection, normalize (worldPos - worldCamPos));
    float y = -sunDirection.y;

    vec4 sunColour = vec4 (3, 2.5, 1, 1);

    // Factor determining the amount of light lost due to absorption
    float atmLightAbsorptionFactor = 0.1; 
    float fogDensity = 15;

    haze = fogExp (haze * 0.005, atmLightAbsorptionFactor);

    // Haze amount calculation
    float invHazeHeight = 100;
    float hazeAbsorption = fogExp (pow (1 - y, invHazeHeight), fogDensity);

    if (incidenceAngleCos > 0) {
        // Factor determining the amount of scattering for the sun light
        float sunlightScatteringFactor = 0.1;
        // Factor determining the amount of sun light intensity lost due to scattering
        float sunlightScatteringLossFactor = 0.3;   

        vec4 sunlightInscatterColour = sunlightInscatter (
                sunColour, 
                clamp ((1 - tex1D (atmRelativeDepth, y).r) * hazeAbsorption, 0, 1), 
                clamp (incidenceAngleCos, 0, 1), 
                sunlightScatteringFactor) * (1 - sunlightScatteringLossFactor);
        hazeColour =
                hazeColour * (1 - sunlightInscatterColour.a) +
                sunlightInscatterColour.rgb * sunlightInscatterColour.a * haze;
    }

    return vec4(hazeColour.rgb, haze);
}

#endif // SKY_DOME_HAZE

MAIN_PARAMETERS
    IN(float2 screenPos, TEXCOORD0)
MAIN_DECLARATION
{
    vec4 inColor = tex2D(screenTexture, screenPos);
    float inDepth = tex2D(depthTexture, screenPos).r;

    // Build normalized device coords; after the perspective divide.
    //vec4 devicePos = vec4(1 - screenPos.x * 2, screenPos.y * 2 - 1, inDepth, 1);
    //vec4 devicePos = vec4(screenPos.x * 2 - 1, 1 - screenPos.y * 2, 2 * inDepth - 1, 1);
    vec4 devicePos = vec4(screenPos.x * 2 - 1, 1 - screenPos.y * 2, inDepth, 1);

    // Go back from device to world coordinates.
    vec4 worldPos = mul(invViewProjMatrix, devicePos);

    // Now undo the perspective divide and go back to "normal" space.
    worldPos /= worldPos.w;
    
    vec4 color = inColor;

#if DEBUG_DEPTH_RENDER
    //color = abs(vec4(inDepth, inDepth, inDepth, 1));
    color = worldPos * vec4(0.001, 0.01, 0.001, 1);
#endif // DEBUG_DEPTH_RENDER

#if EXP_GROUND_FOG
    // Ye olde ground fog.
    float h1 = worldCameraPos.y;
    float h2 = worldPos.y;
    float dist = length(worldCameraPos - worldPos);
    float fogFactor = ExpGroundFog(
            dist, h1, h2,
            groundFogDensity, groundFogVerticalDecay, groundFogBaseLevel);
    color = lerp(color, groundFogColour, fogFactor);
#endif // EXP_GROUND_FOG

#if SKY_DOME_HAZE
    vec4 hazeValue = CalcHaze (
            worldPos.xyz,
            worldCameraPos.xyz,
            hazeColour,
            sunDirection);
    color.rgb = lerp(color.rgb, hazeValue.rgb, hazeValue.a);
#endif // SKY_DOME_HAZE

    gl_FragColor = color;
}



