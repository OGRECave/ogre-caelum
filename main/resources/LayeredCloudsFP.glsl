// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
        // Global cloud textures
        uniform SAMPLER2D(cloud_shape1, 0);
        uniform SAMPLER2D(cloud_shape2, 1);
        uniform SAMPLER2D(cloud_detail, 2);

        uniform float   cloudMassInvScale;
        uniform float   cloudDetailInvScale;
        uniform vec2  cloudMassOffset;
        uniform vec2  cloudDetailOffset;
        uniform float   cloudMassBlend;
        uniform float   cloudDetailBlend;

        uniform float   cloudCoverageThreshold;

        uniform vec4  sunLightColour;
        uniform vec4  sunSphereColour;
        uniform vec4  fogColour;
        uniform vec4	sunDirection;
        uniform float   cloudSharpness;
        uniform float   cloudThickness;
        uniform vec3	camera_position;

        uniform vec3	fadeDistMeasurementVector;
        uniform float	layerHeight;
        uniform float	cloudUVFactor;
        uniform float	heightRedFactor;

        uniform float   nearFadeDist;
        uniform float   farFadeDist;
)

// Get cloud layer intensity at a certain point.
float LayeredClouds_intensity
(
        in vec2   pos,
        float       cloudMassInvScale,
        float       cloudDetailInvScale,
        vec2      cloudMassOffset,
        vec2      cloudDetailOffset,
        float       cloudMassBlend,
        float       cloudDetailBlend,
        float       cloudCoverageThreshold
)
{
	// Calculate the base alpha
	vec2 finalMassOffset = cloudMassOffset + pos;
	float aCloud = lerp(tex2D(cloud_shape1, finalMassOffset * cloudMassInvScale).r,
						tex2D(cloud_shape2, finalMassOffset * cloudMassInvScale).r, 
						cloudMassBlend);
	float aDetail = tex2D(cloud_detail, (cloudDetailOffset + pos) * cloudDetailInvScale).r;
	aCloud = (aCloud + aDetail * cloudDetailBlend) / (1 + cloudDetailBlend);
	return max(0, aCloud - cloudCoverageThreshold);
}

vec4 OldCloudColor
(
		vec2       uv,
		vec3       relPosition,
		float        sunGlow,

        float   cloudMassInvScale,
        float   cloudDetailInvScale,
        vec2  cloudMassOffset,
        vec2  cloudDetailOffset,
        float   cloudMassBlend,
        float   cloudDetailBlend,

        float   cloudCoverageThreshold,

        vec4  sunColour,
        vec4  fogColour,
        float   cloudSharpness,
        float   cloudThickness

) {
    // Initialize output.
	vec4 oCol = vec4(1, 1, 1, 0);
	
	// Get cloud intensity.
	float intensity = LayeredClouds_intensity
    (
            uv,
            cloudMassInvScale,
            cloudDetailInvScale,
            cloudMassOffset,
            cloudDetailOffset,
            cloudMassBlend,
            cloudDetailBlend,
            cloudCoverageThreshold
    );

	// Opacity is exponential.
	float aCloud = saturate(exp(cloudSharpness * intensity) - 1);

	float shine = pow(saturate(sunGlow), 8) / 4;
	sunColour.rgb *= 1.5;
	vec3 cloudColour = fogColour.rgb * (1 - intensity / 3);
	float thickness = saturate(0.8 - exp(-cloudThickness * (intensity + 0.2 - shine)));

	oCol.rgb = lerp(sunColour.rgb, cloudColour.rgb, thickness);
	oCol.a = aCloud;	
	
	return oCol;
}

//Converts a color from RGB to YUV color space
//the rgb color is in [0,1] [0,1] [0,1] range
//the yuv color is in [0,1] [-0.436,0.436] [-0.615,0.615] range
vec3 YUVfromRGB(vec3 col)
{
    return vec3(dot(col, vec3(0.299,0.587,0.114)),
				  dot(col, vec3(-0.14713,-0.28886,0.436)),
				  dot(col, vec3(0.615,-0.51499,-0.10001)));
}

vec3 RGBfromYUV(vec3 col)
{
    return vec3(dot(col,vec3(1,0,1.13983)),
				  dot(col,vec3(1,-0.39465,-0.58060)),
				  dot(col,vec3(1,2.03211,0)));
}

// Creates a color that has the intensity of col1 and the chrominance of col2
vec3 MagicColorMix(vec3 col1, vec3 col2)
{
    return saturate(RGBfromYUV(vec3(YUVfromRGB(col1).x, YUVfromRGB(col2).yz)));
}

MAIN_PARAMETERS
		IN(vec2       uv, TEXCOORD0)
		IN(vec3       relPosition, TEXCOORD1)
		IN(float        sunGlow, TEXCOORD2)
		IN(vec4 worldPosition, TEXCOORD3)		
MAIN_DECLARATION
{
	uv *= cloudUVFactor;

    gl_FragColor = OldCloudColor(
            uv, relPosition, sunGlow,
		    cloudMassInvScale, cloudDetailInvScale,
            cloudMassOffset, cloudDetailOffset,
            cloudMassBlend, cloudDetailBlend,
            cloudCoverageThreshold,
		    sunLightColour,
            fogColour,
            cloudSharpness,
            cloudThickness);
	gl_FragColor.r += layerHeight / heightRedFactor;
	
	//float dist = distance(worldPosition.xyz, camera_position.xyz);
	float dist = length((worldPosition - camera_position) * fadeDistMeasurementVector);
	float aMod = 1;	
	if (dist > nearFadeDist) {
        aMod = saturate(lerp(0, 1, (farFadeDist - dist) / (farFadeDist - nearFadeDist)));
    }
    float alfa = gl_FragColor.a * aMod;
	
	vec3 cloudDir = normalize(
	         vec3(worldPosition.x, layerHeight, worldPosition.y) - camera_position);
	float angleDiff = saturate(dot(cloudDir, normalize(sunDirection.xyz)));
	
	vec3 lCol = lerp(gl_FragColor.rgb, MagicColorMix(gl_FragColor.rgb, sunSphereColour.rgb), angleDiff);
	gl_FragColor.rgb = lerp(lCol, gl_FragColor.rgb, alfa);
	gl_FragColor.a = alfa;
}
