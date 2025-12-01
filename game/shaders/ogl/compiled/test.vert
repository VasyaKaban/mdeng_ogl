#version 450 core

struct QuadVertex
{
    vec2 position;
    vec2 texture_coordinate;
};

const QuadVertex QUAD_VERTICES[6] = {QuadVertex(vec2(-1.0, 1.0), vec2(0.0, 0.0)),
                                     QuadVertex(vec2(1.0, 1.0), vec2(1.0, 0.0)),
                                     QuadVertex(vec2(1.0, -1.0), vec2(1.0, 1.0)),
                                     QuadVertex(vec2(1.0, -1.0), vec2(1.0, 1.0)),
                                     QuadVertex(vec2(-1.0, -1.0), vec2(0.0, 1.0)),
                                     QuadVertex(vec2(-1.0, 1.0), vec2(0.0, 0.0))};

layout(location = 0) out vec2 out_texture_coordinates;

void main()
{
    gl_Position = vec4(QUAD_VERTICES[gl_VertexID].position, 0.0, 1.0);
    out_texture_coordinates = QUAD_VERTICES[gl_VertexID].texture_coordinate;
}