// hud.hpp
#pragma once

#include <glm/glm.hpp>
#include <string>

class HUD {
public:
    void showDebugInfo(const glm::vec3& posBeforeWrap,
                       const glm::vec3& posAfterWrap,
                       const glm::vec3& velocity,
                       const glm::vec2& toroidalAngles,
                       float R, float r);
};