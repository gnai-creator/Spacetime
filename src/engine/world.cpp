// ===== world.cpp =====
#include "world.hpp"
#include <cmath>

void World::update() {
    global_time += 0.016f; // Assumindo 60 FPS
}

glm::vec3 World::wrapPosition(const glm::vec3& pos) const {
    return glm::mod(glm::mod(pos, size) + size, size);
}

float World::computeLocalTime(const glm::vec3& pos) const {
    return global_time + sin(pos.x * 0.1f) + cos(pos.y * 0.1f);
}
