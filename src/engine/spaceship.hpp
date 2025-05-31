// ===== spaceship.hpp =====
#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "world.hpp"

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
    glm::vec3 position = glm::vec3(50.0f);
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 acceleration = glm::vec3(0.0f);
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    float damping = 0.98f;

    std::vector<Bullet> bullets;
    std::vector<glm::vec3> tailPositions;
    std::vector<Particle> particles;

    void update(const World& world, float deltaTime);
    void processInput(GLFWwindow* window);
    void shoot();
    void updateBullets(float deltaTime);
    void applyCameraDirection(const glm::vec3& dir, const glm::vec3& right);
};