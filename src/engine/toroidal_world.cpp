// ===== toroidal_world.cpp =====
#include "toroidal_world.hpp"

void ToroidalWorld::update() {
    global_time += 0.016f; // assuming 60 FPS
}

// Converte posição cartesiana (x, y, z) para coordenadas toroidais (θ, φ)
glm::vec2 ToroidalWorld::cartesianToToroidal(const glm::vec3& pos) const {
    float theta = atan2(pos.z, pos.x);
    float phi = atan2(pos.y, glm::length(glm::vec2(pos.x, pos.z)) - R);
    return glm::vec2(theta, phi);
}

// Converte coordenadas toroidais (θ, φ) para posição cartesiana (x, y, z)
glm::vec3 ToroidalWorld::toroidalToCartesian(float theta, float phi) const {
    float x = (R + r * cos(phi)) * cos(theta);
    float z = (R + r * cos(phi)) * sin(theta);
    float y = r * sin(phi);
    return glm::vec3(x, y, z);
}

// Aplica warp angular em θ e φ para garantir continuidade toroidal
glm::vec3 ToroidalWorld::wrapToroidalPosition(const glm::vec3& pos) const {
    glm::vec2 toroidal = cartesianToToroidal(pos);
    toroidal.x = fmod(toroidal.x + 2 * M_PI, 2 * M_PI);
    toroidal.y = fmod(toroidal.y + 2 * M_PI, 2 * M_PI);
    return toroidalToCartesian(toroidal.x, toroidal.y);
}

float ToroidalWorld::computeLocalTime(const glm::vec3& pos) const {
    glm::vec2 toroidal = cartesianToToroidal(pos);
    return global_time + sin(toroidal.x * 3.0f) + cos(toroidal.y * 3.0f);
}

glm::vec3 ToroidalWorld::getSize() const {
    // Tamanho aproximado em coordenadas cartesianas úteis para renderização
    float width = 2 * (R + r);
    return glm::vec3(width, 2 * r, width);
}