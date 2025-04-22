#include <camera.hpp>

Camera::Camera(glm::vec3 initial_position, float sensitivity, float aspect_ratio) :
    position(initial_position),
    world_up(0.0f, 1.0f, 0.0f),
    yaw(0.0f),
    pitch(0.0f),
    sensitivity(sensitivity),
    fov(45.0f),
    far_plane(100.0f),
    near_plane(0.1f),
    aspect_ratio(aspect_ratio) {
};

void Camera::process_mouse_movement(const float delta_time, float delta_x, float delta_y) {
    delta_x *= sensitivity;
    delta_y *= -sensitivity;

    yaw += delta_x * delta_time;
    pitch += delta_y * delta_time;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }
    update_vectors();
};

void Camera::set_position(glm::vec3 position) {
    this->position = position;
};

void Camera::update_vectors() {

    forward.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    forward.y = glm::sin(glm::radians(pitch));
    forward.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    forward = glm::normalize(forward);

    right = glm::normalize(glm::cross(forward, world_up));
    up = glm::normalize(glm::cross(right, forward));

    update_view_matrix();
};

void Camera::update_view_matrix() {
    view = glm::lookAt(position, position + forward, up);
    projection = glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
};

glm::vec3 Camera::get_position() const { 
    return position;
};

glm::vec3 Camera::get_forward() const {
    return forward;
};

glm::vec3 Camera::get_up() const {
    return up;
};

glm::vec3 Camera::get_right() const {
    return right;
};

float Camera::get_fov() const {
    return fov;
};

void Camera::set_fov(float fov) {
    this->fov = fov;
};

void Camera::set_aspect_ratio(float aspect_ratio) {
    this->aspect_ratio = aspect_ratio;
}

glm::mat4 Camera::get_view_matrix() const {
    return view;
};

glm::mat4 Camera::get_projection_matrix() const {
    return projection;
};