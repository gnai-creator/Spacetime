// ===== main.cpp =====

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <cstdlib>
#include <ctime>

#include "hud.hpp"
#include "spaceship.hpp"
#include "toroidal_world.hpp"
#include "renderer.hpp"
#include "asteroid.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <vector>
#include <algorithm>

GLFWwindow* window;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float yaw = -90.0f, pitch = 0.0f;
float fov = 115.0f;
double lastX = 400, lastY = 300;
bool firstMouse = true;

float radius = 5.0f;
Spaceship ship;
ToroidalWorld world;
std::vector<Asteroid> asteroids;
float asteroidSpawnTimer = 0.0f;

void spawnRandomAsteroid() {
    glm::vec3 offset = glm::sphericalRand(60.0f);
    glm::vec3 pos = ship.getPosition() + offset;
    glm::vec3 vel = glm::normalize(ship.getPosition() - pos) * (1.5f + glm::linearRand(0.0f, 1.0f));
    asteroids.emplace_back(pos, vel, 1.0f, 0);
}

void breakAsteroid(const Asteroid& original, int generation) {
    if (generation >= 2) return;
    int count = rand() % 4; // 0 a 3 novos asteroides
    for (int i = 0; i < count; ++i) {
        glm::vec3 offset = glm::ballRand(1.0f);
        glm::vec3 newPos = original.getPosition() + offset;
        glm::vec3 newVel = glm::ballRand(2.0f);
        asteroids.emplace_back(newPos, newVel, 0.5f, generation + 1);
    }
}

void processInput(GLFWwindow* window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    ship.processInput(window, deltaTime);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 90.0f) fov = 90.0f;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    ship.applyRotationFromMouse(xoffset, yoffset);
}

void checarColisoes(std::vector<Bullet>& bullets, std::vector<Asteroid>& asteroids) {
    for (auto& asteroid : asteroids) {
        if (asteroid.isDestroyed()) continue;

        for (auto& bullet : bullets) {
            if (!bullet.active) continue;

            float dist = glm::distance(asteroid.getPosition(), bullet.position);
            if (dist < asteroid.getSize()) {
                asteroid.destroy();     // método precisa existir
                bullet.active = false; // bullet precisa ter o campo `.active`
                break;
            }
        }
    }

    // Remove balas inativas
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                 [](const Bullet& b) { return !b.active; }),
                  bullets.end());
}


int main() {
    srand(time(0));

    if (!glfwInit()) return -1;

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    int screenWidth = mode->width;
    int screenHeight = mode->height;

    window = glfwCreateWindow(screenWidth, screenHeight, "Toroid Engine", monitor, NULL);
    if (!window) return -1;

    glfwMakeContextCurrent(window);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    if (!Renderer::init()) return -1;

    for (int i = 0; i < 5; ++i) {
        spawnRandomAsteroid();
    }

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        asteroidSpawnTimer += deltaTime;

        processInput(window, deltaTime);
        world.update();
        ship.update(world, deltaTime);

        std::vector<Bullet>& bullets = ship.getBullets();
        for (auto& asteroid : asteroids) {
            if (asteroid.isDestroyed()) continue;
            for (auto& bullet : bullets) {
                if (!bullet.active) continue;
                float dist = glm::distance(asteroid.getPosition(), bullet.position);
                if (dist < asteroid.getSize()) {
                    asteroid.destroy();
                    bullet.active = false;
                    break;
                }
            }
        }
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.active; }), bullets.end());

        std::vector<Asteroid> updatedAsteroids;
        for (auto& asteroid : asteroids) {
            asteroid.update(deltaTime, world);

            float dist = glm::distance(asteroid.getPosition(), ship.getPosition());
            if (dist < asteroid.getSize() + 1.0f) {
                std::cout << "[CRITICAL] Nave colidiu com asteroide!\n";
                // Aqui poderia aplicar lógica de destruição da nave
            }

            if (asteroid.isDestroyed()) {
                breakAsteroid(asteroid, asteroid.getGeneration());
            } else {
                updatedAsteroids.push_back(asteroid);
            }
        }
        asteroids = updatedAsteroids;

        if (asteroidSpawnTimer >= 5.0f) {
            spawnRandomAsteroid();
            asteroidSpawnTimer = 0.0f;
        }

        glm::vec3 cameraOffset = -ship.getForward() * radius + glm::vec3(0.0f, 1.5f, 0.0f);
        glm::vec3 cameraPos = ship.getPosition() + cameraOffset;
        glm::vec3 cameraTarget = ship.getPosition() + ship.getForward() * 2.0f;

        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 100.0f);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        HUD::render(ship);

        Renderer::draw(world, ship, view, projection);
        for (auto& asteroid : asteroids) {
            asteroid.render(view, projection);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
