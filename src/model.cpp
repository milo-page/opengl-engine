#include <model.hpp>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {

    this->vertices = vertices;
    this->indices = indices;

    setup_mesh();
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

void Mesh::draw(Shader &shader) {
    gl::glBindVertexArray(VAO);
    gl::glDrawElements(gl::GL_TRIANGLES, indices.size(), gl::GL_UNSIGNED_INT, 0);
    gl::glBindVertexArray(0);
}

void Mesh::setup_mesh() {

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

Model::Model(const std::string &path) {
    load_model(path);
}

void Model::draw(Shader &shader) {
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].draw(shader);
}

void Model::load_model(const std::string &path) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);	
	
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }

    process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode *node, const aiScene *scene)
{
    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(process_mesh(mesh, scene));			
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene);
    }
}

Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    glm::vec4 mesh_colour = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

    bool has_vertex_colours = mesh->HasVertexColors(0);

    if (!has_vertex_colours && mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiColor4D diffuse_colour;
        if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_colour)) {
            mesh_colour = glm::vec4(diffuse_colour.r, diffuse_colour.g, diffuse_colour.b, diffuse_colour.a);
        }
    }

    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {

        Vertex vertex;
        glm::vec3 temp_vector; 

        temp_vector.x = mesh->mVertices[i].x;
        temp_vector.y = mesh->mVertices[i].y;
        temp_vector.z = mesh->mVertices[i].z;
        vertex.position = temp_vector;

        if (mesh->HasNormals()) {
            temp_vector.x = mesh->mNormals[i].x;
            temp_vector.y = mesh->mNormals[i].y;
            temp_vector.z = mesh->mNormals[i].z;
            vertex.normal = temp_vector;
        } else {
            vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        if (has_vertex_colours && mesh->mColors[0] != nullptr) {
            vertex.colour.r = mesh->mColors[0][i].r;
            vertex.colour.g = mesh->mColors[0][i].g;
            vertex.colour.b = mesh->mColors[0][i].b;
            vertex.colour.a = mesh->mColors[0][i].a;
        } else {
            vertex.colour = mesh_colour;
        }
        
        vertices.push_back(vertex);
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }     
    }

    return Mesh(vertices, indices); 
}
