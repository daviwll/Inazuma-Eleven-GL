#include "game_logic.hpp"
#include "constants.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

void resetGame(Ball& ball, std::vector<Player>& team1, std::vector<Player>& team2, GameState& gameState, int scoringTeamSide) {
    ball.x = 0.0f;
    ball.y = 0.0f;
    ball.dx = 0.0f;
    ball.dy = 0.0f;
    ball.owner = nullptr;

    for (auto& p : team1) {
        p.x = p.startX;
        p.y = p.startY;
        p.stunTimer = 0.0f;
        p.kickPower = 0.0f;
        p.facingX = static_cast<float>(p.side);
        p.facingY = 0.0f;
    }
    for (auto& p : team2) {
        p.x = p.startX;
        p.y = p.startY;
        p.stunTimer = 0.0f;
        p.kickPower = 0.0f;
        p.facingX = static_cast<float>(p.side);
        p.facingY = 0.0f;
    }

    if (scoringTeamSide == -1) {
        ball.owner = &team1[5];
    } else {
        ball.owner = &team2[5];
    }

    if (ball.owner) {
        ball.owner->x = 0.0f;
        ball.owner->y = 0.0f;
    }

    ball.x = 0.0f;
    ball.y = 0.0f;
    gameState.kickoffTimer = 3.0f;
}

int updateBall(Ball& ball, Score& score, std::vector<Player>& team1, std::vector<Player>& team2, GameState& gameState) {
    using namespace Constants;

    if (ball.owner) {
        ball.x = ball.owner->x + (ball.owner->facingX * 0.035f);
        ball.y = ball.owner->y + (ball.owner->facingY * 0.035f);
        ball.dx = 0.0f;
        ball.dy = 0.0f;
        return 0;
    }

    ball.x += ball.dx;
    ball.y += ball.dy;
    ball.dx *= ball.friction;
    ball.dy *= ball.friction;

    if (ball.y > FIELD_BOUNDARY_Y || ball.y < -FIELD_BOUNDARY_Y) {
        ball.dy *= -1.0f;
    }

    if (std::abs(ball.x) > FIELD_BOUNDARY_X) {
        if (std::abs(ball.y) > GOAL_HALF_WIDTH) {
            ball.dx *= -1.0f;
        } else {
            if (ball.x > 0.0f) {
                score.left++;
                resetGame(ball, team1, team2, gameState, 1);
                return -1;
            } else {
                score.right++;
                resetGame(ball, team1, team2, gameState, -1);
                return 1;
            }
        }
    }

    return 0;
}

