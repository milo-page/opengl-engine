#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <map> // Needed for Shader class uniform location caching (optional)

#include <SDL3/SDL.h>

// --- glbinding ---
#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

// --- GLM ---
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

// Use the gl namespace from glbinding
using namespace gl;

// Simple Shader Class (from LearnOpenGL) - Slightly modified for location caching
class Shader {
public:
    GLuint ID; // Use GLuint for OpenGL handles

    Shader(const std::string& vertexPath, const std::string& fragmentPath) : ID(0) { // Initialize ID
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        } catch (const std::ifstream::failure& e) {
            std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << vertexPath << " or " << fragmentPath << "\n" << e.what() << std::endl;
            // Consider throwing or setting an error state instead of just printing
            return; // Exit constructor on failure
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        // 2. compile shaders
        GLuint vertex, fragment;
        vertex = gl::glCreateShader(GL_VERTEX_SHADER);
        gl::glShaderSource(vertex, 1, &vShaderCode, NULL);
        gl::glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        fragment = gl::glCreateShader(GL_FRAGMENT_SHADER);
        gl::glShaderSource(fragment, 1, &fShaderCode, NULL);
        gl::glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        ID = gl::glCreateProgram();
        gl::glAttachShader(ID, vertex);
        gl::glAttachShader(ID, fragment);
        gl::glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        gl::glDeleteShader(vertex);
        gl::glDeleteShader(fragment);
    }

    ~Shader() {
        if (ID != 0) {
            gl::glDeleteProgram(ID);
        }
    }

    void use() const {
        gl::glUseProgram(ID);
    }
    // utility uniform functions
    void setBool(const std::string &name, bool value) const {
        gl::glUniform1i(getLoc(name), (int)value);
    }
    void setInt(const std::string &name, int value) const {
        gl::glUniform1i(getLoc(name), value);
    }
    void setFloat(const std::string &name, float value) const {
        gl::glUniform1f(getLoc(name), value);
    }
    void setVec2(const std::string &name, const glm::vec2 &value) const {
        gl::glUniform2fv(getLoc(name), 1, glm::value_ptr(value));
    }
    void setVec2(const std::string &name, float x, float y) const {
        gl::glUniform2f(getLoc(name), x, y);
    }
    void setVec3(const std::string &name, const glm::vec3 &value) const {
        gl::glUniform3fv(getLoc(name), 1, glm::value_ptr(value));
    }
    void setVec3(const std::string &name, float x, float y, float z) const {
        gl::glUniform3f(getLoc(name), x, y, z);
    }
    void setVec4(const std::string &name, const glm::vec4 &value) const {
        gl::glUniform4fv(getLoc(name), 1, glm::value_ptr(value));
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const {
        gl::glUniform4f(getLoc(name), x, y, z, w);
    }
    void setMat2(const std::string &name, const glm::mat2 &mat) const {
        gl::glUniformMatrix2fv(getLoc(name), 1, GL_FALSE, glm::value_ptr(mat));
    }
    void setMat3(const std::string &name, const glm::mat3 &mat) const {
        gl::glUniformMatrix3fv(getLoc(name), 1, GL_FALSE, glm::value_ptr(mat));
    }
    void setMat4(const std::string &name, const glm::mat4 &mat) const {
        gl::glUniformMatrix4fv(getLoc(name), 1, GL_FALSE, glm::value_ptr(mat));
    }

private:
    // Simple caching for uniform locations (optional performance improvement)
    mutable std::map<std::string, GLint> uniformLocationCache;
    GLint getLoc(const std::string& name) const {
        auto it = uniformLocationCache.find(name);
        if (it == uniformLocationCache.end()) {
            GLint loc = gl::glGetUniformLocation(ID, name.c_str());
            uniformLocationCache[name] = loc;
            return loc;
        }
        return it->second;
    }

    void checkCompileErrors(GLuint shader, std::string type) {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM") {
            gl::glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                gl::glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        } else {
            gl::glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                gl::glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        }
    }
};


// Callback for window resize
void framebuffer_size_callback(SDL_Window* window, int width, int height) {
    if (height == 0) height = 1;
    gl::glViewport(0, 0, width, height);
}


