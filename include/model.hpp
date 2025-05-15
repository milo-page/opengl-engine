#pragma once

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <utils.hpp>
#include <shader.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec4 colour;
    glm::vec3 normal;
};

class Mesh {
    public:

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

class Model {
    public:
        Model(const std::string &path);
        void draw(Shader &shader);

    private:
        std::vector<Mesh> meshes;

        void load_model(const std::string &path);
        void process_node(aiNode *node, const aiScene *scene);
        Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
};