void updateTeam(
    std::vector<Player>& team,
    std::vector<Player>& opponents,
    Ball& ball,
    bool isUserTeam,
    float deltaTime,
    const InputState& inputState,
    const GameState& gameState
) {
    using namespace Constants;

    bool teamPossessing = (ball.owner && ball.owner->side == team[0].side);

    int userPlayerIdx = -1;
    if (isUserTeam) {
        float minDist = 999.0f;
        for (int i = 0; i < static_cast<int>(team.size()); ++i) {
            if (team[i].role == PlayerRole::GOALKEEPER && ball.owner != &team[i]) {
                continue;
            }
            float d = std::sqrt(std::pow(team[i].x - ball.x, 2) + std::pow(team[i].y - ball.y, 2));
            if (d < minDist) {
                minDist = d;
                userPlayerIdx = i;
            }
        }
    }

    std::vector<int> chasers;
    if (!teamPossessing) {
        struct DistancePair {
            int index;
            float dist;
        };

        std::vector<DistancePair> distances;
        for (int i = 0; i < static_cast<int>(team.size()); ++i) {
            if (team[i].role == PlayerRole::GOALKEEPER) {
                continue;
            }
            float d = std::sqrt(std::pow(team[i].x - ball.x, 2) + std::pow(team[i].y - ball.y, 2));
            distances.push_back({i, d});
        }

        std::sort(
            distances.begin(),
            distances.end(),
            [](const DistancePair& a, const DistancePair& b) { return a.dist < b.dist; }
        );

        for (int j = 0; j < 2 && j < static_cast<int>(distances.size()); ++j) {
            chasers.push_back(distances[j].index);
        }
    }

    for (int i = 0; i < static_cast<int>(team.size()); ++i) {
        if (gameState.kickoffTimer <= 0.0f && i == userPlayerIdx) {
            float speedMult = (team[i].stunTimer > 0.0f) ? 0.3f : 1.0f;
            if (team[i].kickPower > 0.0f) {
                speedMult *= 0.8f;
            }

            if (inputState.axisX != 0.0f || inputState.axisY != 0.0f) {
                team[i].x += inputState.axisX * team[i].speed * speedMult * deltaTime;
                team[i].y += inputState.axisY * team[i].speed * speedMult * deltaTime;
                team[i].facingX = inputState.axisX;
                team[i].facingY = inputState.axisY;
            }

            if (team[i].stunTimer > 0.0f) {
                team[i].stunTimer -= deltaTime;
            }

            if (ball.owner == &team[i]) {
                if (inputState.spacePressed) {
                    team[i].kickPower += deltaTime * 2.0f;
                    if (team[i].kickPower > 1.0f) {
                        team[i].kickPower = 1.0f;
                    }
                } else if (inputState.spaceWasPressed) {
                    float dirX = inputState.mouseX - team[i].x;
                    float dirY = inputState.mouseY - team[i].y;
                    float mag = std::sqrt(dirX * dirX + dirY * dirY);
                    if (mag > 0.001f) {
                        float finalPower = (0.012f + (team[i].kickPower * 0.023f)) * 0.5f;
                        ball.dx = (dirX / mag) * finalPower;
                        ball.dy = (dirY / mag) * finalPower;
                    }
                    ball.owner = nullptr;
                    ball.x += ball.dx * 2.0f;
                    ball.y += ball.dy * 2.0f;
                    team[i].kickPower = 0.0f;
                    team[i].stunTimer = 0.3f;
                }
            } else {
                team[i].kickPower = 0.0f;
            }
        } else if (gameState.kickoffTimer <= 0.0f) {
            team[i].is_targeting_ball = std::find(chasers.begin(), chasers.end(), i) != chasers.end();
            team[i].update(ball.x, ball.y, teamPossessing, ball.owner, team, opponents, deltaTime);

            if (ball.owner == &team[i]) {
                float targetGoalX = (team[i].side == -1) ? FIELD_BOUNDARY_X : -FIELD_BOUNDARY_X;

                Player* bestPassTarget = nullptr;
                float bestTargetScore = -1.0f;

                for (auto& mate : team) {
                    if (&mate == &team[i] || mate.role == PlayerRole::GOALKEEPER) {
                        continue;
                    }

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

                if (bestPassTarget && (std::rand() % 100) < 3) {
                    float passDirX = bestPassTarget->x - team[i].x;
                    float passDirY = bestPassTarget->y - team[i].y;
                    float passMag = std::sqrt(passDirX * passDirX + passDirY * passDirY);
                    float passPower = 0.012f;
                    ball.dx = (passDirX / passMag) * passPower;
                    ball.dy = (passDirY / passMag) * passPower;
                    ball.owner = nullptr;
                    team[i].stunTimer = 0.8f;
                } else if (canShoot && (std::rand() % 100) < 5) {
                    float finalPower = 0.015f + (std::rand() % 10) / 1000.0f;
                    ball.dx = (targetGoalX - team[i].x) / distToGoal * finalPower;
                    ball.dy = (0.0f - team[i].y) / distToGoal * finalPower;
                    ball.owner = nullptr;
                    ball.x += ball.dx * 2.0f;
                    ball.y += ball.dy * 2.0f;
                    team[i].stunTimer = 1.0f;
                } else {
                    float dribbleDirX = targetGoalX - team[i].x;
                    float dribbleDirY = (0.0f - team[i].y) * 0.3f;
                    float dribbleMag = std::sqrt(dribbleDirX * dribbleDirX + dribbleDirY * dribbleDirY);
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

        float distToBall = std::sqrt(std::pow(team[i].x - ball.x, 2) + std::pow(team[i].y - ball.y, 2));
        if (distToBall < 0.04f) {
            if (!ball.owner && team[i].stunTimer <= 0.0f) {
                ball.owner = &team[i];
            } else if (ball.owner && ball.owner->side != team[i].side && team[i].stunTimer <= 0.0f) {
                if (ball.owner->role != PlayerRole::GOALKEEPER) {
                    ball.owner->stunTimer = 0.5f;
                    ball.owner->kickPower = 0.0f;
                    ball.owner = &team[i];
                }
            }
        }
    }
}
