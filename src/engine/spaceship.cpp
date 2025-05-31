// ===== spaceship.cpp =====
#include "spaceship.hpp"

void Spaceship::update(const World& world, float deltaTime) {
    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;
    velocity *= damping;

    position = world.wrapPosition(position);

    modelMatrix = glm::translate(glm::mat4(1.0f), position);
    modelMatrix *= glm::mat4_cast(glm::quatLookAt(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    modelMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0)); // <- esta linha



    tailPositions.push_back(position);
    if (tailPositions.size() > 50)
        tailPositions.erase(tailPositions.begin());

    particles.push_back({ position, 1.0f });
    for (auto& p : particles)
        p.lifetime -= deltaTime;

    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const Particle& p) { return p.lifetime <= 0.0f; }),
        particles.end());

    updateBullets(deltaTime);
    acceleration = glm::vec3(0.0f);
}

void Spaceship::processInput(GLFWwindow* window) {
    const float thrust = 8.0f;
    const float strafe = 4.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        acceleration += forward * thrust;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        acceleration -= forward * thrust;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        acceleration -= glm::cross(forward, glm::vec3(0, 1, 0)) * strafe;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        acceleration += glm::cross(forward, glm::vec3(0, 1, 0)) * strafe;
}

void Spaceship::shoot() {
    Bullet b;
    b.position = position + forward * 1.0f;
    b.velocity = forward * 20.0f;
    bullets.push_back(b);
}

void Spaceship::updateBullets(float deltaTime) {
    for (auto& b : bullets) {
        b.position += b.velocity * deltaTime;
        b.lifetime -= deltaTime;
    }
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return b.lifetime <= 0.0f; }),
        bullets.end());
}

void Spaceship::applyCameraDirection(const glm::vec3& dir, const glm::vec3& right) {
    forward = glm::normalize(dir);
}
