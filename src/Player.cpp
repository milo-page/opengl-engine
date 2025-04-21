#pragma once

#include <Player.hpp>

Player::Player(
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f)) :
    position(position),
    velocity(velocity),
    acceleration(acceleration) {
};

void Player::update_physics(float delta_time) {
    set_velocity(velocity + (acceleration * delta_time));
    set_position(position + (velocity * delta_time));
}

glm::vec3 Player::get_position() {
    return position;
};

glm::vec3 Player::get_velocity() {
    return velocity;
};

glm::vec3 Player::get_acceleration() {
    return acceleration;
};