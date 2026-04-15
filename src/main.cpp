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
float kickoffTimer = 2.0f; // 2 segundos de atraso no início
double mouseX = 0, mouseY = 0;

std::vector<Player> team1;
std::vector<Player> team2;
float initialPlayerSpeed = 0.2f;

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    mouseX = (xpos / (double)width) * 2.0 - 1.0;
    mouseY = 1.0 - (ypos / (double)height) * 2.0;
}

void resetGame(Ball& ball, int scoringTeamSide) {
    using namespace Constants;
    ball.x = 0.0f;
    ball.y = 0.0f;
    ball.dx = 0;
    ball.dy = 0;
    ball.owner = nullptr;

    for (auto& p : team1) {
        p.x = p.startX;
        p.y = p.startY;
        p.stunTimer = 0.0f;
        p.kickPower = 0.0f;
        p.facingX = p.side; p.facingY = 0.0f;
    }
    for (auto& p : team2) {
        p.x = p.startX;
        p.y = p.startY;
        p.stunTimer = 0.0f;
        p.kickPower = 0.0f;
        p.facingX = p.side; p.facingY = 0.0f;
    }

    if (scoringTeamSide == -1) { 
        ball.owner = &team2[5]; 
        ball.x = 0.05f; ball.y = 0.0f; 
    } else { 
        ball.owner = &team1[5]; 
        ball.x = -0.05f; ball.y = 0.0f; 
    }

    kickoffTimer = 3.0f; 
}


