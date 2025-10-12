// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
        uniform mat4 worldViewProj;
        uniform vec4 camPos;
        uniform vec3 sunDirection;
)

MAIN_PARAMETERS
        IN(vec4 position, POSITION)
		IN(vec4 normal, NORMAL)

        OUT(float haze, TEXCOORD0)
        OUT(float2 sunlight, TEXCOORD1)
MAIN_DECLARATION
{
	sunDirection = normalize (sunDirection);
	gl_Position = mul(worldViewProj, position);
	haze = length (camPos - position);
	sunlight.x = dot (-sunDirection, normalize (position - camPos));
	sunlight.y = -sunDirection.y;
}