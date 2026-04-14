#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "field.hpp"
#include "stadium.hpp"
#include "player.hpp"
#include "constants.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

struct Ball {
    float x, y;
    float dx, dy;
    float friction;
    Player* owner;
};

int scoreLeft = 0;
int scoreRight = 0;
float inputX = 0, inputY = 0;
bool spacePressed = false;
bool spaceWasPressed = false;

void processInput(GLFWwindow* window) {
    inputX = 0; inputY = 0;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) inputY += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) inputY -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) inputX -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) inputX += 1.0f;
    if (inputX != 0 && inputY != 0) { inputX *= 0.707f; inputY *= 0.707f; }
    spaceWasPressed = spacePressed;
    spacePressed = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
}

void updateBall(Ball& ball) {
    using namespace Constants;
    if (ball.owner) {
        ball.x = ball.owner->x + (ball.owner->facingX * 0.035f);
        ball.y = ball.owner->y + (ball.owner->facingY * 0.035f);
        ball.dx = 0; ball.dy = 0;
    } else {
        ball.x += ball.dx; ball.y += ball.dy;
        ball.dx *= ball.friction; ball.dy *= ball.friction;
        if (ball.y > FIELD_BOUNDARY_Y || ball.y < -FIELD_BOUNDARY_Y) ball.dy *= -1.0f;
        if (std::abs(ball.x) > FIELD_BOUNDARY_X) {
            if (std::abs(ball.y) > GOAL_HALF_WIDTH) ball.dx *= -1.0f;
            else {
                if (ball.x > 0) scoreLeft++; else scoreRight++;
                ball.x = 0.0f; ball.y = 0.0f; ball.dx = 0; ball.dy = 0;
            }
        }
    }
}

void updateTeam(std::vector<Player>& team, std::vector<Player>& opponents, Ball& ball, bool isUserTeam, float deltaTime) {
    using namespace Constants;
    bool teamPossessing = (ball.owner && ball.owner->side == team[0].side);
    
    int userPlayerIdx = -1;
    if (isUserTeam) {
        float minDist = 999.0f;
        for (int i = 0; i < (int)team.size(); ++i) {
            float d = std::sqrt(std::pow(team[i].x - ball.x, 2) + std::pow(team[i].y - ball.y, 2));
            if (d < minDist) { minDist = d; userPlayerIdx = i; }
        }
    }

    // Identifica os 2 mais próximos da bola para perseguir se ninguém do time tiver a posse
    std::vector<int> chasers;
    if (!teamPossessing) {
        struct DistancePair { int index; float dist; };
        std::vector<DistancePair> distances;
        for (int i = 0; i < (int)team.size(); ++i) {
            if (team[i].role == PlayerRole::GOALKEEPER) continue;
            float d = std::sqrt(std::pow(team[i].x - ball.x, 2) + std::pow(team[i].y - ball.y, 2));
            distances.push_back({i, d});
        }
        std::sort(distances.begin(), distances.end(), [](const DistancePair& a, const DistancePair& b) { return a.dist < b.dist; });
        for(int j=0; j<2 && j<(int)distances.size(); ++j) chasers.push_back(distances[j].index);
    }

    for (int i = 0; i < (int)team.size(); ++i) {
        if (i == userPlayerIdx) {
            float speedMult = (team[i].stunTimer > 0) ? 0.3f : 1.0f;
            if (team[i].kickPower > 0) speedMult *= 0.2f;
            if (inputX != 0 || inputY != 0) {
                team[i].x += inputX * team[i].speed * speedMult;
                team[i].y += inputY * team[i].speed * speedMult;
                team[i].facingX = inputX; team[i].facingY = inputY;
            }
            if (team[i].stunTimer > 0) team[i].stunTimer -= deltaTime;
            
            if (ball.owner == &team[i]) {
                if (spacePressed) {
                    team[i].kickPower += deltaTime * 2.0f;
                    if (team[i].kickPower > 1.0f) team[i].kickPower = 1.0f;
                } else if (spaceWasPressed) {
                    float finalPower = 0.012f + (team[i].kickPower * 0.023f);
                    ball.dx = team[i].facingX * finalPower;
                    ball.dy = team[i].facingY * finalPower;
                    ball.owner = nullptr;
                    ball.x += ball.dx * 2.0f; ball.y += ball.dy * 2.0f;
                    team[i].kickPower = 0.0f;
                    team[i].stunTimer = 0.3f;
                }
            } else { team[i].kickPower = 0.0f; }
        } else {
            team[i].is_targeting_ball = std::find(chasers.begin(), chasers.end(), i) != chasers.end();
            team[i].update(ball.x, ball.y, teamPossessing, ball.owner, team, opponents, deltaTime);
            
            // IA Chuta se estiver com a bola
            if (ball.owner == &team[i]) {
                if ((rand() % 100) < 5) {
                    float targetGoalX = (team[i].side == -1) ? FIELD_BOUNDARY_X : -FIELD_BOUNDARY_X;
                    float dirX = targetGoalX - team[i].x;
                    float dirY = 0.0f - team[i].y;
                    float mag = std::sqrt(dirX*dirX + dirY*dirY);
                    float finalPower = 0.015f + (rand()%10)/1000.0f;
                    ball.dx = (dirX / mag) * finalPower;
                    ball.dy = (dirY / mag) * finalPower;
                    ball.owner = nullptr;
                    ball.x += ball.dx * 2.0f; ball.y += ball.dy * 2.0f;
                    team[i].stunTimer = 0.3f;
                }
            }
        }

        // Roubo
        float distToBall = std::sqrt(std::pow(team[i].x - ball.x, 2) + std::pow(team[i].y - ball.y, 2));
        if (distToBall < 0.04f) {
            if (!ball.owner && team[i].stunTimer <= 0) {
                ball.owner = &team[i];
            } else if (ball.owner && ball.owner->side != team[i].side && team[i].stunTimer <= 0) {
                ball.owner->stunTimer = 0.5f; ball.owner->kickPower = 0.0f; ball.owner = &team[i];
            }
        }
    }
}

