#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_colour;

out vec3 colour;

// uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    //gl_Position = transform * vec4(a_pos, 1.0);
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    colour = a_colour;
}