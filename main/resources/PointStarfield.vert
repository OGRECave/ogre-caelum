// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

OGRE_NATIVE_GLSL_VERSION_DIRECTIVE

#include <OgreUnifiedShader.h>

OGRE_UNIFORMS(
uniform mat4 worldviewproj_matrix;

// These params are in clipspace; not pixels
uniform float mag_scale;
uniform float mag0_size;
uniform float min_size;
uniform float max_size;
uniform float render_target_flipping;

// width/height
uniform float aspect_ratio;
)

MAIN_PARAMETERS
IN(vec4 vertex, POSITION)
IN(vec3 uv0, TEXCOORD0)
OUT(vec2 oTexCoord, TEXCOORD0)
OUT(vec4 oColour, TEXCOORD1)
MAIN_DECLARATION
{
    gl_Position = mul(worldviewproj_matrix, vertex);
    oTexCoord = uv0.xy;

    float magnitude = uv0.z;
    float size = exp(mag_scale * magnitude) * mag0_size;

    // Fade below minSize.
    float fade = saturate(size / min_size);
    oColour = vec4(1.0, 1.0, 1.0, fade * fade);

    // Clamp size to range.
    size = clamp(size, min_size, max_size);

    // Splat the billboard on the screen.
    gl_Position.xy +=
            gl_Position.w *
            uv0.xy *
            vec2(size, size * aspect_ratio * render_target_flipping);
}
