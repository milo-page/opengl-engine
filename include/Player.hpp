#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

class Player {
    public:
        Player(
            glm::vec3 position = glm::vec3(0.0f),
            glm::vec3 velocity = glm::vec3(0.0f)
        );
        void update_physics(const glm::vec3& input_direction, const glm::vec3& gravity, float delta_time);
        glm::vec3 get_position() const;
        glm::vec3 get_velocity() const;
        bool is_flying() const;
        bool is_sprinting() const;
        void set_position(const glm::vec3& position);
        void set_velocity(const glm::vec3& velocity);
        void set_flying(bool flying);
        void set_sprinting(bool sprinting);

    private:
        glm::vec3 position;
        glm::vec3 velocity;
        float move_speed;
        float sprint_speed;
        float acceleration_force = 50.0f;
        float friction_coefficient = 10.0f;
        float air_control_factor = 0.1f;
        bool flying;
        bool sprinting;

        void limit_horizontal_speed(float max_speed);
};