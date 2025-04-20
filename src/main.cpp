#include <iostream>
#include <string>
#include <SDL3/SDL.h>
#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

using namespace gl;

// set the callback so that it will resize
void framebuffer_size_callback(SDL_Window* window, int width, int height) {
    glViewport(0, 0, width, height);
}

std::string load_shader(const std::string& filename) {
    std::ifstream file("../src/shaders/" + filename);
    if (!file.is_open()) {
        std::cerr << "Could not open shader file: " << filename << std::endl;
        return "";
    }

    std::string shader;
    std::string line;
    while (std::getline(file, line)) {
        shader += line + '\n';
    }

    return shader;
}

void check_shader_compilation (uint32_t shader) {
    int  success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << info_log << std::endl;
    }
}
void check_program_linking(uint32_t program) {
    int  success;
    char info_log[512];

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, info_log);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL initialized" << '\n';
    } else {
        std::cerr << "SDL failed to initialize: " << SDL_GetError() << '\n';
    }

    // window stuff
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window* window = nullptr;
    int window_width = 1000;
    int window_height = 1000;
    const std::string title = "OpenGL Windows Test";

    window = SDL_CreateWindow(
        title.c_str(),
        window_width, window_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE //| SDL_WINDOW_FULLSCREEN
    );
    if (!window) {
        throw std::runtime_error("Window creation failed: " + std::string(SDL_GetError()));
    }

    // opengl initialization
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        throw std::runtime_error("OpenGL context creation failed: " + std::string(SDL_GetError()));
    }

    glbinding::Binding::initialize();

    std::cout << "OpenGL initialized" << '\n';
    std::cout << "Vendor: "   << glGetString(GL_VENDOR)   << '\n';
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';
    std::cout << "Version: "  << glGetString(GL_VERSION)  << '\n';

    // Enable VSync
    SDL_GL_SetSwapInterval(1);

    SDL_Event event;
    bool running_flag = true;

    glViewport(0, 0, window_width, window_height);

    // vertex shader
    std::string vertex_shader_code = load_shader("vertex.vert");
    const char* vertex_shader_source = vertex_shader_code.c_str();
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string fragment_shader_code = load_shader("fragment.frag");
    const char* fragment_shader_source = fragment_shader_code.c_str();
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    // check for shader compile errors
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    // check for linking errors
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    float one_two = 1 / glm::sqrt(2);

    float vertices[] = {

        0.0f, 1.0f, 0.0f,           1.0f, 0.0f, 0.0f,
        -one_two, 0.0f, one_two,   0.0f, 1.0f, 0.0f,
        one_two, 0.0f, one_two,    0.0f, 0.0f, 1.0f,
        one_two, 0.0f, -one_two,   0.0f, 1.0f, 0.0f,
        -one_two, 0.0f, -one_two,  1.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f,          0.0f, 1.0f, 0.0f

    };

    // glm::vec3 position[] = {
    //     glm::vec3(0.0f, 0.0f, 0.0f)
    // };

    uint32_t indices[] = {
        0,1,2,
        0,2,3,
        0,3,4,
        0,4,1,

        5,1,2,
        5,2,3,
        5,3,4,
        5,4,1,

    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    // glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // glBindVertexArray(0); 

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // matrix shit

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::half_pi<float>() / 2, glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5)); 


    // glm::mat4 ortho = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
    // glm::mat4 proj = glm::perspective(glm::half_pi<float>() / 2, (float)window_width/(float)window_height, 0.1f, 100.0f);

    // // model matrix
    // glm::mat4 model = glm::mat4(1.0f);
    // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 

    // glm::mat4 view = glm::mat4(1.0f);
    // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));


    glUseProgram(shader_program);

    double last_time = SDL_GetTicks() / 1000.0;
    int frame_count = 0;
    double fps = 0.0;
    std::string original_title = "OpenGL Test";

    float rotation_speed = 1;

    int mesh = 0;

    while (running_flag) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        double current_time = SDL_GetTicks() / 1000.0;
        frame_count++;
        double delta_time = current_time - last_time;

        if (delta_time >= 0.5)
        {
            fps = double(frame_count) / delta_time;

            std::string title = original_title + " - FPS: " + std::to_string((int)round(fps));
            SDL_SetWindowTitle(window, title.c_str());

            frame_count = 0;
            last_time = current_time;
        }

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running_flag = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE) {
                        running_flag = false;
                    } else if(event.key.key == SDLK_W) {
                        rotation_speed += 0.2;
                    } else if (event.key.key == SDLK_S) {
                        rotation_speed -= 0.2;
                    } else if (event.key.key == SDLK_M) {
                        if (mesh != 2) {
                            mesh++;
                        } else {
                            mesh = 0;
                        }
                    }
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                    framebuffer_size_callback(window, event.window.data1, event.window.data2);
                    break;
            }
        }
        
        // --------- rendering ----------

        switch (mesh) {
            case 0:
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                break;
            case 1:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            case 2:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            default:
                break;

        }

        glm::mat4 trans = glm::mat4(1.0f);
        //trans = glm::rotate(trans, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        trans = glm::rotate(trans, glm::half_pi<float>() / 5, glm::vec3(1.0f, 0.0f, 0.0f));
        trans = glm::translate(trans, glm::vec3(static_cast<float>(glm::sin(current_time)) / 2, static_cast<float>(glm::cos(3*current_time)) / 2,0.0f));
        trans = glm::rotate(trans, rotation_speed*(float)current_time, glm::vec3(1.0f, 0.2f, 0.0f));


        // int modelLoc = glGetUniformLocation(shader_program, "model");
        // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


        uint32_t transformLoc = glGetUniformLocation(shader_program, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));


        float green_value = (glm::sin(current_time) / 2.0f) + 0.5f;
        int vertex_colour_location = glGetUniformLocation(shader_program, "colour_multiplier");
        glUniform4f(vertex_colour_location, 1 - green_value, green_value, (glm::cos(current_time) / 2.0f) + 0.5f, 1.0f);
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(vertices) / sizeof(float), GL_UNSIGNED_INT, 0);
        SDL_GL_SwapWindow(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader_program);

    if (gl_context) SDL_GL_DestroyContext(gl_context);
    SDL_Quit();
    return 0;
}