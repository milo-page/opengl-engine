#include <shader.hpp>

Shader::Shader(const std::string vertex_filename, const std::string fragment_filename) {
    std::string vertex_shader_source_str = load_shader_source(vertex_filename);
    std::string fragment_shader_source_str = load_shader_source(fragment_filename);
    const char* vertex_shader_source = vertex_shader_source_str.c_str();
    const char* fragment_shader_source = fragment_shader_source_str.c_str();

    // vertex shader
    uint32_t vertex_shader = gl::glCreateShader(gl::GL_VERTEX_SHADER);
    gl::glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    gl::glCompileShader(vertex_shader);
    check_compile_errors(vertex_shader, "VERTEX");

    // fragment shader
    uint32_t fragment_shader = gl::glCreateShader(gl::GL_FRAGMENT_SHADER);
    gl::glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    gl::glCompileShader(fragment_shader);
    check_compile_errors(fragment_shader, "FRAGMENT");

    // shader program
    id = gl::glCreateProgram();
    gl::glAttachShader(id, vertex_shader);
    gl::glAttachShader(id, fragment_shader);
    gl::glLinkProgram(id);
    check_compile_errors(id, "PROGRAM");

    // delete shaders after linking
    gl::glDeleteShader(vertex_shader);
    gl::glDeleteShader(fragment_shader);
}

Shader::~Shader() {
    gl::glDeleteProgram(id);
}

void Shader::use() {
    gl::glUseProgram(id);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
    gl::GLint uniform_location = gl::glGetUniformLocation(id, name.c_str());
    if (uniform_location == -1) {
        std::cerr << "Warning: uniform '" << name << "' doesn't exist or is not used." << std::endl;
        return;
    }
    
    gl::glUniformMatrix4fv(uniform_location, 1, gl::GL_FALSE, glm::value_ptr(value));
}

std::string Shader::load_shader_source(const std::string& filename) {
    std::ifstream file("../src/shaders/" + filename);
    if (!file.is_open()) {
        std::cerr << "Could not open shader file: " << filename << std::endl;
        return "";
    }

    std::ostringstream ss;
    ss << file.rdbuf();

    return ss.str();
}

void Shader::check_compile_errors(uint32_t shader, std::string type) {
    int success;
    char info_log[1024];
    if (type != "PROGRAM") {
        gl::glGetShaderiv(shader, gl::GL_COMPILE_STATUS, &success);
        if (!success) {
            gl::glGetShaderInfoLog(shader, 1024, NULL, info_log);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << info_log << std::endl;
        }
    } else {
        gl::glGetProgramiv(shader, gl::GL_LINK_STATUS, &success);
        if (!success) {
            gl::glGetProgramInfoLog(shader, 1024, NULL, info_log);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << info_log << std::endl;
        }
    }
}