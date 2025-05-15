#pragma once

#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#include <utils.hpp>
#include <shader.hpp>

class Mesh {
    public:
        struct Vertex {
            glm::vec3 position;
            glm::vec3 colour;
            glm::vec3 normal;
        };

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    
        Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        ~Mesh();

        void draw(Shader &shader);
    private:
        gl::GLuint VAO = 0;
        gl::GLuint VBO = 0;
        gl::GLuint EBO = 0;
        gl::GLsizei index_count = 0;

        void setup_mesh();
};