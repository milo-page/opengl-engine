#include <mesh.hpp>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {

    // generate objects
    gl::glGenVertexArrays(1, &VAO);
    gl::glGenBuffers(1, &VBO);
    gl::glGenBuffers(1, &EBO);

    gl::glBindVertexArray(VAO);

    gl::glBindBuffer(gl::GL_ARRAY_BUFFER, VBO);
    gl::glBufferData(gl::GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), gl::GL_STATIC_DRAW);

    gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, EBO);
    gl::glBufferData(gl::GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), gl::GL_STATIC_DRAW);

    // attributes
    gl::glEnableVertexAttribArray(0);
    gl::glVertexAttribPointer(0, 3, gl::GL_FLOAT, gl::GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    gl::glEnableVertexAttribArray(1);
    gl::glVertexAttribPointer(1, 3, gl::GL_FLOAT, gl::GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, colour));

    gl::glEnableVertexAttribArray(2);
    gl::glVertexAttribPointer(2, 3, gl::GL_FLOAT, gl::GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

   // clean up
   gl::glBindVertexArray(0);
   gl::glBindBuffer(gl::GL_ARRAY_BUFFER, 0);
   gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, 0);

}

Mesh::~Mesh() {
    if (VAO != 0) {
        gl::glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO != 0) {
        gl::glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (EBO != 0) {
        gl::glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
}