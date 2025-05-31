#include "hud.hpp"
#include "imgui.h"

namespace HUD {
    void render(const Spaceship& ship) {
        ImGui::Begin("HUD - Nave");

        glm::vec3 pos = ship.getPosition();
        glm::vec3 vel = ship.getVelocity();

        ImGui::Text("Posição: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
        ImGui::Text("Velocidade: (%.2f, %.2f, %.2f)", vel.x, vel.y, vel.z);
        ImGui::Text("Magnitude: %.2f", glm::length(vel));
        ImGui::Text("Yaw: %.1f", ship.getYaw());
        ImGui::Text("Pitch: %.1f", ship.getPitch());

        ImGui::End();
    }
}
