#version 330 core

in vec4 colour;
in vec3 normal;

out vec4 frag_colour;

uniform vec3 light_direction = normalize(vec3(-0.5, -1.0, -0.3));

void main()
{
    float diff = max(dot(normalize(normal), -light_direction), 0.15);
    frag_colour = vec4(colour.rgb * diff, colour.a);
}