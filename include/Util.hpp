#pragma once

#include <glm/glm.hpp>
#include <iostream>

void display_vector(glm::vec3 vec) {
    std::cout << vec.x << ", " << vec.y << ", " << vec.z << std::endl;
}

float vector_length(glm::vec3 vec) {
    return glm::sqrt(glm::pow(vec.x, 2)+ glm::pow(vec.y, 2) + glm::pow(vec.z, 2));
}