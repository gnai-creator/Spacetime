// renderer.cpp
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "renderer.hpp"
#include <iostream>
#include <vector>

static GLuint shaderProgram;

bool Renderer::init() {
    glewExperimental = GL_TRUE;
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        std::cerr << "[ERROR] glewInit() failed: " << glewGetErrorString(glewStatus) << std::endl;
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    shaderProgram = loadShaders();
    return true;
}

GLuint Renderer::loadShaders() {
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(0.3, 0.8, 1.0, 1.0);
        }
    )";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void Renderer::drawGrid(float worldSizeX, float worldSizeZ, int divisions) {
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINES);

    glColor3f(0.3f, 0.3f, 0.3f);

    float dx = worldSizeX / divisions;
    float dz = worldSizeZ / divisions;

    for (int i = 0; i <= divisions; ++i) {
        float x = -worldSizeX / 2 + i * dx;
        glVertex3f(x, 0.0f, -worldSizeZ / 2);
        glVertex3f(x, 0.0f,  worldSizeZ / 2);
    }

    for (int i = 0; i <= divisions; ++i) {
        float z = -worldSizeZ / 2 + i * dz;
        glVertex3f(-worldSizeX / 2, 0.0f, z);
        glVertex3f( worldSizeX / 2, 0.0f, z);
    }

    glEnd();
}

void Renderer::drawSphere(const glm::vec3& position, float radius, const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);

    static std::vector<glm::vec3> sphereVertices;
    static bool initialized = false;

    if (!initialized) {
        const int stacks = 10, slices = 10;
        for (int i = 0; i <= stacks; ++i) {
            float phi = glm::pi<float>() * float(i) / float(stacks);
            for (int j = 0; j <= slices; ++j) {
                float theta = glm::two_pi<float>() * float(j) / float(slices);
                float x = sin(phi) * cos(theta);
                float y = cos(phi);
                float z = sin(phi) * sin(theta);
                sphereVertices.emplace_back(x, y, z);
            }
        }
        initialized = true;
    }

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(glm::vec3), sphereVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc  = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc  = glGetUniformLocation(shaderProgram, "projection");

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, glm::vec3(radius));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glDrawArrays(GL_POINTS, 0, sphereVertices.size());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Renderer::draw(const ToroidalWorld& world, const Spaceship& ship, const glm::mat4& view, const glm::mat4& projection) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);

    float triangle[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.7f, 0.0f
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc  = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc  = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    for (int i = 0; i < 9; ++i) {
        float thetaOffset = static_cast<float>(i % 3 - 1) * glm::two_pi<float>();
        float phiOffset = static_cast<float>(i / 3 - 1) * glm::two_pi<float>();

        glm::vec3 pos = world.wrapToroidalPositionWithOffset(ship.getPosition(), thetaOffset, phiOffset);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        model *= glm::mat4_cast(glm::quatLookAt(ship.getForward(), glm::vec3(0.0f, 1.0f, 0.0f)));
        model *= glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    for (const auto& bullet : ship.getBullets()) {
        if (!bullet.active) continue;
        glm::mat4 model = glm::translate(glm::mat4(1.0f), bullet.position);
        model = glm::scale(model, glm::vec3(0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    if (!ship.getTailPositions().empty()) {
        std::vector<float> tailVertices;
        for (const auto& p : ship.getTailPositions()) {
            tailVertices.push_back(p.x);
            tailVertices.push_back(p.y);
            tailVertices.push_back(p.z);
        }

        GLuint tailVBO;
        glGenBuffers(1, &tailVBO);
        glBindBuffer(GL_ARRAY_BUFFER, tailVBO);
        glBufferData(GL_ARRAY_BUFFER, tailVertices.size() * sizeof(float), tailVertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_LINE_STRIP, 0, ship.getTailPositions().size());

        glDisableVertexAttribArray(0);
        glDeleteBuffers(1, &tailVBO);
    }

    if (!ship.getParticles().empty()) {
        std::vector<float> particleVertices;
        for (const auto& p : ship.getParticles()) {
            particleVertices.push_back(p.x);
            particleVertices.push_back(p.y);
            particleVertices.push_back(p.z);
        }

        GLuint particleVBO, particleVAO;
        glGenVertexArrays(1, &particleVAO);
        glGenBuffers(1, &particleVBO);

        glBindVertexArray(particleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
        glBufferData(GL_ARRAY_BUFFER, particleVertices.size() * sizeof(float), particleVertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glPointSize(3.0f);
        glDrawArrays(GL_POINTS, 0, ship.getParticles().size());

        glDisableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glDeleteBuffers(1, &particleVBO);
        glDeleteVertexArrays(1, &particleVAO);
    }

    drawSphere(ship.getWrappedDebugPos(), 0.15f, view, projection);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}
