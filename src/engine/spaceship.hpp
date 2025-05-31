// ===== spaceship.hpp =====
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <GLFW/glfw3.h>
#include "toroidal_world.hpp"

struct Bullet {
    glm::vec3 position;
    glm::vec3 velocity;
    float lifetime = 2.0f;
};

struct Particle {
    glm::vec3 position;
    float lifetime = 1.0f;
};

class Spaceship {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 forward;
    glm::vec3 up;
    glm::mat4 modelMatrix;
    float acceleration = 5.0f;
    float yaw, pitch;

    std::vector<Bullet> bullets;
    std::vector<glm::vec3> tailPositions;
    std::vector<Particle> particles;

    Spaceship();

    void update(const ToroidalWorld& world, float deltaTime);
    void processInput(GLFWwindow* window, float deltaTime);
    void applyRotationFromMouse(float xoffset, float yoffset);
    void shoot();
    void updateBullets(float deltaTime);
    void applyCameraDirection(const glm::vec3& front, const glm::vec3& right);
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getForward() const { return forward; }
    glm::vec3 getVelocity() const { return velocity; }
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }

};