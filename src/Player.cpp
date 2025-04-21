#include <Player.hpp>
// #include <glm/gtx/norm.hpp>

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

// void Player::update_physics(float delta_time) {
//     if (sprinting) {
//         set_velocity(sprint_speed * glm::normalize(velocity + (acceleration * delta_time)));
//     } else {
//         set_velocity(move_speed * glm::normalize(velocity + (acceleration * delta_time)));
//     }

//     set_position(position + (velocity * delta_time));
// };

void Player::update_physics(const glm::vec3& input_direction, const glm::vec3& gravity, float delta_time) {

    // Determine current control factor (less control in air unless flying)
    // NOTE: You need logic to determine if the player is 'grounded'
    // For this example, we assume not grounded unless flying=true overrides gravity.
    bool grounded = position.y >= 0.0f; // <<<< TODO: Implement ground detection!
    float current_acceleration_force = acceleration_force;
    float current_friction_coefficient = friction_coefficient;

    if (!grounded && !flying) {
        current_acceleration_force *= air_control_factor;
        current_friction_coefficient *= air_control_factor; // Less friction in air
    }

    // --- Apply Forces/Acceleration ---

    // 1. Gravity (only if not flying)
    if (!flying) {
        velocity += gravity * delta_time;
    }

    // 2. Movement Input Acceleration (Horizontal Plane)
    glm::vec3 horizontal_input_dir = glm::vec3(input_direction.x, 0.0f, input_direction.z);
    if (glm::sqrt(horizontal_input_dir.length()) > EPSILON) {
         // Normalize again just to be safe if input wasn't perfectly normalized
        horizontal_input_dir = glm::normalize(horizontal_input_dir);
        velocity += horizontal_input_dir * current_acceleration_force * delta_time;
    }

    // --- Handle Horizontal Speed & Friction ---
    glm::vec3 horizontal_velocity = glm::vec3(velocity.x, 0.0f, velocity.z);
    float horizontal_speed_sq = glm::sqrt(horizontal_velocity.length());

    // 3. Apply Friction/Damping (if no horizontal input)
    if (glm::sqrt(input_direction.length()) < EPSILON && horizontal_speed_sq > EPSILON && grounded) { // Only apply ground friction if grounded
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
         if (horizontal_speed_sq > max_horizontal_speed * max_horizontal_speed) {
             // Preserve vertical velocity, clamp horizontal
             glm::vec3 clamped_horizontal = glm::normalize(horizontal_velocity) * max_horizontal_speed;
             velocity.x = clamped_horizontal.x;
             velocity.z = clamped_horizontal.z;
             // velocity.y remains unchanged by this clamp
         }
    }
    this->position += this->velocity * delta_time;
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