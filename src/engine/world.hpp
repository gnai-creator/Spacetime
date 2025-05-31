// ===== world.hpp =====
#pragma once
#include <glm/glm.hpp>

class World {
public:
    glm::vec3 size = glm::vec3(100.0f);
    float global_time = 0.0f;

    void update();
    glm::vec3 wrapPosition(const glm::vec3& pos) const;
    float computeLocalTime(const glm::vec3& pos) const;
};
