#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Player {
    public:
        Player(
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f)
        );
        void update_physics(float delta_time);
        glm::vec3 get_position();
        glm::vec3 get_velocity();
        glm::vec3 get_acceleration();
        void set_position(glm::vec3 position);
        void set_velocity(glm::vec3 velocity);
        void set_acceleration(glm::vec3 acceleration);

    private:
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 acceleration;
};