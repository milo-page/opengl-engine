#include <SDL3/SDL.h>
#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

// standard libraries
#include <iostream>
#include <string>
#include <fstream>

// class Object {
//     public:
//         Object() {
//             glGenVertexArrays(1, &VAO);
//             glGenBuffers(1, &VBO);
//             glGenBuffers(1, &EBO);
//             glBindVertexArray(VAO);
            
//             // bind them
//             glBindBuffer(GL_ARRAY_BUFFER, VBO);
//             glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//             glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//             glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

//             // position attribute
//             glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//             glEnableVertexAttribArray(0);
//             // color attribute
//             glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
//             glEnableVertexAttribArray(1);
//         }
//     private:
//         uint32_t VBO;
//         uint32_t EBO;
//         uint32_t VAO;
//         const uint32_t vertex_count;
//         float vertices[vertex_count];
//         uint32_t indices[];
// }

using namespace gl;

class Clock {
    public:
        Clock() {
            last_time = SDL_GetTicks() / 1000.0;
        }

        void update_time() {
            current_time = SDL_GetTicks() / 1000.0;
            frame_count++;
            delta_time = current_time - last_time;
        }

        void display_fps_title(SDL_Window** window, const std::string* title) {
            if (delta_time >= 0.5)
            {
                fps = double(frame_count) / delta_time;
                
                std::string new_title = *title + " - FPS: " + std::to_string(static_cast<int>(round(fps)));
                SDL_SetWindowTitle(*window, new_title.c_str());
    
                frame_count = 0;
                last_time = current_time;
            }
        }
        
        double get_fps() {
            fps = static_cast<double>(frame_count) / delta_time;
            return fps;
        }
        double get_time() {
            return current_time;
        }
    private:
        double last_time = 0;
        double current_time = 0;
        double delta_time = 0;
        uint32_t frame_count = 0;
        double fps = 0.0;
};

// set the callback so that it will resize
void framebuffer_size_callback(SDL_Window* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// shader loading function
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

// check if the shader compiled properly
void check_shader_compilation (uint32_t shader) {
    int  success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << info_log << std::endl;
    }
}

