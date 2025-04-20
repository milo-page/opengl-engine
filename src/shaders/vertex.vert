#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_colour;

out vec3 colour;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(a_pos, 3.0);
    colour = a_colour;
}