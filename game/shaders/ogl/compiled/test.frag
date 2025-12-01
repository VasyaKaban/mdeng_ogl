#version 450 core

layout(location = 0) in vec2 in_texture_coordinates;

layout(location = 0) out vec4 output_color;

layout(binding = 0) uniform sampler2D input_tetxure;

void main()
{
    output_color = texture(input_tetxure, in_texture_coordinates);
}
