// ===== toroid_utils.hpp =====
#pragma once
#include <glm/glm.hpp>

// Converte coordenadas toroidais para espaciais 3D
inline glm::vec3 toroidalToCartesian(float majorRadius, float minorRadius, float theta, float phi) {
    float x = (majorRadius + minorRadius * cos(theta)) * cos(phi);
    float y = (majorRadius + minorRadius * cos(theta)) * sin(phi);
    float z = minorRadius * sin(theta);
    return glm::vec3(x, y, z);
}

// Converte coordenadas 3D para coordenadas toroidais aproximadas (útil se necessário)
inline glm::vec2 cartesianToToroidal(const glm::vec3& pos, float majorRadius, float minorRadius) {
    float phi = atan2(pos.y, pos.x);
    float theta = atan2(pos.z, glm::length(glm::vec2(pos.x, pos.y)) - majorRadius);
    return glm::vec2(theta, phi);
}

inline float toroidal_wrap(float value, float max_value) {
    return fmodf((fmodf(value, max_value) + max_value), max_value);
}
