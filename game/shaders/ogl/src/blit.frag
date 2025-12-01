#version 450 core

//#extension GL_ARB_shading_language_include : enable

#include "include/constants.glsl"

//let's assume that OpenGL's origin is bottom-left(with depth [0;1])
//also we load images in flipped mode so we need to make -y for all textures in shaders
//layout(origin_upper_left) in vec4 gl_FragCoord;

layout(location = 0) out vec4 output_color;

layout(binding = 0) uniform sampler2D input_color_attachment_no_msaa;
layout(binding = 0) uniform sampler2DMS input_color_attachment_msaa;

layout(location = 0) uniform bool msaa_enabled;

void main()
{
    int samples = 1;

    if(msaa_enabled)
        samples = textureSamples(input_color_attachment_msaa);

    if(samples > 1)
    {
        vec4 color = vec4(0.0, 0.0, 0.0, 0.0);

        for(int i = 0; i < samples; i++)
            color +=
                texelFetch(input_color_attachment_msaa, ivec2(gl_FragCoord.x, gl_FragCoord.y), i);

        output_color = color / samples;
    }
    else
    {
        //ivec2 size = textureSize(input_color_attachment_no_msaa, 0);
        //output_color =
        //    texture(input_color_attachment_no_msaa,
        //            vec2(gl_FragCoord.x * (1.0 / size.x), gl_FragCoord.y * (1.0 / size.y)));

        output_color =
            texelFetch(input_color_attachment_no_msaa, ivec2(gl_FragCoord.x, gl_FragCoord.y), 0);
    }

    //if(gl_FragCoord.y < 400)
    //    output_color = vec4(1.0, 0.0, 0.0, 0.0);
    //else
    //    output_color = vec4(0.0, 1.0, 0.0, 0.0);

    output_color.a = 1.0; //for non-transparent images
}
