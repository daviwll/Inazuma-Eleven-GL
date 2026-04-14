#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>

class Player {
public:
    float x, y, speed;
    float baseX, baseY;
    float r, g, b;
    float radius;
    int teamId;
    float stamina;
    float maxStamina;

    Player(float startX, float startY, float startSpeed, float colorR, float colorG, float colorB, float startRadius, int startTeamId);
    void move(float dx, float dy);
    void moveToward(float targetX, float targetY, float moveSpeed, float dtSeconds);
    void setBaseToCurrent();
    void render(); 
};

std::vector<Player> createDefaultPlayers();

#endif
