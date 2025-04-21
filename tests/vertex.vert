#version 330 core

// Input vertex attributes from VBO
// Matches the layout location used in glVertexAttribPointer
layout (location = 0) in vec3 aPos;   // Vertex Position
layout (location = 1) in vec3 aColor; // Vertex Color

// Uniforms set from C++ application
uniform mat4 model;      // Model matrix (transforms model space to world space)
uniform mat4 view;       // View matrix (positions the camera)
uniform mat4 projection; // Projection matrix (defines perspective/ortho view)

// Output variable to pass color to the fragment shader
// The name ('vertexColor') must match the 'in' variable in the fragment shader
out vec3 vertexColor;

void main()
{
    // Transform vertex position from model space to clip space
    // gl_Position is a required built-in output variable
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // Pass the input color directly to the fragment shader
    // It will be automatically interpolated across the triangle surface
    vertexColor = aColor;
}

