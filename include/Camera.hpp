#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

class Camera {
    public:
        Camera(glm::vec3 initial_position = glm::vec3(-1.0f, -1.0f, 0.0f), float sensitivity = 0.1f, float aspect_ratio = 1.77778);
        void process_mouse_movement(const float delta_time, float delta_x, float delta_y);
        void set_position(glm::vec3 position);
        glm::vec3 get_position() const;
        glm::vec3 get_forward() const;
        glm::vec3 get_up() const;
        glm::vec3 get_right() const;
        float get_fov() const;
        void set_fov(float fov);
        glm::mat4 get_view_matrix() const;
        glm::mat4 get_projection_matrix() const;

    private:
        // vectors
        glm::vec3 position;
        glm::vec3 forward;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 world_up;

        // matrices
        glm::mat4 view;
        glm::mat4 projection;
        
        // angles
        float pitch;
        float yaw;

        // options
        const float sensitivity;
        float fov;
        float far_plane;
        float near_plane;
        float aspect_ratio;

        void update_vectors();
        void update_view_matrix();
};