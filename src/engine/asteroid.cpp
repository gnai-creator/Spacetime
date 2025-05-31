// ===== asteroid.cpp =====
#include "asteroid.hpp"
#include "renderer.hpp"
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/matrix_transform.hpp>

Asteroid::Asteroid()
    : position(0.0f), velocity(0.0f), size(1.0f), generation(0), destroyed(false) {}

Asteroid::Asteroid(glm::vec3 pos, glm::vec3 vel, float sz, int gen)
    : position(pos), velocity(vel), size(sz), generation(gen), destroyed(false) {}

void Asteroid::update(float deltaTime, const ToroidalWorld& world) {
    position += velocity * deltaTime;
    position = world.wrapToroidalPosition(position);
}

void Asteroid::render(const glm::mat4& view, const glm::mat4& projection) const {
    Renderer::drawSphere(position, size, view, projection);
}

std::vector<Asteroid> Asteroid::split() const {
    std::vector<Asteroid> fragments;
    if (generation < 2) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

        for (int i = 0; i < 2; ++i) {
            glm::vec3 newVel = glm::normalize(glm::vec3(dist(gen), dist(gen), dist(gen))) * glm::length(velocity);
            float newSize = size * 0.5f;
            fragments.emplace_back(position, newVel, newSize, generation + 1);
        }
    }
    return fragments;
}

void Asteroid::destroy() {
    destroyed = true;
}

bool Asteroid::checkCollision(const glm::vec3& point, float radius) const {
    return !destroyed && glm::distance(position, point) < (size + radius);
}
