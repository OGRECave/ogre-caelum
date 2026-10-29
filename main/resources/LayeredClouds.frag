// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include <OgreUnifiedShader.h>

// Global cloud textures
SAMPLER2D(cloud_shape1, 0);
SAMPLER2D(cloud_shape2, 1);
SAMPLER2D(cloud_detail, 2);

OGRE_UNIFORMS(
uniform float cloudMassInvScale;
uniform float cloudDetailInvScale;
uniform vec2 cloudMassOffset;
uniform vec2 cloudDetailOffset;
uniform float cloudMassBlend;
uniform float cloudDetailBlend;

uniform float cloudCoverageThreshold;

uniform vec4 sunLightColour;
uniform vec4 sunSphereColour;
uniform vec4 fogColour;
uniform vec3 sunDirection;
uniform float cloudSharpness;
uniform float cloudThickness;
uniform f32vec3 camera_position;

uniform vec3 fadeDistMeasurementVector;
uniform float layerHeight;
uniform float cloudUVFactor;
uniform float heightRedFactor;

uniform float32_t nearFadeDist;
uniform float32_t farFadeDist;
)

// Get cloud layer intensity at a certain point.
float LayeredClouds_intensity(vec2 pos)
{
    // Calculate the base alpha
    vec2 finalMassOffset = cloudMassOffset + pos;
    float aCloud = mix(texture2D(cloud_shape1, finalMassOffset * cloudMassInvScale).r,
                       texture2D(cloud_shape2, finalMassOffset * cloudMassInvScale).r,
                       cloudMassBlend);
    float aDetail = texture2D(cloud_detail, (cloudDetailOffset + pos) * cloudDetailInvScale).r;
    aCloud = (aCloud + aDetail * cloudDetailBlend) / (1.0 + cloudDetailBlend);
    return max(0.0, aCloud - cloudCoverageThreshold);
}

vec4 OldCloudColor(vec2 uv, float sunGlow)
{
    // Get cloud intensity.
    float intensity = LayeredClouds_intensity(uv);

    // Opacity is exponential.
    float aCloud = saturate(exp(cloudSharpness * intensity) - 1.0);

    float shine = pow(saturate(sunGlow), 8.0) / 4.0;
    vec3 sunColour = sunLightColour.rgb * 1.5;
    vec3 cloudColour = fogColour.rgb * (1.0 - intensity / 3.0);
    float thickness = saturate(0.8 - exp(-cloudThickness * (intensity + 0.2 - shine)));

    return vec4(mix(sunColour, cloudColour, thickness), aCloud);
}

// Converts a color from RGB to YUV color space
// the rgb color is in [0,1] [0,1] [0,1] range
// the yuv color is in [0,1] [-0.436,0.436] [-0.615,0.615] range
vec3 YUVfromRGB(vec3 col)
{
    return vec3(dot(col, vec3(0.299, 0.587, 0.114)),
                dot(col, vec3(-0.14713, -0.28886, 0.436)),
                dot(col, vec3(0.615, -0.51499, -0.10001)));
}

vec3 RGBfromYUV(vec3 col)
{
    return vec3(dot(col, vec3(1.0, 0.0, 1.13983)),
                dot(col, vec3(1.0, -0.39465, -0.58060)),
                dot(col, vec3(1.0, 2.03211, 0.0)));
}

// Creates a color that has the intensity of col1 and the chrominance of col2
vec3 MagicColorMix(vec3 col1, vec3 col2)
{
    return saturate(RGBfromYUV(vec3(YUVfromRGB(col1).x, YUVfromRGB(col2).yz)));
}

MAIN_PARAMETERS
IN(vec2 oUv, TEXCOORD0)
IN(vec3 relPosition, TEXCOORD1)
IN(float sunGlow, TEXCOORD2)
IN(f32vec4 worldPosition, TEXCOORD3)
MAIN_DECLARATION
{
    vec4 oCol = OldCloudColor(oUv * cloudUVFactor, sunGlow);
    oCol.r += layerHeight / heightRedFactor;

    float32_t dist = length((worldPosition.xyz - camera_position) * fadeDistMeasurementVector);
    float aMod = 1.0;
    if (dist > nearFadeDist) {
        aMod = saturate((farFadeDist - dist) / (farFadeDist - nearFadeDist));
    }
    float alfa = oCol.a * aMod;

    vec3 cloudDir = normalize(
             vec3(worldPosition.x, layerHeight, worldPosition.y) - camera_position);
    float angleDiff = saturate(dot(cloudDir, normalize(sunDirection)));

    vec3 lCol = mix(oCol.rgb, MagicColorMix(oCol.rgb, sunSphereColour.rgb), angleDiff);
    oCol.rgb = mix(lCol, oCol.rgb, alfa);
    oCol.a = alfa;

    gl_FragColor = oCol;
}
