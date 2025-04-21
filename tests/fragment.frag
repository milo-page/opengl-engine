#version 330 core

// Input variable received from the vertex shader
// Name ('vertexColor') must match the 'out' variable in the vertex shader
// Value is interpolated from the vertex colors of the triangle vertices
in vec3 vertexColor;

// Output variable for the final color of the fragment (pixel)
out vec4 FragColor;

void main()
{
    // Set the final fragment color using the interpolated color
    // Set alpha to 1.0 for a solid color
    FragColor = vec4(vertexColor, 1.0);
}

