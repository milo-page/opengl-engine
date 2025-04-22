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

// custom
#include <player.hpp>
#include <camera.hpp>
#include <clock.hpp>
#include <utils.hpp>

using namespace gl;

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
    int window_width = 1000;
    int window_height = 1000;
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

    // depth testin
    glEnable(GL_DEPTH_TEST);

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

        0.0f, 1.0f, 0.0f,           1.0f, 0.0f, 0.0f,
        -1.0, 0.0f, 1.0,            0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 1.0f,           0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, -1.0f,          0.0f, 1.0f, 0.0f,
        -1.0f, 0.0f, -1.0f,         0.0f, 0.0f, 1.0f,
        0.0f, -1.0f, 0.0f,          1.0f, 0.0f, 0.0f

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

    float floor = -1.0f;

    float floor_vertices[] = {
        -30.0f, floor, 30.0f,        0.0f, 0.0f, 0.5f,
        30.0f, floor, 30.0f,         0.5f, 0.0f, 0.0f,
        30.0f, floor, -30.0f,        0.0f, 0.5f, 0.0f,
        -30.0f, floor, -30.0f,       0.5f, 0.0f, 0.0f,
    };

    uint32_t floor_indices[] = {
        0,1,2,
        2,0,3
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

    // floor stuff
    // create gpu objects
    uint32_t floor_VBO, floor_VAO, floor_EBO;
    glGenVertexArrays(1, &floor_VAO);
    glGenBuffers(1, &floor_VBO);
    glGenBuffers(1, &floor_EBO);
    glBindVertexArray(floor_VAO);
    
    // bind them
    glBindBuffer(GL_ARRAY_BUFFER, floor_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floor_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floor_indices), floor_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shader_program);

    const bool* keyboard_state = SDL_GetKeyboardState(nullptr);
    Clock clock = Clock();
    double current_time;
    int mesh = 0;

    const glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);
    double delta_time = 0.0f;
    float aspect_ratio = static_cast<float>(window_width / window_height);
    GLenum current_mode;
    bool window_transition = false;
    int mouse_delta_x = 0;
    int mouse_delta_y = 0;

    // objects
    Player player = Player(glm::vec3(-10.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    Camera player_camera = Camera(player.get_position(), 0.05f, aspect_ratio);

    // capture mouse
    SDL_SetWindowRelativeMouseMode(window, true);

    while (running_flag) {

        // clear the screen
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // timing stuff
        clock.update_time();
        clock.display_fps_title(&window, &title);
        current_time = clock.get_time();
        delta_time = clock.get_delta_time();

        // mouse stuff 
        mouse_delta_x = 0;
        mouse_delta_y = 0;

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
                    } else if (event.key.key == SDLK_F) {
                        player.set_flying(!player.is_flying());
                    } else if (event.key.key == SDLK_SPACE) {
                        player.jump();
                    }
                    break;

                case SDL_EVENT_WINDOW_RESIZED:
                    window_width = event.window.data1;
                    window_height = event.window.data2;
                    aspect_ratio = static_cast<float>(window_width) / static_cast<float>(window_height);
                    framebuffer_size_callback(window, window_width, window_height);
                    player_camera.set_aspect_ratio(aspect_ratio);
                    window_transition = true;
                    break;

                case SDL_EVENT_MOUSE_MOTION:
                    mouse_delta_x = event.motion.xrel;
                    mouse_delta_y = event.motion.yrel;
                    break;

                default:
                    break;
            }
        }

        switch (mesh) {
            case 0:
                current_mode = GL_FILL;
                break;
            case 1:
                current_mode = GL_LINE;
                break;
            case 2:
                current_mode = GL_POINT;
                break;
            default:
                break;
        }
        if (!window_transition) {
            player_camera.process_mouse_movement(delta_time, mouse_delta_x, mouse_delta_y);
        } else {
            window_transition = false;
        }

        glm::vec3 camera_forward = player_camera.get_forward();
        glm::vec3 camera_right = player_camera.get_right();
        glm::vec3 camera_world_up = player_camera.get_world_up();

        keyboard_state = SDL_GetKeyboardState(nullptr);
        glm::vec3 input_direction = glm::vec3(0.0f);

        glm::vec3 forward = glm::normalize(glm::vec3(camera_forward.x, 0.0f, camera_forward.z));
        glm::vec3 right = glm::normalize(glm::vec3(camera_right.x, 0.0f, camera_right.z));

        if (keyboard_state[SDL_SCANCODE_W]) input_direction += forward;
        if (keyboard_state[SDL_SCANCODE_S]) input_direction -= forward;
        if (keyboard_state[SDL_SCANCODE_D]) input_direction += right;
        if (keyboard_state[SDL_SCANCODE_A]) input_direction -= right;
        if (player.is_flying()) {
            if (keyboard_state[SDL_SCANCODE_SPACE]) input_direction += camera_world_up;
            if (keyboard_state[SDL_SCANCODE_LCTRL]) input_direction -= camera_world_up;
        }

        player.set_sprinting(false);

        if (glm::length2(input_direction) > constants::epsilon_squared) {
            input_direction = glm::normalize(input_direction);

            if (keyboard_state[SDL_SCANCODE_W] && keyboard_state[SDL_SCANCODE_LSHIFT]) {
                player.set_sprinting(true);
            }
        }

        // update player and camera
        player.update_physics(input_direction, gravity, delta_time);
        player_camera.set_position(player.get_position() + glm::vec3(0.0f, 1.0f, 0.0f));

        // update model at some point
        glm::mat4 model = glm::mat4(1.0f);

        // passing model matrix to uniform in shaders
        uint32_t model_uniform_location = glGetUniformLocation(shader_program, "model");
        glUniformMatrix4fv(model_uniform_location, 1, GL_FALSE, glm::value_ptr(model));
        
        // same for view
        uint32_t view_uniform_location = glGetUniformLocation(shader_program, "view");
        glUniformMatrix4fv(view_uniform_location, 1, GL_FALSE, glm::value_ptr(player_camera.get_view_matrix()));

        // same for projection
        uint32_t projection_uniform_location = glGetUniformLocation(shader_program, "projection");
        glUniformMatrix4fv(projection_uniform_location, 1, GL_FALSE, glm::value_ptr(player_camera.get_projection_matrix()));

        // changing colour a bit
        int vertex_colour_location = glGetUniformLocation(shader_program, "colour_multiplier");
        glUniform3f(vertex_colour_location, 0.5, glm::sin(current_time) / 2 + 0.5f, 0.5);

        // rendering diamond
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, current_mode);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(uint32_t), GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glUniform3f(vertex_colour_location, 0.0f, 0.0f, 0.0f);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(uint32_t), GL_UNSIGNED_INT, 0);

        // rendering floor
        glPolygonMode(GL_FRONT_AND_BACK, current_mode);
        glBindVertexArray(floor_VAO);
        glDrawElements(GL_TRIANGLES, sizeof(floor_indices) / sizeof(uint32_t), GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window);
    }
    
    // cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glDeleteVertexArrays(1, &floor_VAO);
    glDeleteBuffers(1, &floor_VBO);
    glDeleteBuffers(1, &floor_EBO);
    glDeleteProgram(shader_program);

    if (gl_context) SDL_GL_DestroyContext(gl_context);
    SDL_Quit();
    return 0;
}