#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>

enum class PlayerRole {
    GOALKEEPER,
    DEFENDER,
    MIDFIELDER,
    ATTACKER
};

class Player {
public:
    float x, y, speed;
    float startX, startY;
    float facingX, facingY;
    int side;
    PlayerRole role;
    
    bool is_targeting_ball;
    float stunTimer;
    float kickPower;

    Player(float startX, float startY, float startSpeed, int startSide, PlayerRole role);
    
    void update(float ballX, float ballY, bool is_team_possessing, Player* ballOwner, const std::vector<Player>& teammates, const std::vector<Player>& opponents, float deltaTime);
    void render(); 
    void renderPowerBar();

private:
    void moveTowards(float targetX, float targetY, float currentSpeed, float deltaTime);
};

#endif
