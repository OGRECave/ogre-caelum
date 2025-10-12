// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
    uniform SAMPLER2D(scene, 0);
    uniform SAMPLER2D(samplerPrec, 1);

    uniform float intensity;
    uniform vec4 ambient_light_colour;

    // - - corner
    uniform vec4 corner1;
    // + - corner
    uniform vec4 corner2;
    // - + corner
    uniform vec4 corner3;
    // + + corner
    uniform vec4 corner4;

    // The x and y coordinal deviations for all 3 layers of precipitation
    uniform vec4 deltaX;
    uniform vec4 deltaY;

    uniform vec4 precColor;
)

// Cartesian to cylindrical coordinates
vec2 CylindricalCoordinates(vec4 dir) {
	float R = 0.5;
	vec2 res;
	//cubical root is used to counteract top/bottom circle effect
	dir *= R / pow(length(dir.xz), 0.33);
	res.y = -dir.y;
	res.x = -atan2(dir.z, dir.x);
	return res;
}

// Returns alpha value of a precipitation
// view_direction is the direction vector resulting from the eye direction,wind direction and possibly other factors
float Precipitation
	(
		vec2 cCoords,
		float intensity,
		vec2 delta
	) {
	cCoords -= delta;
	vec4 raincol = tex2D(samplerPrec, cCoords);
	return (raincol.g<intensity) ? (raincol.r) : 1;
}

MAIN_PARAMETERS
	IN(vec2 scr_pos, TEXCOORD0)
MAIN_DECLARATION
{
	vec4 eye = lerp (
		lerp(corner1, corner3, scr_pos.y),
		lerp(corner2, corner4, scr_pos.y),
		scr_pos.x ) ;
	
	vec4 scenecol = tex2D(scene, scr_pos);
	vec2 cCoords = CylindricalCoordinates(eye);
	float prec1 = Precipitation(cCoords, intensity/4, vec2(deltaX.x,deltaY.x));
	float prec2 = Precipitation(cCoords, intensity/4, vec2(deltaX.y,deltaY.y));
	float prec3 = Precipitation(cCoords, intensity/4, vec2(deltaX.z,deltaY.z));
	float prec = min( min (prec1, prec2), prec3);
	gl_FragColor = lerp(precColor, scenecol, prec );	
}

