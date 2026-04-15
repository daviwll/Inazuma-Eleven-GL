#include "game_logic.hpp"
#include "constants.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

int updateBall(Ball& ball, Score& score) {
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
                ball.x = 0.0f;
                ball.y = 0.0f;
                ball.dx = 0.0f;
                ball.dy = 0.0f;
                return -1;
            } else {
                score.right++;
                ball.x = 0.0f;
                ball.y = 0.0f;
                ball.dx = 0.0f;
                ball.dy = 0.0f;
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
    const InputState& inputState
) {
    using namespace Constants;

    bool teamPossessing = (ball.owner && ball.owner->side == team[0].side);

    int userPlayerIdx = -1;
    if (isUserTeam) {
        float minDist = 999.0f;
        for (int i = 0; i < static_cast<int>(team.size()); ++i) {
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
        if (i == userPlayerIdx) {
            float speedMult = (team[i].stunTimer > 0.0f) ? 0.3f : 1.0f;
            if (team[i].kickPower > 0.0f) {
                speedMult *= 0.2f;
            }

            if (inputState.axisX != 0.0f || inputState.axisY != 0.0f) {
                team[i].x += inputState.axisX * team[i].speed * speedMult;
                team[i].y += inputState.axisY * team[i].speed * speedMult;
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
                    float finalPower = 0.012f + (team[i].kickPower * 0.023f);
                    ball.dx = team[i].facingX * finalPower;
                    ball.dy = team[i].facingY * finalPower;
                    ball.owner = nullptr;
                    ball.x += ball.dx * 2.0f;
                    ball.y += ball.dy * 2.0f;
                    team[i].kickPower = 0.0f;
                    team[i].stunTimer = 0.3f;
                }
            } else {
                team[i].kickPower = 0.0f;
            }
        } else {
            team[i].is_targeting_ball = std::find(chasers.begin(), chasers.end(), i) != chasers.end();
            team[i].update(ball.x, ball.y, teamPossessing, ball.owner, team, opponents, deltaTime);

            if (ball.owner == &team[i]) {
                if ((std::rand() % 100) < 5) {
                    float targetGoalX = (team[i].side == -1) ? FIELD_BOUNDARY_X : -FIELD_BOUNDARY_X;
                    float dirX = targetGoalX - team[i].x;
                    float dirY = 0.0f - team[i].y;
                    float mag = std::sqrt(dirX * dirX + dirY * dirY);
                    float finalPower = 0.015f + (std::rand() % 10) / 1000.0f;

                    ball.dx = (dirX / mag) * finalPower;
                    ball.dy = (dirY / mag) * finalPower;
                    ball.owner = nullptr;
                    ball.x += ball.dx * 2.0f;
                    ball.y += ball.dy * 2.0f;
                    team[i].stunTimer = 0.3f;
                }
            }
        }

        float distToBall = std::sqrt(std::pow(team[i].x - ball.x, 2) + std::pow(team[i].y - ball.y, 2));
        if (distToBall < 0.04f) {
            if (!ball.owner && team[i].stunTimer <= 0.0f) {
                ball.owner = &team[i];
            } else if (ball.owner && ball.owner->side != team[i].side && team[i].stunTimer <= 0.0f) {
                ball.owner->stunTimer = 0.5f;
                ball.owner->kickPower = 0.0f;
                ball.owner = &team[i];
            }
        }
    }
}