int main()
{
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Inazuma Eleven GL", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    using namespace Constants;
    Field field; Stadium stadium;
    Ball ball{0.0f, 0.0f, 0.005f, 0.002f, 0.992f, nullptr};

    float s = 0.0025f;
    std::vector<Player> team1;
    team1.push_back(Player(-FIELD_BOUNDARY_X,  0.00f, s, -1, PlayerRole::GOALKEEPER));
    team1.push_back(Player(-0.65f,  0.25f, s, -1, PlayerRole::DEFENDER));
    team1.push_back(Player(-0.65f, -0.25f, s, -1, PlayerRole::DEFENDER));
    team1.push_back(Player(-0.60f,  0.50f, s, -1, PlayerRole::DEFENDER));
    team1.push_back(Player(-0.60f, -0.50f, s, -1, PlayerRole::DEFENDER));
    team1.push_back(Player(-0.35f,  0.00f, s, -1, PlayerRole::MIDFIELDER));
    team1.push_back(Player(-0.35f,  0.30f, s, -1, PlayerRole::MIDFIELDER));
    team1.push_back(Player(-0.35f, -0.30f, s, -1, PlayerRole::MIDFIELDER));
    team1.push_back(Player(-0.10f,  0.00f, s, -1, PlayerRole::ATTACKER));
    team1.push_back(Player(-0.10f,  0.40f, s, -1, PlayerRole::ATTACKER));
    team1.push_back(Player(-0.10f, -0.40f, s, -1, PlayerRole::ATTACKER));

    std::vector<Player> team2;
    team2.push_back(Player( FIELD_BOUNDARY_X,  0.00f, s,  1, PlayerRole::GOALKEEPER));
    team2.push_back(Player( 0.65f,  0.25f, s,  1, PlayerRole::DEFENDER));
    team2.push_back(Player( 0.65f, -0.25f, s,  1, PlayerRole::DEFENDER));
    team2.push_back(Player( 0.60f,  0.50f, s,  1, PlayerRole::DEFENDER));
    team2.push_back(Player( 0.60f, -0.50f, s,  1, PlayerRole::DEFENDER));
    team2.push_back(Player( 0.35f,  0.00f, s,  1, PlayerRole::MIDFIELDER));
    team2.push_back(Player( 0.35f,  0.30f, s,  1, PlayerRole::MIDFIELDER));
    team2.push_back(Player( 0.35f, -0.30f, s,  1, PlayerRole::MIDFIELDER));
    team2.push_back(Player( 0.10f,  0.00f, s,  1, PlayerRole::ATTACKER));
    team2.push_back(Player( 0.10f,  0.40f, s,  1, PlayerRole::ATTACKER));
    team2.push_back(Player( 0.10f, -0.40f, s,  1, PlayerRole::ATTACKER));

    float lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window);
        updateBall(ball);
        updateTeam(team1, team2, ball, true, deltaTime);
        updateTeam(team2, team1, ball, false, deltaTime);

        stadium.render(); stadium.renderScoreboard(scoreLeft, scoreRight);
        field.render();

        glColor3f(1.0f, 1.0f, 1.0f); glPointSize(4.0f);
        glBegin(GL_POINTS); glVertex2f(ball.x, ball.y); glEnd();
        for(auto& p : team1) { p.render(); p.renderPowerBar(); }
        for(auto& p : team2) { p.render(); p.renderPowerBar(); }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
