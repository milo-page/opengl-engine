#version 330 core

in vec3 colour;
in vec3 normal;

out vec4 frag_colour;

void main() {
    frag_colour = vec4(colour, 1.0);
}