#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class ToroidalWorld {
public:
    float R = 1e5f;
    float r = 1e5f;
    float global_time = 0.0f;

    void update();
    glm::vec2 cartesianToToroidal(const glm::vec3& pos) const;
    glm::vec3 toroidalToCartesian(float theta, float phi) const;
    glm::vec3 wrapToroidalPosition(const glm::vec3& pos) const;
    float computeLocalTime(const glm::vec3& pos) const;

    float getSizeX() const { return 2.0f * glm::pi<float>() * R; }
    float getSizeZ() const { return 2.0f * glm::pi<float>() * r; }
    glm::vec3 getSize() const; // ← apenas a *declaração*!
};
