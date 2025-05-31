// renderer.hpp

#pragma once
#include <glm/glm.hpp>
#include "toroidal_world.hpp"
#include "spaceship.hpp"

class Renderer {
public:
    static bool init();
    static void drawGrid(float worldSizeX, float worldSizeZ, int divisions);
    static void draw(const ToroidalWorld& world, const Spaceship& ship, const glm::mat4& view, const glm::mat4& projection);
    static void drawSphere(const glm::vec3& position, float radius, const glm::mat4& view, const glm::mat4& projection);

private:
    static GLuint loadShaders();
};