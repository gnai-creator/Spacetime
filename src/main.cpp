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
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

float fov = 115.0f;
float yaw = -90.0f;    // Inicie olhando para o eixo Z-
float pitch = 0.0f;    // Sem inclinação
float lastX;
float lastY;
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 0.0f); // posição inicial da câmera
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // para onde olha (atualizado pelo mouse-look)
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraSpeed     = 3.0f;
bool firstMouse = true;



float radius = 5.0f;
ToroidalWorld world;
std::vector<Asteroid> asteroids;
float asteroidSpawnTimer = 0.0f;
Spaceship ship;
glm::vec3 cameraTarget = ship.getPosition() + ship.getForward() * 2.0f;
HUD hud;


void spawnRandomAsteroid() {
    glm::vec3 offset = glm::sphericalRand(60.0f);
    glm::vec3 pos = ship.getPosition() + offset;
    glm::vec3 vel = glm::normalize(ship.getPosition() - pos) * (2.5f + glm::linearRand(0.0f, 1.0f));
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

void processInput(GLFWwindow* window, float deltaTime, glm::mat4 view) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Processa movimento da nave
    ship.processInput(window, deltaTime);
    
    // Atualiza câmera para seguir a nave
    cameraPos = ship.getPosition() - ship.getForward() * 3.0f; // 3 unidades atrás
    cameraFront = ship.getForward(); // Mesma direção da nave
    
    // Atualiza a view matrix
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, glm::vec3(0.0f, 1.0f, 0.0f));
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 90.0f) fov = 90.0f;
}

// Callback para mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Y invertido

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    // Limita o pitch para não virar a câmera do avesso
    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    
    cameraFront = glm::normalize(front);
    ship.setForward(cameraFront); // Nave e câmera compartilham a mesma direção
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

    ship.setHUD(&hud);  // função que você criará no próximo passo
    // float x = world.getSizeX();
    // ship.setPosition(glm::vec3(100.0f, 0.0f, 0.0f));  // Um ponto longe do centro

    if (!glfwInit()) return -1;

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    int screenWidth = mode->width;
    int screenHeight = mode->height;

    window = glfwCreateWindow(screenWidth, screenHeight, "Toroid Engine", monitor, NULL);
    lastX = screenWidth / 2.0f;
    lastY = screenHeight / 2.0f;
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
    // No início do main(), após criar a nave:
    // Posição inicial
    // No main(), após criar a nave:
    ship.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    ship.setForward(glm::vec3(0.0f, 0.0f, -1.0f));
    cameraPos = ship.getPosition() - ship.getForward() * 3.0f;
    cameraFront = ship.getForward();
    // Renderer::initBullets();
    // Renderer::initShip();
    for (int i = 0; i < 5; ++i) {
        spawnRandomAsteroid();
    }

    int width = mode->width;
    int height = mode->height;
    glm::mat4 projection;
    glm::mat4 view;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        asteroidSpawnTimer += deltaTime;

       
        world.update();
        

     
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

        if (asteroidSpawnTimer >= 3.0f) {
            spawnRandomAsteroid();
            asteroidSpawnTimer = 0.0f;
        }

        
        
         // Atualiza dimensões (sem redeclarar)
        glfwGetFramebufferSize(window, &width, &height);
        
        // Previne divisão por zero
        if (height == 0) height = 1;
        
        // Atualiza projeção (usando a variável já declarada)
        projection = glm::perspective(
            glm::radians(fov),
            (float)width/(float)height,
            0.1f,  // near plane
            100.0f // far plane
        );
        
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        // Atualiza viewport
        glViewport(0, 0, width, height);
    
        // Atualize a view:
    

        processInput(window, deltaTime, view);
        // Exemplo: posiciona a nave SEMPRE na frente da câmera
        float distanceFromCamera = -2.0f;
        // ship.position = cameraPos + cameraFront * distanceFromCamera;

// Bullets também usam cameraPos e cameraFront ao nascer!

        // glm::vec3 pos = cameraPos + cameraFront * distanceFromCamera;
        // ship.setPosition(pos);

        ship.updateBullets(deltaTime, world );
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



        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
       // Ajuste o near plane para 0.01f para objetos próximos não desaparecerem
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.01f, 100.0f);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ship.update(world, deltaTime);
        hud.showDebugInfo(
            ship.getPosition(),                      // precisa adicionar esse método
            ship.getWrappedDebugPos(),
            ship.getForward(),
            world.cartesianToToroidal(ship.getWrappedDebugPos()),
            world.getSizeX(), world.getSizeY()
        );

        // No loop principal, antes de Renderer::draw()
        std::cout << "Ship position: " << ship.getPosition().x << ", " 
                << ship.getPosition().y << ", " << ship.getPosition().z << std::endl;
        std::cout << "Ship forward: " << ship.getForward().x << ", "
                << ship.getForward().y << ", " << ship.getForward().z << std::endl;
        glm::vec3 cameraForward = glm::normalize(cameraTarget - cameraPos);
        glm::vec3 cameraUp = glm::vec3(0, 1, 0); // Y para cima (padrão)

        Renderer::draw(world, ship, view, projection, cameraPos,cameraTarget, cameraForward, cameraUp);
        for (auto& asteroid : asteroids) {
            asteroid.render(view, projection);
        }
        // ... outros desenhos ...

       

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
