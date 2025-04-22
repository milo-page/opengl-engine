#include <player.hpp>

const float EPSILON = 0.0001f;

Player::Player(
    glm::vec3 position,
    glm::vec3 velocity) :
    position(position),
    velocity(velocity),
    sprint_speed(10.0f),
    move_speed(5.0f),
    flying(false),
    sprinting(false) {
};

void Player::update_physics(const glm::vec3& input_direction, const glm::vec3& gravity, float delta_time) {

    grounded = position.y <= 0.0f;

    float current_acceleration_force = acceleration_force;
    float current_friction_coefficient = friction_coefficient;

    if (!grounded && !flying) {
        current_acceleration_force *= air_control_factor;
        current_friction_coefficient *= air_control_factor;
        velocity += gravity * delta_time;
    }

    glm::vec3 horizontal_input_direction = glm::vec3(input_direction.x, 0.0f, input_direction.z);
    if (vector_length(horizontal_input_direction) > EPSILON) {
        horizontal_input_direction = glm::normalize(horizontal_input_direction);
        velocity += horizontal_input_direction * current_acceleration_force * delta_time;
    }

    glm::vec3 horizontal_velocity = glm::vec3(velocity.x, 0.0f, velocity.z);
    float horizontal_speed = vector_length(horizontal_velocity);

    // 3. Apply Friction/Damping (if no horizontal input)
    if (vector_length(input_direction) < EPSILON && horizontal_speed > EPSILON && grounded) { // Only apply ground friction if grounded
         // Calculate damping force - stronger at higher speeds, scales with time
         glm::vec3 damping = -glm::normalize(horizontal_velocity) * current_friction_coefficient; // Simplified friction
         velocity += damping * delta_time;

         // Prevent reversing direction due to friction overshoot
         glm::vec3 next_horizontal_velocity = glm::vec3(velocity.x, 0.0f, velocity.z);
         if (glm::dot(next_horizontal_velocity, horizontal_velocity) < 0.0f) {
             velocity.x = 0.0f;
             velocity.z = 0.0f;
         }
    }


    // 4. Speed Limiting
    float max_horizontal_speed = sprinting ? sprint_speed : move_speed;

    // Limit walk speed IF walking AND grounded (or if flying allows speed limit)
    // Don't limit falling speed with walk speed limit
    if (grounded || flying) { // Apply speed limits when grounded or flying
         if (horizontal_speed > max_horizontal_speed * max_horizontal_speed) {
             // Preserve vertical velocity, clamp horizontal
             glm::vec3 clamped_horizontal = glm::normalize(horizontal_velocity) * max_horizontal_speed;
             velocity.x = clamped_horizontal.x;
             velocity.z = clamped_horizontal.z;
             // velocity.y remains unchanged by this clamp
         }
    }
    if (grounded) {
        velocity.y = 0.0f;
        position.y = 0.0f;
    }
    this->position += this->velocity * delta_time;
}

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