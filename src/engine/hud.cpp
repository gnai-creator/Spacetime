// hud.cpp
#include "hud.hpp"
#include <sstream>
#include <iomanip>
#include "imgui.h"

void HUD::showDebugInfo(const glm::vec3& posBeforeWrap,
                        const glm::vec3& posAfterWrap,
                        const glm::vec3& velocity,
                        const glm::vec2& toroidalAngles,
                        float R, float r) {
    ImGui::Begin("Debug Info");
    ImGui::Text("POS (pre-wrap): (%.2f, %.2f, %.2f)", posBeforeWrap.x, posBeforeWrap.y, posBeforeWrap.z);
    ImGui::Text("POS (wrapped):  (%.2f, %.2f, %.2f)", posAfterWrap.x, posAfterWrap.y, posAfterWrap.z);
    ImGui::Text("VEL:            (%.2f, %.2f, %.2f)", velocity.x, velocity.y, velocity.z);
    ImGui::Text("THETA / PHI:    (%.2f, %.2f)", toroidalAngles.x, toroidalAngles.y);
    ImGui::Text("R / r:          (%.2f, %.2f)", R, r);
    ImGui::End();
}
