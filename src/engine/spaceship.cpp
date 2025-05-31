// ===== spaceship.cpp =====
#include "spaceship.hpp"
#include <glm/gtc/constants.hpp>
#include <algorithm>

Spaceship::Spaceship()
    : position(50.0f, 50.0f, 50.0f), velocity(0.0f), acceleration(25.0f),
      forward(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f), yaw(-90.0f), pitch(0.0f),
      modelMatrix(1.0f) {}

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
    float thrust = 4.0 * acceleration / glm::pi<float>();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        velocity += forward * thrust * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        velocity -= forward * thrust * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        velocity -= glm::normalize(glm::cross(forward, glm::vec3(0,1,0))) * thrust * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        velocity += glm::normalize(glm::cross(forward, glm::vec3(0,1,0))) * thrust * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        shoot();
}

void Spaceship::update(const ToroidalWorld& world, float deltaTime) {
    position += velocity * deltaTime;
    velocity *= 0.98f;  // Aplicar atrito
    position = world.wrapToroidalPosition(position);  // mundo toroidal

    updateBullets(deltaTime);

    tailPositions.push_back(position);
    if (tailPositions.size() > 30)
        tailPositions.erase(tailPositions.begin());

    modelMatrix = glm::translate(glm::mat4(1.0f), position);
    // modelMatrix = glm::rotate(modelMatrix, glm::radians(yaw), glm::vec3(0,1,0)); // ou outro eixo
    modelMatrix *= glm::mat4_cast(glm::quatLookAt(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    modelMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
}


void Spaceship::shoot() {
    Bullet b;
    b.position = position;
    b.velocity = forward * 20.0f;
    bullets.push_back(b);
}


void Spaceship::updateBullets(float deltaTime) {
    for (auto& b : bullets) {
        b.position += b.velocity * deltaTime;
        b.lifetime -= deltaTime;
    }
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return b.lifetime <= 0.0f; }), bullets.end());
}

void Spaceship::applyCameraDirection(const glm::vec3& front, const glm::vec3& right) {
    forward = glm::normalize(front);
}
