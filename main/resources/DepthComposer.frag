// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE

#include "CaelumCommon.h"

SAMPLER2D(screenTexture, 0);
SAMPLER2D(depthTexture, 1);
#ifdef SKY_DOME_HAZE
SAMPLER2D(atmRelativeDepth, 2);
#endif

OGRE_UNIFORMS_BEGIN
#ifdef SKY_DOME_HAZE
uniform vec3 hazeColour;
uniform vec3 sunDirection;
#endif // SKY_DOME_HAZE

uniform mat4 invViewProjMatrix;
uniform f32vec4 worldCameraPos;

#ifdef EXP_GROUND_FOG
uniform float groundFogDensity;
uniform float groundFogVerticalDecay;
uniform float groundFogBaseLevel;
uniform vec4 groundFogColour;
#endif // EXP_GROUND_FOG
OGRE_UNIFORMS_END

#ifdef SKY_DOME_HAZE
vec4 CalcHaze(
        f32vec3 worldPos,
        f32vec3 worldCamPos,
        vec3 hazeColour,
        vec3 sunDirection)
{
    float32_t haze = length(worldCamPos - worldPos);
    float incidenceAngleCos = dot(-sunDirection, normalize(worldPos - worldCamPos));
    float y = -sunDirection.y;

    vec4 sunColour = vec4(3.0, 2.5, 1.0, 1.0);

    // Factor determining the amount of light lost due to absorption
    float atmLightAbsorptionFactor = 0.1;
    float fogDensity = 15.0;

    haze = fogExp(haze * 0.005, atmLightAbsorptionFactor);

    // Haze amount calculation
    float invHazeHeight = 100.0;
    float hazeAbsorption = fogExp(pow(1.0 - y, invHazeHeight), fogDensity);

    if (incidenceAngleCos > 0.0) {
        // Factor determining the amount of scattering for the sun light
        float sunlightScatteringFactor = 0.1;
        // Factor determining the amount of sun light intensity lost due to scattering
        float sunlightScatteringLossFactor = 0.3;

        vec4 sunlightInscatterColour = sunlightInscatter(
                sunColour,
                clamp((1.0 - texture2D(atmRelativeDepth, vec2(y, 0.5)).r) * hazeAbsorption, 0.0, 1.0),
                clamp(incidenceAngleCos, 0.0, 1.0),
                sunlightScatteringFactor) * (1.0 - sunlightScatteringLossFactor);
        hazeColour =
                hazeColour * (1.0 - sunlightInscatterColour.a) +
                sunlightInscatterColour.rgb * sunlightInscatterColour.a * haze;
    }

    return vec4(hazeColour, haze);
}
#endif // SKY_DOME_HAZE

MAIN_PARAMETERS
IN(vec2 oUv0, TEXCOORD0)
MAIN_DECLARATION
{
    vec4 inColor = texture2D(screenTexture, oUv0);
    float32_t inDepth = texture2D(depthTexture, oUv0).r;
#if !defined(OGRE_HLSL) && !defined(OGRE_REVERSED_Z) && !defined(VULKAN)
    // OpenGL maps the [-1, 1] device depth to the [0, 1] depth buffer
    inDepth = inDepth * 2.0 - 1.0;
#endif

    // Build normalized device coords; after the perspective divide.
    f32vec4 devicePos = vec4(oUv0.x * 2.0 - 1.0, 1.0 - oUv0.y * 2.0, inDepth, 1.0);

    // Go back from device to world coordinates.
    f32vec4 worldPos = mul(invViewProjMatrix, devicePos);

    // Now undo the perspective divide and go back to "normal" space.
    if (worldPos.w > 1e-6) {
        worldPos /= worldPos.w;
    } else {
        // The far plane is at infinity; pick a point far away along the view ray
        f32vec4 rayPos = mul(invViewProjMatrix, vec4(devicePos.xy, 0.0, 1.0));
        f32vec3 rayDir = normalize(rayPos.xyz / rayPos.w - worldCameraPos.xyz);
        worldPos = vec4(worldCameraPos.xyz + rayDir * 1e6, 1.0);
    }

    vec4 color = inColor;

#ifdef DEBUG_DEPTH_RENDER
    //color = abs(vec4(inDepth, inDepth, inDepth, 1.0));
    color = worldPos * vec4(0.001, 0.01, 0.001, 1.0);
#endif // DEBUG_DEPTH_RENDER

#ifdef EXP_GROUND_FOG
    // Ye olde ground fog.
    float32_t h1 = worldCameraPos.y;
    float32_t h2 = worldPos.y;
    float32_t dist = length(worldCameraPos.xyz - worldPos.xyz);
    float fogFactor = ExpGroundFog(
            dist, h1, h2,
            groundFogDensity, groundFogVerticalDecay, groundFogBaseLevel);
    color = mix(color, groundFogColour, fogFactor);
#endif // EXP_GROUND_FOG

#ifdef SKY_DOME_HAZE
    vec4 hazeValue = CalcHaze(
            worldPos.xyz,
            worldCameraPos.xyz,
            hazeColour,
            sunDirection);
    color.rgb = mix(color.rgb, hazeValue.rgb, hazeValue.a);
#endif // SKY_DOME_HAZE

    gl_FragColor = color;
}
