#ifndef MATCH_HPP
#define MATCH_HPP

#include <vector>
#include "player.hpp"
#include "goalkeeper.hpp"
#include "ball.hpp"

class Match {
public:
    std::vector<Player> players;
    std::vector<Goalkeeper> keepers;
    Ball ball;
    int activePlayerIndex;
    bool ballPossessed;
    int ballOwnerIndex;
    float aimX;
    float aimY;
    float passCharge;
    float shootCharge;
    int scoreLeft;
    int scoreRight;
    int prevSpace;
    int prevTab;
    int prevShoot;

    Match();
    void update(float dtSeconds, float timeSeconds, void* glfwWindow);
    void render();
    void renderHud();
};

#endif
