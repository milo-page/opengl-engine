#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_colour;

out vec3 colour;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    colour = a_colour;
    normal = mat3(transpose(inverse(model))) * a_normal;
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
}