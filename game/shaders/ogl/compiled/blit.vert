#version 450 core

const vec2 QUAD[6] = {vec2(-1.0, 1.0),
                      vec2(-1.0, -1.0),
                      vec2(1.0, -1.0),

                      vec2(1.0, -1.0),
                      vec2(1.0, 1.0),
                      vec2(-1.0, 1.0)};

void main()
{
    gl_Position = vec4(QUAD[gl_VertexID], 0.0, 1.0);
}