#ifndef GAME_LOGIC_HPP
#define GAME_LOGIC_HPP

#include "ball.hpp"
#include "input.hpp"
#include "player.hpp"
#include <vector>

struct Score {
    int left;
    int right;
};

int updateBall(Ball& ball, Score& score);

void updateTeam(
    std::vector<Player>& team,
    std::vector<Player>& opponents,
    Ball& ball,
    bool isUserTeam,
    float deltaTime,
    const InputState& inputState
);

#endif
