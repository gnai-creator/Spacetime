#pragma once
#include <glm/glm.hpp>
#include "world.hpp"
#include "spaceship.hpp"

class Renderer {
public:
    static bool init();
    static void draw(const World& world, const Spaceship& ship, const glm::mat4& view, const glm::mat4& projection);

private:
    static GLuint loadShaders();
};
