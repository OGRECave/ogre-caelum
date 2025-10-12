// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
    uniform float lightAbsorption
    uniform mat4 worldViewProj
    uniform vec3 sunDirection
)

MAIN_PARAMETERS
    IN(vec4 position, POSITION)
    IN(vec4 normal : NORMAL)
    IN(float2 uv : TEXCOORD0,

    OUT(vec4 oCol , COLOR)
    OUT(vec2 oUv , TEXCOORD0)
    OUT(float incidenceAngleCos , TEXCOORD1)
    OUT(float y , TEXCOORD2)
    OUT(vec3 oNormal , TEXCOORD3)
MAIN_DECLARATION
{
	sunDirection = normalize (sunDirection);
	normal = normalize (normal);
	float cosine = dot (-sunDirection, normal);
	incidenceAngleCos = -cosine;

	y = -sunDirection.y;

	gl_Position = mul (worldViewProj, position);
	oCol = vec4 (1, 1, 1, 1);
	oUv = uv;
	oNormal = -normal.xyz;
}