// check if the shader program linked properly
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

    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL initialized" << '\n';
    } else {
        std::cerr << "SDL failed to initialize: " << SDL_GetError() << '\n';
    }

    // opengl stuff
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // window creation
    SDL_Window* window = nullptr;
    uint32_t window_width = 1000;
    uint32_t window_height = 1000;
    const std::string title = "OpenGL App";

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

    // enable opengl features
    glbinding::Binding::initialize();

    std::cout << "OpenGL initialized" << '\n';
    std::cout << "Vendor: "   << glGetString(GL_VENDOR)   << '\n';
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';
    std::cout << "Version: "  << glGetString(GL_VERSION)  << '\n';

    SDL_Event event;
    bool running_flag = true;

    // Enable VSync
    SDL_GL_SetSwapInterval(1);
    
    // create the opengl viewport
    glViewport(0, 0, window_width, window_height);

    // load shaders from files
    std::string vertex_shader_code = load_shader("vertex.vert");
    std::string fragment_shader_code = load_shader("fragment.frag");
    const char* vertex_shader_source = vertex_shader_code.c_str();
    const char* fragment_shader_source = fragment_shader_code.c_str();

    // vertex shader creation
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    check_shader_compilation(vertex_shader);

    // fragment shader
    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    check_shader_compilation(fragment_shader);

    // link shaders
    uint32_t shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    check_program_linking(shader_program);

    // delete shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // diamond in center of screen
    float vertices[] = {

        0.0f, 1.0f, 0.0f,           0.0f, 0.0f, 0.0f,
        -1.0, 0.0f, 1.0,            0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f,           0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, -1.0f,          0.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, -1.0f,         0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f,          0.0f, 0.0f, 0.0f

    };

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

    float floor_vertices[] = {

        -30.0f, 0.0f, 30.0f,        0.0f, 0.0f, 0.0f,
        30.0f, 0.0f, 30.0f,         0.0f, 0.0f, 0.0f,
        30.0f, 0.0f, -30.0f,        0.0f, 0.0f, 0.0f,
        -30.0f, 0.0f, -30.0f,        0.0f, 0.0f, 0.0f,


    };

    uint32_t floor_indices[] = {
        0,1,2,
        2,3,1
    };


    // create gpu objects
    uint32_t VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    
    // bind them
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

    glUseProgram(shader_program);

    // camera stuff
    glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 camera_right = glm::normalize(glm::cross(camera_front, camera_up));

    // starting variables
    int move_direction[] = {0,0,0};
    const float camera_speed = 0.05f;
    const float sensitivity = 0.2;
    float pitch = 0.0f;
    float yaw = -90.0f;
    float x_offset = 0;
    float y_offset = 0;
    float mouse_x_rel = 0;
    float mouse_y_rel = 0;
    int num_keys;
    const bool* keyboard_state = SDL_GetKeyboardState(&num_keys);
    Clock clock = Clock();
    double current_time;
    int mesh = 0;

    SDL_SetWindowRelativeMouseMode(window, true);

    while (running_flag) {
        // clear the screen
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // timing stuff
        clock.update_time();
        clock.display_fps_title(&window, &title);
        current_time = clock.get_time();

        // mouse stuff 
        mouse_x_rel = 0;
        mouse_y_rel = 0;

        // events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running_flag = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE) {
                        running_flag = false;
                    } else if (event.key.key == SDLK_M) {
                        mesh = (mesh + 1) % 3;
                    }
                    break;

                case SDL_EVENT_WINDOW_RESIZED:
                    framebuffer_size_callback(window, event.window.data1, event.window.data2);
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    mouse_x_rel = event.motion.xrel;
                    mouse_y_rel = event.motion.yrel;
                    break;
                default:
                    break;
            }
        }

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
        x_offset = mouse_x_rel * sensitivity;
        y_offset = -mouse_y_rel * sensitivity;

        yaw += x_offset;
        pitch += y_offset;

        if(pitch > 89.0f) pitch = 89.0f;
        if(pitch < -89.0f) pitch = -89.0f;

        camera_front.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
        camera_front.y = glm::sin(glm::radians(pitch));
        camera_front.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
        camera_front = glm::normalize(camera_front);

        move_direction[0] = 0;
        move_direction[1] = 0;
        move_direction[2] = 0;
        if (keyboard_state[SDL_SCANCODE_W]) move_direction[0] += 1;
        if (keyboard_state[SDL_SCANCODE_S]) move_direction[0] -= 1;
        if (keyboard_state[SDL_SCANCODE_D]) move_direction[1] += 1;
        if (keyboard_state[SDL_SCANCODE_A]) move_direction[1] -= 1;
        if (keyboard_state[SDL_SCANCODE_SPACE]) move_direction[2] -= 1;
        if (keyboard_state[SDL_SCANCODE_LCTRL]) move_direction[2] += 1;

        // camera stuff in rendering
        glm::vec3 camera_right = glm::normalize(glm::cross(camera_front, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::vec3 camera_up = glm::cross(camera_front, camera_right);

        camera_position += move_direction[0] * camera_speed * camera_front;
        camera_position += move_direction[1] * camera_speed * camera_right;
        camera_position += move_direction[2] * camera_speed * camera_up;
        
        glm::mat4 view;
        view = glm::lookAt(camera_position, camera_position + camera_front, glm::vec3(0.0f,1.0f,0.0f));//camera_up);

        glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 projection = glm::perspective(glm::half_pi<float>() * 0.7f, static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);
        model = glm::rotate(model, static_cast<float>(glm::sin(current_time)), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::half_pi<float>() / 4, glm::vec3(1.0f, 0.0f, 0.0f));

        // passing model matrix to uniform in shaders
        uint32_t model_uniform_location = glGetUniformLocation(shader_program, "model");
        glUniformMatrix4fv(model_uniform_location, 1, GL_FALSE, glm::value_ptr(model));
        
        // same for view
        uint32_t view_uniform_location = glGetUniformLocation(shader_program, "view");
        glUniformMatrix4fv(view_uniform_location, 1, GL_FALSE, glm::value_ptr(view));

        // same for projection
        uint32_t projection_uniform_location = glGetUniformLocation(shader_program, "projection");
        glUniformMatrix4fv(projection_uniform_location, 1, GL_FALSE, glm::value_ptr(projection));

        float positive_sin = (glm::sin(current_time) / 2.0f) + 0.5f;
        float positive_cos = (glm::cos(current_time) / 2.0f) + 0.5f;

        int vertex_colour_location = glGetUniformLocation(shader_program, "colour_multiplier");
        glUniform3f(vertex_colour_location, positive_sin / 2.0f, positive_cos / 2.0f, (positive_cos + positive_sin) / 4.0f);
        
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(uint32_t), GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glUniform3f(vertex_colour_location, 0.0f, 0.0f, 0.0f);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(uint32_t), GL_UNSIGNED_INT, 0);
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