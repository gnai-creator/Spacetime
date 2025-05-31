// ===== asteroid.cpp =====

#include "asteroid.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "renderer.hpp"

Asteroid::Asteroid(glm::vec3 pos, glm::vec3 vel, float sz, int gen)
    : position(pos), velocity(vel), size(sz), generation(gen), destroyed(false) {
    if (generation == 0) maxGeneration = 2;
    else maxGeneration = generation;
}

void Asteroid::update(float deltaTime) {
    position += velocity * deltaTime;
    // Simples checagem de "destruir" (exemplo: posição Y negativa)
    if (position.y < -10.0f) destroyed = true;
}

void Asteroid::render(const glm::mat4& view, const glm::mat4& projection) const {
    Renderer::drawSphere(position, size, view, projection);
}

bool Asteroid::isDestroyed() const {
    return destroyed;
}

glm::vec3 Asteroid::getPosition() const {
    return position;
}

float Asteroid::getSize() const {
    return size;
}

int Asteroid::getGeneration() const {
    return generation;
}

std::vector<Asteroid> Asteroid::split() const {
    std::vector<Asteroid> children;
    if (generation >= maxGeneration) return children;

    int numFragments = 3 + rand() % 3; // 3 a 5 fragmentos
    for (int i = 0; i < numFragments; ++i) {
        glm::vec3 randomDir = glm::normalize(glm::vec3(
            ((rand() % 200) - 100) / 100.0f,
            ((rand() % 200) - 100) / 100.0f,
            ((rand() % 200) - 100) / 100.0f
        ));
        glm::vec3 newVel = velocity + randomDir * 2.0f;
        children.emplace_back(position, newVel, size * 0.5f, generation + 1);
    }
    return children;
}