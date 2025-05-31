// ===== asteroid.hpp =====

#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "toroidal_world.hpp" // Certifique-se de incluir isso se ToroidalWorld for usado

class Asteroid {
public:
    Asteroid();
    Asteroid(glm::vec3 pos, glm::vec3 vel, float sz, int gen);

    void update(float deltaTime, const ToroidalWorld& world);
    void render(const glm::mat4& view, const glm::mat4& projection) const;

    bool isDestroyed() const { return destroyed; }
    glm::vec3 getPosition() const { return position; }
    float getSize() const { return size; }
    int getGeneration() const { return generation; }
    std::vector<Asteroid> split() const;
    void destroy();
    bool checkCollision(const glm::vec3& point, float radius) const;

private:
    glm::vec3 position;
    glm::vec3 velocity;
    float size;
    int generation;
    bool destroyed = false;
};