void processInput(GLFWwindow* window) {
    inputX = 0; inputY = 0;
    if (kickoffTimer <= 0.0f) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) inputY += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) inputY -= 1.0f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) inputX -= 1.0f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) inputX += 1.0f;
        if (inputX != 0 && inputY != 0) { inputX *= 0.707f; inputY *= 0.707f; }
        spaceWasPressed = spacePressed;
        spacePressed = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
    } else {
        spacePressed = false; 
    }
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
                if (ball.x > 0) { scoreLeft++; resetGame(ball, 1); } 
                else { scoreRight++; resetGame(ball, -1); } 
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
        if (kickoffTimer <= 0.0f) { 
            if (i == userPlayerIdx) {
                float speedMult = (team[i].stunTimer > 0) ? 0.3f : 1.0f;
                if (team[i].kickPower > 0) speedMult *= 0.2f;
                if (inputX != 0 || inputY != 0) {
                    team[i].x += inputX * team[i].speed * speedMult * deltaTime;
                    team[i].y += inputY * team[i].speed * speedMult * deltaTime;
                    team[i].facingX = inputX; team[i].facingY = inputY;
                }
                if (team[i].stunTimer > 0) team[i].stunTimer -= deltaTime;

                if (ball.owner == &team[i]) {
                    if (spacePressed) {
                        team[i].kickPower += deltaTime * 2.0f;
                        if (team[i].kickPower > 1.0f) team[i].kickPower = 1.0f;
                    } else if (spaceWasPressed) {
                        float dirX = mouseX - team[i].x;
                        float dirY = mouseY - team[i].y;
                        float mag = std::sqrt(dirX*dirX + dirY*dirY);
                        if (mag > 0.001f) {
                           float finalPower = (0.012f + (team[i].kickPower * 0.023f)) * 0.5f;
                           ball.dx = (dirX / mag) * finalPower;
                           ball.dy = (dirY / mag) * finalPower;
                        }
                        ball.owner = nullptr;
                        ball.x += ball.dx * 2.0f; ball.y += ball.dy * 2.0f;
                        team[i].kickPower = 0.0f;
                        team[i].stunTimer = 0.3f;
                    }
                } else { team[i].kickPower = 0.0f; }
            } else {
                team[i].is_targeting_ball = std::find(chasers.begin(), chasers.end(), i) != chasers.end();
                team[i].update(ball.x, ball.y, teamPossessing, ball.owner, team, opponents, deltaTime);

                if (ball.owner == &team[i]) {
                    // --- AI Decision: Pass, Shoot, or Dribble ---
                    float targetGoalX = (team[i].side == -1) ? FIELD_BOUNDARY_X : -FIELD_BOUNDARY_X;
                    
                    Player* bestPassTarget = nullptr;
                    float bestTargetScore = -1.0f; 

                    for (auto& mate : team) {
                        if (&mate == &team[i] || mate.role == PlayerRole::GOALKEEPER) continue;

                        float mateDistToGoal = std::sqrt(std::pow(targetGoalX - mate.x, 2) + std::pow(0.0f - mate.y, 2));
                        float playerDistToGoal = std::sqrt(std::pow(targetGoalX - team[i].x, 2) + std::pow(0.0f - team[i].y, 2));

                        if (mateDistToGoal < playerDistToGoal || mate.role == PlayerRole::ATTACKER) { 
                            float score = 1.0f / (1.0f + mateDistToGoal); 
                            
                            bool isMateOpen = true;
                            for (const auto& opp : opponents) {
                                float oppToMateDist = std::sqrt(std::pow(opp.x - mate.x, 2) + std::pow(opp.y - mate.y, 2));
                                if (oppToMateDist < 0.2f) { 
                                    isMateOpen = false;
                                    break;
                                }
                            }

                            if (isMateOpen && score > bestTargetScore) {
                                bestTargetScore = score;
                                bestPassTarget = &mate;
                            }
                        }
                    }
                    
                    float distToGoal = std::sqrt(std::pow(targetGoalX - team[i].x, 2) + std::pow(0.0f - team[i].y, 2));
                    bool canShoot = distToGoal < 0.4f;

                    if (bestPassTarget && (rand() % 100) < 3) { // Reduced pass chance
                        float passDirX = bestPassTarget->x - team[i].x;
                        float passDirY = bestPassTarget->y - team[i].y;
                        float passMag = std::sqrt(passDirX*passDirX + passDirY*passDirY);
                        float passPower = 0.012f; 
                        ball.dx = (passDirX / passMag) * passPower;
                        ball.dy = (passDirY / passMag) * passPower;
                        ball.owner = nullptr;
                        team[i].stunTimer = 0.8f; // Increased post-pass delay
                    }
                    else if (canShoot && (rand() % 100) < 5) { 
                        float finalPower = 0.015f + (rand()%10)/1000.0f;
                        ball.dx = (targetGoalX - team[i].x) / distToGoal * finalPower;
                        ball.dy = (0.0f - team[i].y) / distToGoal * finalPower;
                        ball.owner = nullptr;
                        ball.x += ball.dx * 2.0f;
                        ball.y += ball.dy * 2.0f;
                        team[i].stunTimer = 1.0f; 
                    } else {
                        // Dribble forward if no other action is taken
                        float dribbleDirX = targetGoalX - team[i].x;
                        float dribbleDirY = (0.0f - team[i].y) * 0.3f; // Less vertical movement
                        float dribbleMag = std::sqrt(dribbleDirX*dribbleDirX + dribbleDirY*dribbleDirY);
                        if (dribbleMag > 0.001f) {
                            float dribbleSpeed = team[i].speed * 0.8f * deltaTime;
                            team[i].x += (dribbleDirX / dribbleMag) * dribbleSpeed;
                            team[i].y += (dribbleDirY / dribbleMag) * dribbleSpeed;
                            team[i].facingX = dribbleDirX / dribbleMag;
                            team[i].facingY = dribbleDirY / dribbleMag;
                        }
                    }
                }
            }
        }
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

    glfwSetCursorPosCallback(window, cursorPosCallback);

    using namespace Constants;
    Field field; Stadium stadium;
    Ball ball{0.0f, 0.0f, 0.005f, 0.002f, 0.98f, nullptr};

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

    resetGame(ball, 1); 

    float lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        if (kickoffTimer > 0) kickoffTimer -= deltaTime;

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
