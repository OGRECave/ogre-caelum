// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
		uniform mat4 worldViewProj;
		uniform mat4 worldMatrix;
		uniform vec3 sunDirection;
)

MAIN_PARAMETERS
		IN(vec4 position, POSITION)
		IN(vec2 uv, TEXCOORD0)
		
		OUT(vec2 oUv, TEXCOORD0)
		OUT(vec3 relPosition, TEXCOORD1)
		OUT(float sunGlow, TEXCOORD2)
		OUT(vec4 worldPosition, TEXCOORD3)
MAIN_DECLARATION
{

	gl_Position = mul(worldViewProj, position);
	worldPosition = mul(worldMatrix, position);
	oUv = uv;

    // This is the relative position, or view direction.
	relPosition = normalize (position.xyz);

    // Calculate the angle between the direction of the sun and the current
    // view direction. This we call "glow" and ranges from 1 next to the sun
    // to -1 in the opposite direction.
	sunGlow = dot (relPosition, normalize (-sunDirection));
}