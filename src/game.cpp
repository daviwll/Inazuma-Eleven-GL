#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "constants.hpp"
#include "field.hpp"
#include "game.hpp"
#include "game_logic.hpp"
#include "input.hpp"
#include "player.hpp"
#include "stadium.hpp"

#include <vector>

int runGame() {
    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Inazuma Eleven GL", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    using namespace Constants;

    Field field;
    Stadium stadium;
    Ball ball{0.0f, 0.0f, 0.005f, 0.002f, 0.992f, nullptr};
    Score score{0, 0};
    InputState inputState{0.0f, 0.0f, false, false};

    float speed = 0.0025f;

    std::vector<Player> team1;
    team1.push_back(Player(-FIELD_BOUNDARY_X, 0.00f, speed, -1, PlayerRole::GOALKEEPER));
    team1.push_back(Player(-0.65f, 0.25f, speed, -1, PlayerRole::DEFENDER));
    team1.push_back(Player(-0.65f, -0.25f, speed, -1, PlayerRole::DEFENDER));
    team1.push_back(Player(-0.60f, 0.50f, speed, -1, PlayerRole::DEFENDER));
    team1.push_back(Player(-0.60f, -0.50f, speed, -1, PlayerRole::DEFENDER));
    team1.push_back(Player(-0.35f, 0.00f, speed, -1, PlayerRole::MIDFIELDER));
    team1.push_back(Player(-0.35f, 0.30f, speed, -1, PlayerRole::MIDFIELDER));
    team1.push_back(Player(-0.35f, -0.30f, speed, -1, PlayerRole::MIDFIELDER));
    team1.push_back(Player(-0.10f, 0.00f, speed, -1, PlayerRole::ATTACKER));
    team1.push_back(Player(-0.10f, 0.40f, speed, -1, PlayerRole::ATTACKER));
    team1.push_back(Player(-0.10f, -0.40f, speed, -1, PlayerRole::ATTACKER));

    std::vector<Player> team2;
    team2.push_back(Player(FIELD_BOUNDARY_X, 0.00f, speed, 1, PlayerRole::GOALKEEPER));
    team2.push_back(Player(0.65f, 0.25f, speed, 1, PlayerRole::DEFENDER));
    team2.push_back(Player(0.65f, -0.25f, speed, 1, PlayerRole::DEFENDER));
    team2.push_back(Player(0.60f, 0.50f, speed, 1, PlayerRole::DEFENDER));
    team2.push_back(Player(0.60f, -0.50f, speed, 1, PlayerRole::DEFENDER));
    team2.push_back(Player(0.35f, 0.00f, speed, 1, PlayerRole::MIDFIELDER));
    team2.push_back(Player(0.35f, 0.30f, speed, 1, PlayerRole::MIDFIELDER));
    team2.push_back(Player(0.35f, -0.30f, speed, 1, PlayerRole::MIDFIELDER));
    team2.push_back(Player(0.10f, 0.00f, speed, 1, PlayerRole::ATTACKER));
    team2.push_back(Player(0.10f, 0.40f, speed, 1, PlayerRole::ATTACKER));
    team2.push_back(Player(0.10f, -0.40f, speed, 1, PlayerRole::ATTACKER));

    float lastFrameTime = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window)) {
        float currentFrameTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glClear(GL_COLOR_BUFFER_BIT);

        processInput(window, inputState);
        updateBall(ball, score);
        updateTeam(team1, team2, ball, true, deltaTime, inputState);
        updateTeam(team2, team1, ball, false, deltaTime, inputState);

        stadium.render();
        stadium.renderScoreboard(score.left, score.right);
        field.render();

        glColor3f(1.0f, 1.0f, 1.0f);
        glPointSize(4.0f);
        glBegin(GL_POINTS);
        glVertex2f(ball.x, ball.y);
        glEnd();

        for (auto& p : team1) {
            p.render();
            p.renderPowerBar();
        }

        for (auto& p : team2) {
            p.render();
            p.renderPowerBar();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
