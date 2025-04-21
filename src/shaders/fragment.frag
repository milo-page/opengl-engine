#version 330 core
out vec4 frag_colour;
in vec3 colour;

uniform vec3 colour_multiplier;

void main()
{
    frag_colour = vec4(colour_multiplier + colour, 1.0);
}