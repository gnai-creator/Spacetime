// ===== asteroid.hpp =====

#pragma once
#include <glm/glm.hpp>
#include <vector>

class Asteroid {
public:
    Asteroid(glm::vec3 pos, glm::vec3 vel, float sz, int gen=0);

    void update(float deltaTime);
    void render(const glm::mat4& view, const glm::mat4& projection) const;
    bool isDestroyed() const;
    glm::vec3 getPosition() const;
    float getSize() const;
    int getGeneration() const;
    std::vector<Asteroid> split() const;

private:
    glm::vec3 position;
    glm::vec3 velocity;
    float size;
    int generation;
    int maxGeneration;
    bool destroyed;
};