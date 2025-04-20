#version 330 core
out vec4 frag_colour;
in vec3 colour;

uniform vec4 colour_multiplier;

void main()
{
    frag_colour = vec4(colour.x * colour_multiplier.x, colour.y * colour_multiplier.y, colour.z * colour_multiplier.z, 1.0);
}