#include <utils.hpp>
#include <iostream>
#include <glm/gtx/norm.hpp>

void display_vector(const glm::vec3& v) {
    std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
}

float vector_length(const glm::vec3& v) {
    return glm::length(v);
}