int main(int argc, char* argv[]) {
    // --- SDL Initialization ---
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) { // Ensure Events subsystem is initialized
        std::cerr << "SDL failed to initialize: " << SDL_GetError() << std::endl;
        return 1;
    } else {
        std::cout << "SDL initialized" << std::endl;
    }

    // --- SDL Window and OpenGL Context Setup ---
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    int window_width = 1000;
    int window_height = 1000;
    const std::string baseWindowTitle = "OpenGL Camera Movement";

    SDL_Window* window = SDL_CreateWindow(
        baseWindowTitle.c_str(),
        window_width, window_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN
    );
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        std::cerr << "OpenGL context creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // --- Initialize glbinding ---
    glbinding::Binding::initialize(SDL_GL_GetProcAddress);
    std::cout << "glbinding initialized" << std::endl;

    std::cout << "OpenGL Vendor: "   << (const char*)gl::glGetString(GL_VENDOR)   << std::endl;
    std::cout << "OpenGL Renderer: " << (const char*)gl::glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: "  << (const char*)gl::glGetString(GL_VERSION)  << std::endl;

    // --- Configure OpenGL State ---
    gl::glEnable(GL_DEPTH_TEST);
    gl::glEnable(GL_CULL_FACE);
    gl::glCullFace(GL_BACK);
    gl::glFrontFace(GL_CCW);

    if (SDL_GL_SetSwapInterval(1) < 0) {
         std::cerr << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << std::endl;
    }

    // --- Capture Mouse for Look Controls ---
    SDL_SetWindowRelativeMouseMode(window, true);

    // --- Load Shaders ---
    Shader ourShader("../src/shaders/vertex.vert", "../src/shaders/fragment.frag");
    if (ourShader.ID == 0) {
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // --- Vertex Data (Octahedron - Position + Color) ---
    float one_two = 1.0f / glm::sqrt(2.0f);
    float vertices[] = {
        // Position           Color
        0.0f,  1.0f,  0.0f,   1.0f, 0.0f, 0.0f, // 0: Top Apex
       -one_two, 0.0f,  one_two,  0.0f, 1.0f, 0.0f, // 1: Middle (-X, +Z)
        one_two, 0.0f,  one_two,  0.0f, 0.0f, 1.0f, // 2: Middle (+X, +Z)
        one_two, 0.0f, -one_two,  0.0f, 1.0f, 0.0f, // 3: Middle (+X, -Z)
       -one_two, 0.0f, -one_two,  1.0f, 0.0f, 0.0f, // 4: Middle (-X, -Z)
        0.0f, -1.0f,  0.0f,   0.0f, 1.0f, 0.0f  // 5: Bottom Apex
    };
    uint32_t indices[] = {
        0, 1, 2,  0, 2, 3,  0, 3, 4,  0, 4, 1, // Top faces
        5, 2, 1,  5, 3, 2,  5, 4, 3,  5, 1, 4  // Bottom faces (ensure CCW from outside)
    };
    const GLsizei indexCount = sizeof(indices) / sizeof(indices[0]);

    // --- OpenGL Buffer Setup ---
    GLuint VAO, VBO, EBO;
    gl::glGenVertexArrays(1, &VAO);
    gl::glGenBuffers(1, &VBO);
    gl::glGenBuffers(1, &EBO);

    gl::glBindVertexArray(VAO);

    gl::glBindBuffer(GL_ARRAY_BUFFER, VBO);
    gl::glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    gl::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    gl::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLsizei stride = 6 * sizeof(float);
    gl::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0); // Position
    gl::glEnableVertexAttribArray(0);
    gl::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float))); // Color
    gl::glEnableVertexAttribArray(1);

    gl::glBindBuffer(GL_ARRAY_BUFFER, 0);
    gl::glBindVertexArray(0);


    // --- Application Variables ---
    SDL_Event event;
    bool running_flag = true;
    double lastFrameTime = SDL_GetTicks() / 1000.0;
    int frameCount = 0;
    double fps = 0.0;
    float objectRotationSpeed = 0.5f; // Reduced speed slightly
    int polygonMode = 2;

    // --- Camera Variables ---
    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.5f, 3.5f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
    float cameraSpeed = 2.5f;

    // --- Mouse Look Variables ---
    float yaw   = -90.0f; // Yaw is initialized pointing along negative Z axis
    float pitch =  0.0f;  // Pitch starts level
    float lastX = (float)window_width / 2.0f; // Start mouse in middle
    float lastY = (float)window_height / 2.0f;
    float mouseSensitivity = 0.05f;
    bool firstMouse = true; // Flag to handle initial mouse jump

    // Set initial viewport
    gl::glViewport(0, 0, window_width, window_height);

    // --- Render Loop ---
    while (running_flag) {
        // --- Timing Calculation ---
        double currentFrameTime = SDL_GetTicks() / 1000.0;
        double deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
        frameCount++;

        // Update FPS counter every half second
        static double fpsTimer = 0.0;
        fpsTimer += deltaTime;
        if (fpsTimer >= 0.5) {
            fps = double(frameCount) / fpsTimer;
            std::string title = baseWindowTitle + " - FPS: " + std::to_string((int)round(fps));
            SDL_SetWindowTitle(window, title.c_str());
            frameCount = 0;
            fpsTimer = 0.0;
        }

        // --- Event Handling ---
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running_flag = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE) {
                        running_flag = false;
                    } else if (event.key.key == SDLK_M) {
                        polygonMode = (polygonMode + 1) % 3;
                    }
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                    window_width = event.window.data1;
                    window_height = event.window.data2;
                    framebuffer_size_callback(window, window_width, window_height);
                    break;
                case SDL_EVENT_MOUSE_MOTION: // Handle mouse look
                    { // Create scope for motion variables
                        float xpos = event.motion.x;
                        float ypos = event.motion.y;

                        // Prevent jump on first mouse capture
                        if (firstMouse) {
                            lastX = xpos;
                            lastY = ypos;
                            firstMouse = false;
                        }

                        // Calculate offset since last frame
                        float xoffset = event.motion.xrel; // Use relative motion provided by SDL
                        float yoffset = -event.motion.yrel; // Reversed since y-coordinates go from bottom to top in OpenGL

                        // Apply sensitivity
                        xoffset *= mouseSensitivity;
                        yoffset *= mouseSensitivity;

                        // Update yaw and pitch
                        yaw += xoffset;
                        pitch += yoffset;

                        // Constrain pitch to avoid flipping
                        if (pitch > 89.0f) pitch = 89.0f;
                        if (pitch < -89.0f) pitch = -89.0f;

                        // Calculate new cameraFront vector
                        glm::vec3 front;
                        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                        front.y = sin(glm::radians(pitch));
                        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                        cameraFront = glm::normalize(front);
                    }
                    break; // End of MOUSE_MOTION case
            } // End switch event.type
        } // End while SDL_PollEvent

        // --- Input Handling for Camera Movement (Keyboard) ---
        const bool* currentKeyStates = SDL_GetKeyboardState(NULL);
        float actualCameraSpeed = cameraSpeed * (float)deltaTime;

        if (currentKeyStates[SDL_SCANCODE_W]) cameraPos += actualCameraSpeed * cameraFront;
        if (currentKeyStates[SDL_SCANCODE_S]) cameraPos -= actualCameraSpeed * cameraFront;
        if (currentKeyStates[SDL_SCANCODE_A]) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * actualCameraSpeed;
        if (currentKeyStates[SDL_SCANCODE_D]) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * actualCameraSpeed;
        if (currentKeyStates[SDL_SCANCODE_SPACE]) cameraPos += actualCameraSpeed * cameraUp;
        if (currentKeyStates[SDL_SCANCODE_LCTRL] || currentKeyStates[SDL_SCANCODE_RCTRL]) cameraPos -= actualCameraSpeed * cameraUp;


        // --- Rendering ---
        gl::glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set Polygon Mode
        switch (polygonMode) {
            case 0: gl::glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
            case 1: gl::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
            case 2: gl::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
        }

        // Activate shader
        ourShader.use();

        // --- Transformations ---
        // View matrix (camera) - Updated based on cameraPos and new cameraFront
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Projection matrix
        float aspectRatio = (window_height > 0) ? (float)window_width / (float)window_height : 1.0f;
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

        // Model matrix (object's transformation)
        glm::mat4 model = glm::mat4(1.0f);
        float angle = objectRotationSpeed * (float)currentFrameTime;
        model = glm::rotate(model, angle, glm::normalize(glm::vec3(0.5f, 1.0f, 0.2f)));

        // --- Set Uniforms ---
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);

        // --- Draw Call ---
        gl::glBindVertexArray(VAO);
        gl::glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        gl::glBindVertexArray(0);

        // --- Swap Buffers ---
        SDL_GL_SwapWindow(window);
    }

    // --- Cleanup ---
    // Release mouse capture
    SDL_SetWindowRelativeMouseMode(window, false);

    // Shader object goes out of scope here, its destructor deletes the program
    gl::glDeleteVertexArrays(1, &VAO);
    gl::glDeleteBuffers(1, &VBO);
    gl::glDeleteBuffers(1, &EBO);

    if (gl_context) SDL_GL_DestroyContext(gl_context);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << "SDL cleaned up." << std::endl;
    return 0;
}