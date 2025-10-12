// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
	uniform mat4 worldviewproj_matrix,

    // These params are in clipspace; not pixels
    uniform float mag_scale,
    uniform float mag0_size,
    uniform float min_size,
    uniform float max_size,
    uniform float render_target_flipping,

    // width/height
    uniform float aspect_ratio,
)

MAIN_PARAMETERS
    IN(vec4 in_position, POSITION)
	IN(vec3 in_texcoord, TEXCOORD0)
	
	OUT(float2 out_texcoord, TEXCOORD0)
	OUT(vec4 out_color, COLOR)
MAIN_DECLARATION
{
    vec4 in_color = vec4(1, 1, 1, 1);
    gl_Position = mul(worldviewproj_matrix, in_position);
    out_texcoord = in_texcoord.xy;

    float magnitude = in_texcoord.z;
    float size = exp(mag_scale * magnitude) * mag0_size;

    // Fade below minSize.
    float fade = saturate(size / min_size);
    out_color = vec4(in_color.rgb, fade * fade);

    // Clamp size to range.
    size = clamp(size, min_size, max_size);

    // Splat the billboard on the screen.
    gl_Position.xy +=
            gl_Position.w *
            in_texcoord.xy *
            float2(size, size * aspect_ratio * render_target_flipping);
}