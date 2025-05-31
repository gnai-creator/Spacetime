// ===== toroidal_world.hpp =====
#pragma once
#include <glm/glm.hpp>
#include <cmath>

class ToroidalWorld {
public:
    float R = 20.0f;  // raio do círculo grande (toro)
    float r = 10.0f;  // raio do círculo pequeno (tubo)
    float global_time = 0.0f;

    void update();

    glm::vec2 cartesianToToroidal(const glm::vec3& pos) const;
    glm::vec3 toroidalToCartesian(float theta, float phi) const;

    glm::vec3 wrapToroidalPosition(const glm::vec3& pos) const;
    float computeLocalTime(const glm::vec3& pos) const;
    glm::vec3 getSize() const; // novo método
};