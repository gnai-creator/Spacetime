// renderer.hpp

#pragma once
#include <glm/glm.hpp>
#include "toroidal_world.hpp"
#include "spaceship.hpp"
#include "asteroid.hpp"

class Renderer {
    public:
    static bool init();
    static GLuint loadShaders();
    static GLuint vaoBullet;
    static GLuint vboBullet;
    static GLuint shipVAO;
    static GLuint shipVBO;
    static void initBullets();
    static void initShip();
    static void drawGrid(float worldSizeX, float worldSizeZ, int divisions);
    static void drawSphere(const glm::vec3& position, float radius, const glm::mat4& view, const glm::mat4& projection);
    static void drawAsteroids(const ToroidalWorld& world, const std::vector<Asteroid>& asteroids);
    static void draw(const ToroidalWorld& world, const Spaceship& ship, const glm::mat4& view, const glm::mat4& projection,const glm::vec3& cameraPos);

private:
    static GLuint shaderProgram;
    static GLuint asteroidVAO;
    static GLuint asteroidVBO;
    static GLuint asteroidEBO;
    static GLsizei asteroidIndexCount;
    static void loadAsteroidMesh();  // novo método privado
};
