// ===== spaceship.cpp =====
#include <GL/glew.h> // Necessário para glUniformMatrix4fv
#include <glm/gtc/type_ptr.hpp> // Necessário para glm::value_ptr
#include <glm/gtc/constants.hpp>
#include "spaceship.hpp"
#include <algorithm>
#include <cmath>  // necessário para std::isfinite
#include <iostream> // necessário para std::cout


Spaceship::Spaceship()
    : position(0.0f, 0.0f, 0.0f), velocity(0.0f), acceleration(1.0f),
      forward(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f), yaw(-90.0f), pitch(0.0f),
      modelMatrix(1.0f), thrust(0.0f) {}  // Note thrust iniciando em 0

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
    thrust = 0.0f; // Reset a cada frame

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        thrust = 2.0f;
        velocity = forward * thrust;
        position += velocity * deltaTime;
    }
    
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        shoot(position + forward * 0.5f, forward, 0.1f); // Dispara um pouco à frente da nave
    }
}

// Corrigindo update_position para manter a spaceship sempre na frente da câmera (sem usar velocity)
void Spaceship::update_position(float deltaTime, const ToroidalWorld& world, const glm::vec3& cameraPos, const glm::vec3& cameraFront, float distanceFromCamera) {
    // Ignora velocity, prende a nave sempre na frente da câmera
    setPosition(cameraPos + cameraFront * distanceFromCamera);

    
    // Caso você queira manter efeito toroide:
    position.x = toroidal_wrap(position.x, world.getSizeX());
    position.y = toroidal_wrap(position.y, world.getSizeY());
}



void Spaceship::update(const ToroidalWorld& world, float deltaTime) {
    // update_position(deltaTime, world);

}



// Ajustando os tiros (bullets) para seguir sempre a direção da câmera (forward atualizado)

void Spaceship::shoot(const glm::vec3& cameraPos, const glm::vec3& cameraFront, float distanceFromCamera) {
    Bullet b;
    b.position = cameraPos + cameraFront * distanceFromCamera;
    b.velocity = cameraFront * 20.0f;
    b.lifetime = 5.0f;
    b.active = true;
    bullets.push_back(b);
}




void Spaceship::updateBullets(float deltaTime, const ToroidalWorld& world) {
    for (auto& bullet : bullets) {
        bullet.position += bullet.velocity * deltaTime;
        bullet.position = world.wrapToroidalPosition(bullet.position);
        bullet.lifetime -= deltaTime;
        if (bullet.lifetime <= 0.0f) bullet.active = false;
    }
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.active; }), bullets.end());
}



void Spaceship::renderBullets(GLuint modelLoc, const glm::vec3& cameraPos) const {
    for (const auto& bullet : bullets) {
        if (!bullet.active) continue;
        glm::vec3 relPos = bullet.position - cameraPos;
        std::cout << "relPos: " << relPos.x << ", " << relPos.y << ", " << relPos.z << std::endl;
        glm::mat4 model = glm::translate(glm::mat4(1.0f), relPos);
        model = glm::scale(model, glm::vec3(0.05f)); // se quiser pequeno
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
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
void Spaceship::setForward(const glm::vec3& newForward) {
    forward = glm::normalize(newForward);
}