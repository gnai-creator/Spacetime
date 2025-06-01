// toroidal_world.cpp
#include "toroidal_world.hpp"
#include <glm/gtc/constants.hpp>
#include <glm/gtx/norm.hpp>
#include <cmath>
#include <random>

void ToroidalWorld::update() {
    global_time += 0.016f; // assuming 60 FPS
}

// Converte posição cartesiana (x, y, z) para coordenadas toroidais (θ, φ)
glm::vec2 ToroidalWorld::cartesianToToroidal(const glm::vec3& pos) const {
    float theta = atan2(pos.z, pos.x);
    glm::vec3 local = pos - glm::vec3(R * cos(theta), 0.0f, R * sin(theta));
    float phi = atan2(local.y, glm::length(glm::vec2(local.x, local.z)));

    if (theta < 0.0f) theta += glm::two_pi<float>();
    if (phi < 0.0f)   phi += glm::two_pi<float>();

    return glm::vec2(theta, phi);
}

// Converte coordenadas toroidais (θ, φ) para posição cartesiana (x, y, z)
glm::vec3 ToroidalWorld::toroidalToCartesian(float theta, float phi) const {
    float x = (R + r * cos(phi)) * cos(theta);
    float z = (R + r * cos(phi)) * sin(theta);
    float y = r * sin(phi);
    return glm::vec3(x, y, z);
}

// Aplica wrap toroidal na posição para manter a continuidade espacial
glm::vec3 ToroidalWorld::wrapToroidalPosition(const glm::vec3& position) const {
    glm::vec2 toroidal = cartesianToToroidal(position);

    toroidal.x = fmod(toroidal.x, glm::two_pi<float>());
    if (toroidal.x < 0.0f) toroidal.x += glm::two_pi<float>();

    toroidal.y = fmod(toroidal.y, glm::two_pi<float>());
    if (toroidal.y < 0.0f) toroidal.y += glm::two_pi<float>();

    return toroidalToCartesian(toroidal.x, toroidal.y);
}


glm::vec3 ToroidalWorld::wrapToroidalPositionWithOffset(const glm::vec3& pos, float thetaOffset, float phiOffset) const {
    glm::vec2 toroidal = cartesianToToroidal(pos);

    toroidal.x = fmod(toroidal.x + thetaOffset, glm::two_pi<float>());
    if (toroidal.x < 0.0f) toroidal.x += glm::two_pi<float>();

    toroidal.y = fmod(toroidal.y + phiOffset, glm::two_pi<float>());
    if (toroidal.y < 0.0f) toroidal.y += glm::two_pi<float>();

    return toroidalToCartesian(toroidal.x, toroidal.y);
}

float ToroidalWorld::computeLocalTime(const glm::vec3& pos) const {
    glm::vec2 toroidal = cartesianToToroidal(pos);
    return global_time + sin(toroidal.x * 3.0f) + cos(toroidal.y * 3.0f);
}

glm::vec3 ToroidalWorld::getSize() const {
    float width = 2 * (R + r);
    return glm::vec3(width, 2 * r, width);
}

void ToroidalWorld::initAsteroids(int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (int i = 0; i < count; ++i) {
        glm::vec3 pos = toroidalToCartesian(dist(gen) * glm::two_pi<float>(), dist(gen) * glm::two_pi<float>());
        glm::vec3 vel = glm::normalize(glm::vec3(dist(gen), dist(gen), dist(gen))) * 2.0f;
        asteroids.emplace_back(pos, vel, 1.0f, 0);
    }
}
