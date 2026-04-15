#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ball.hpp"
#include "field.hpp"
#include "game.hpp"
#include "game_logic.hpp"
#include "input.hpp"
#include "stadium.hpp"
#include "player.hpp"
#include "constants.hpp"

#include <vector>

int runGame()
{
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Inazuma Eleven GL", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    using namespace Constants;
    Field field; Stadium stadium;
    GameState gameState{2.0f};
    Ball ball{0.0f, 0.0f, 0.005f, 0.002f, 0.98f, nullptr};
    Score score{0, 0};
    InputState inputState{0.0f, 0.0f, false, false, 0.0f, 0.0f};
    const float initialPlayerSpeed = 0.2f;
    std::vector<Player> team1;
    std::vector<Player> team2;

    team1.push_back(Player(-FIELD_BOUNDARY_X,  0.00f, initialPlayerSpeed, -1, PlayerRole::GOALKEEPER));
    team1.push_back(Player(-0.65f,  0.25f, initialPlayerSpeed, -1, PlayerRole::DEFENDER));
    team1.push_back(Player(-0.65f, -0.25f, initialPlayerSpeed, -1, PlayerRole::DEFENDER));
    team1.push_back(Player(-0.60f,  0.50f, initialPlayerSpeed, -1, PlayerRole::DEFENDER));
    team1.push_back(Player(-0.60f, -0.50f, initialPlayerSpeed, -1, PlayerRole::DEFENDER));
    team1.push_back(Player(-0.35f,  0.00f, initialPlayerSpeed, -1, PlayerRole::MIDFIELDER));
    team1.push_back(Player(-0.35f,  0.30f, initialPlayerSpeed, -1, PlayerRole::MIDFIELDER));
    team1.push_back(Player(-0.35f, -0.30f, initialPlayerSpeed, -1, PlayerRole::MIDFIELDER));
    team1.push_back(Player(-0.10f,  0.00f, initialPlayerSpeed, -1, PlayerRole::ATTACKER));
    team1.push_back(Player(-0.10f,  0.40f, initialPlayerSpeed, -1, PlayerRole::ATTACKER));
    team1.push_back(Player(-0.10f, -0.40f, initialPlayerSpeed, -1, PlayerRole::ATTACKER));

    team2.push_back(Player( FIELD_BOUNDARY_X,  0.00f, initialPlayerSpeed,  1, PlayerRole::GOALKEEPER));
    team2.push_back(Player( 0.65f,  0.25f, initialPlayerSpeed,  1, PlayerRole::DEFENDER));
    team2.push_back(Player( 0.65f, -0.25f, initialPlayerSpeed,  1, PlayerRole::DEFENDER));
    team2.push_back(Player( 0.60f,  0.50f, initialPlayerSpeed,  1, PlayerRole::DEFENDER));
    team2.push_back(Player( 0.60f, -0.50f, initialPlayerSpeed,  1, PlayerRole::DEFENDER));
    team2.push_back(Player( 0.35f,  0.00f, initialPlayerSpeed,  1, PlayerRole::MIDFIELDER));
    team2.push_back(Player( 0.35f,  0.30f, initialPlayerSpeed,  1, PlayerRole::MIDFIELDER));
    team2.push_back(Player( 0.35f, -0.30f, initialPlayerSpeed,  1, PlayerRole::MIDFIELDER));
    team2.push_back(Player( 0.10f,  0.00f, initialPlayerSpeed,  1, PlayerRole::ATTACKER));
    team2.push_back(Player( 0.10f,  0.40f, initialPlayerSpeed,  1, PlayerRole::ATTACKER));
    team2.push_back(Player( 0.10f, -0.40f, initialPlayerSpeed,  1, PlayerRole::ATTACKER));

    resetGame(ball, team1, team2, gameState, 1);

    float lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
        stadium.update(deltaTime);

        if (gameState.kickoffTimer > 0.0f) {
            gameState.kickoffTimer -= deltaTime;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window, inputState, gameState.kickoffTimer <= 0.0f);
        int scorerSide = updateBall(ball, score, team1, team2, gameState);
        if (scorerSide != 0) {
            stadium.triggerCrowdCelebration(scorerSide);
        }
        updateTeam(team1, team2, ball, true, deltaTime, inputState, gameState);
        updateTeam(team2, team1, ball, false, deltaTime, inputState, gameState);

        stadium.render(); stadium.renderScoreboard(score.left, score.right);
        field.render();

        glColor3f(1.0f, 1.0f, 1.0f); glPointSize(4.0f);
        glBegin(GL_POINTS); glVertex2f(ball.x, ball.y); glEnd();
        for(auto& p : team1) { p.render(); p.renderPowerBar(); }
        for(auto& p : team2) { p.render(); p.renderPowerBar(); }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    stadium.shutdown();
    glfwTerminate();
    return 0;
}
