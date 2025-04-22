#pragma once

#include <utils.hpp>

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
        void jump();
        void display_state();

    private:
        // vectors
        glm::vec3 position;
        glm::vec3 velocity;

        // movement
        float move_speed                = 10.0f;
        float sprint_speed              = 20.0f;
        float flying_speed              = 15.0f;
        float jump_strength             = 5.0f;
        float ground_acceleration       = 20.0f;
        float air_acceleration          = 10.0f;
        float flying_acceleration       = 40.0f;
        float ground_damping            = 30.0f;
        float air_damping               = 1.0f;
        float flying_damping            = 5.0f;

        // state
        bool flying;
        bool sprinting;
        bool grounded;
};