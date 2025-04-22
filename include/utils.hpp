#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>

namespace constants {
    inline constexpr float epsilon = 0.00001f;
    inline constexpr float epsilon_squared = epsilon * epsilon;
}

void display_vector(const glm::vec3& v);
float vector_length(const glm::vec3& v);