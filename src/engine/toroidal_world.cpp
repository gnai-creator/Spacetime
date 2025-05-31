// ===== toroidal_world.cpp =====
#include "toroidal_world.hpp"

void ToroidalWorld::update() {
    global_time += 0.016f; // assuming 60 FPS
}

// Converte posição cartesiana (x, y, z) para coordenadas toroidais (θ, φ)
glm::vec2 ToroidalWorld::cartesianToToroidal(const glm::vec3& pos) const {
    float theta = atan2(pos.z, pos.x);
    float r = glm::length(glm::vec2(pos.x, pos.z));
    float phi = atan2(pos.y, r - R);

    // Wrap manual
    if (theta < 0.0f) theta += 2.0f * M_PI;
    if (phi < 0.0f)   phi += 2.0f * M_PI;

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

    // Apenas wrap em theta (volta ao redor do toroide maior)
    toroidal.x = fmod(toroidal.x, 2.0f * M_PI);
    if (toroidal.x < 0.0f) toroidal.x += 2.0f * M_PI;

    // Não aplica wrap em phi
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