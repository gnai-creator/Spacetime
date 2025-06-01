#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "asteroid.hpp"

class ToroidalWorld {
public:
    float R = 30.0f;
    float r = 15.0f;
    float global_time = 0.0f;
   
    void update();
    glm::vec2 cartesianToToroidal(const glm::vec3& pos) const;
    glm::vec3 toroidalToCartesian(float theta, float phi) const;
    glm::vec3 wrapToroidalPosition(const glm::vec3& pos) const;
    glm::vec3 wrapToroidalPositionWithOffset(const glm::vec3& pos, float thetaOffset, float phiOffset) const;

    float computeLocalTime(const glm::vec3& pos) const;

    float getSizeX() const { return 2.0f * glm::pi<float>() * R; }
    float getSizeY() const { return 2.0f * glm::pi<float>() * r; }
    glm::vec3 getSize() const; // ← apenas a *declaração*!

    void initAsteroids(int count);
    void addAsteroid(const Asteroid& a) { asteroids.push_back(a); }
    const std::vector<Asteroid>& getAsteroids() const { return asteroids; }
    std::vector<Asteroid>& getAsteroids() { return asteroids; } // se quiser modificar


private:
    std::vector<Asteroid> asteroids;
};
