#include <player.hpp>

Player::Player(
    glm::vec3 position,
    glm::vec3 velocity) :
    position(position),
    velocity(velocity),
    flying(false),
    sprinting(false) {
};

void Player::update_physics(const glm::vec3& input_direction, const glm::vec3& gravity, float delta_time) {

    grounded = (position.y <= 0.0f + constants::epsilon);

    if (grounded) {
        if (position.y < 0.0f) {
            position.y = 0.0f;
        }

        if (velocity.y < 0.0f) {
             velocity.y = 0.0f;
        }
    }

    if (flying) {
        velocity += input_direction * flying_acceleration * delta_time;

        if (glm::length2(velocity) > constants::epsilon_squared) {
            float damping_factor = glm::max(0.0f, 1.0f - flying_damping * delta_time);
            velocity *= damping_factor;
        }

        if (glm::length2(velocity) > flying_speed * flying_speed) {
            velocity = flying_speed * glm::normalize(velocity);
        }

    } else {
        if (!grounded) {
            velocity += gravity * delta_time;
        }

        float current_acceleration = grounded ? ground_acceleration : air_acceleration;
        float current_damping = grounded ? ground_damping : air_damping;

        glm::vec3 horizontal_input_direction = glm::vec3(input_direction.x, 0.0f, input_direction.z);

        if (glm::length2(horizontal_input_direction) > constants::epsilon_squared) {
            horizontal_input_direction = glm::normalize(horizontal_input_direction);
            velocity += horizontal_input_direction * current_acceleration * delta_time;

            if (!grounded) {
                glm::vec3 horizontal_velocity = glm::vec3(velocity.x, 0.0f, velocity.z);
                if (glm::length2(horizontal_velocity) > constants::epsilon_squared) {
                     float damping_factor = glm::max(0.0f, 1.0f - current_damping * delta_time);
                     velocity.x *= damping_factor;
                     velocity.z *= damping_factor;
                }
            }

        } else {
            glm::vec3 horizontal_velocity = glm::vec3(velocity.x, 0.0f, velocity.z);
            if (glm::length2(horizontal_velocity) > constants::epsilon_squared) {
                 float damping_factor = glm::max(0.0f, 1.0f - current_damping * delta_time);
                 velocity.x *= damping_factor;
                 velocity.z *= damping_factor;
            }
        }

        glm::vec3 final_horizontal_velocity = glm::vec3(velocity.x, 0.0f, velocity.z);
        float horizontal_speed_sq = glm::length2(final_horizontal_velocity);

        float max_horizontal_speed = sprinting ? sprint_speed : move_speed;

        if (glm::length2(final_horizontal_velocity) > max_horizontal_speed * max_horizontal_speed) {
            glm::vec3 clamped_horizontal = glm::normalize(final_horizontal_velocity) * max_horizontal_speed;
            velocity.x = clamped_horizontal.x;
            velocity.z = clamped_horizontal.z;
        }

        float max_fall_speed = 50.0f;
        if (velocity.y < -max_fall_speed) {
            velocity.y = -max_fall_speed;
        }
    }

    position += velocity * delta_time;

     if (!flying && position.y < 0.0f) {
        position.y = 0.0f;
     }
};

glm::vec3 Player::get_position() const {
    return position;
};

glm::vec3 Player::get_velocity() const {
    return velocity;
};

bool Player::is_flying() const {
    return flying;
}

bool Player::is_sprinting() const {
    return sprinting;
};

void Player::set_position(const glm::vec3& position) {
    this->position = position;
};

void Player::set_velocity(const glm::vec3& velocity) {
    this->velocity = velocity;
};

void Player::set_flying(bool flying) {
    this->flying = flying;
};

void Player::set_sprinting(bool sprinting) {
    this->sprinting = sprinting;
};

void Player::jump() {
    if (grounded) {
        velocity.y += jump_strength;
    }
}

void Player::display_state() {
    std::cout << "positon: " << position.x << ", " << position.y << ", " << position.z
        << " velocity: " << velocity.x << ", " << velocity.y << ", " << velocity.z
        << " flying: " << flying << " grounded: " << grounded << " sprinting: " << sprinting
        << std::endl;
}