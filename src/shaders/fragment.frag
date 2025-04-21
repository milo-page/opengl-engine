#version 330 core
out vec4 frag_colour;
in vec4 colour;

uniform vec4 colour_multiplier;

void main()
{
    frag_colour = colour_multiplier * colour;
}