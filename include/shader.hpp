#pragma once

#include <utils.hpp>

#include <string>
#include <fstream>
#include <sstream>

class Shader {
    public:
        uint32_t id;

        Shader(const std::string vertex_filename, const std::string fragment_filename);
        ~Shader();

        void use();

        void set_bool();

    private:
        std::string load_shader_source(const std::string& filename);
        void check_compile_errors(uint32_t shader, std::string type);

};