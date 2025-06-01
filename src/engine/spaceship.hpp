// ===== spaceship.hpp =====
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include "toroidal_world.hpp"
#include "hud.hpp"

struct Bullet {
    glm::vec3 position;
    glm::vec3 velocity;
    float lifetime;
    bool active = true;
};

class Spaceship {
public:
    Spaceship();

    void processInput(GLFWwindow* window, float deltaTime);
    void update(const ToroidalWorld& world, float deltaTime);
    void applyRotationFromMouse(float xoffset, float yoffset);
    void applyCameraDirection(const glm::vec3& front, const glm::vec3& right);
    void updateBullets(float deltaTime, const ToroidalWorld& world);
    glm::vec3 getOriginalPosition() const { return originalPosition; }

    const glm::vec3& getPosition() const;
    const glm::vec3& getForward() const;
    const glm::vec3& getVelocity() const { return velocity; }
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }

    const glm::mat4& getModelMatrix() const;

    const std::vector<Bullet>& getBullets() const;
    std::vector<Bullet>& getBullets() { return bullets; }
    const std::vector<glm::vec3>& getTailPositions() const;
    const std::vector<glm::vec3>& getParticles() const;
    glm::vec3 setPosition(glm::vec3 pos) {
        position = pos;
        return position;
    }
    glm::vec3 getWrappedDebugPos() const { return wrappedDebugPos; }
    void setHUD(HUD* hud) { hudPtr = hud; }

private:
    glm::vec3 wrappedDebugPos;
    glm::vec3 position;
    glm::vec3 originalPosition = position;  // salvar antes do wrap
    glm::vec3 velocity;
    glm::vec3 forward;
    glm::vec3 up;
    float yaw;
    float pitch;
    float acceleration;
    float thrust;
    HUD* hudPtr = nullptr;
    std::vector<glm::vec3> particles;
    glm::mat4 modelMatrix;
    std::vector<Bullet> bullets;
    std::vector<glm::vec3> tailPositions;

    void shoot();
    void updateBullets(float deltaTime);
};