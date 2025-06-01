// ===== spaceship.cpp =====
#include "spaceship.hpp"
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <cmath>  // necessário para std::isfinite

Spaceship::Spaceship()
    : position(50.0f, 50.0f, 50.0f), velocity(1.0f), acceleration(1.78f),
      forward(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f), yaw(-90.0f), pitch(0.0f),
      modelMatrix(1.0f), thrust(1.78f) {}

void Spaceship::applyRotationFromMouse(float xoffset, float yoffset) {
    float sensitivity = 0.1f;
    yaw += xoffset * sensitivity;
    pitch += yoffset * sensitivity;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 dir;
    dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    forward = glm::normalize(dir);
}

void Spaceship::processInput(GLFWwindow* window, float deltaTime) {
    thrust += 4.0f * acceleration / glm::pi<float>();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        velocity += forward * thrust * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        velocity -= forward * thrust * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        velocity -= glm::normalize(glm::cross(forward, glm::vec3(0,1,0))) * thrust * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        velocity += glm::normalize(glm::cross(forward, glm::vec3(0,1,0))) * thrust * deltaTime;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        shoot();
    


}

void Spaceship::update(const ToroidalWorld& world, float deltaTime) {
    originalPosition = position;

    position += velocity * deltaTime;
    velocity *= 0.99f;

    constexpr float velocityLimit = 1e5f;
    if (!std::isfinite(velocity.x) || 
        !std::isfinite(velocity.y) || 
        !std::isfinite(velocity.z) ||
        glm::length(velocity) > velocityLimit)
    {
        velocity = glm::normalize(velocity) * velocityLimit;
    }

    // Aqui aplica corretamente o wrap
    position = world.wrapToroidalPosition(position);
    wrappedDebugPos = position;

    updateBullets(deltaTime, world);

    tailPositions.push_back(position);
    if (tailPositions.size() > 30)
        tailPositions.erase(tailPositions.begin());

    forward = glm::normalize(forward);

    modelMatrix = glm::translate(glm::mat4(1.0f), position);
    modelMatrix *= glm::mat4_cast(glm::quatLookAt(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    modelMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
}



void Spaceship::shoot() {
    Bullet b;
    b.position = position;
    b.velocity = velocity + forward * 20.0f;
    b.lifetime = 5.0f;
    bullets.push_back(b);
}

void Spaceship::updateBullets(float deltaTime, const ToroidalWorld& world) {
    for (auto& bullet : bullets) {
        bullet.position += bullet.velocity * deltaTime;
        bullet.position = world.wrapToroidalPosition(bullet.position);
        bullet.lifetime -= deltaTime;
        if (bullet.lifetime <= 0.0f) bullet.active = false;
    }

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return !b.active; }), bullets.end());
}



void Spaceship::applyCameraDirection(const glm::vec3& front, const glm::vec3& right) {
    forward = glm::normalize(front);
}

const glm::vec3& Spaceship::getPosition() const {
    return position;
}

const glm::vec3& Spaceship::getForward() const {
    return forward;
}

const glm::mat4& Spaceship::getModelMatrix() const {
    return modelMatrix;
}

const std::vector<Bullet>& Spaceship::getBullets() const {
    return bullets;
}

std::vector<Bullet>& getBullets() {
    return getBullets();
}

const std::vector<glm::vec3>& Spaceship::getTailPositions() const {
    return tailPositions;
}
const std::vector<glm::vec3>& Spaceship::getParticles() const {
    return particles;
}
