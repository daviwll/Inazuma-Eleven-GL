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
    unsigned int texFace, texBack, texLeft, texRight;
    
    std::vector<unsigned int> runFramesLeft;
    std::vector<unsigned int> runFramesRight;
    float animTimer;
    bool isMoving;

    Player(float startX, float startY, float startSpeed, int startSide, PlayerRole role, 
           unsigned int texFace = 0, unsigned int texBack = 0, unsigned int texLeft = 0, unsigned int texRight = 0);
    
    void update(float ballX, float ballY, bool is_team_possessing, Player* ballOwner, const std::vector<Player>& teammates, const std::vector<Player>& opponents, float deltaTime);
    void render(); 
    void renderPowerBar();

private:
    void moveTowards(float targetX, float targetY, float currentSpeed, float deltaTime);
};

#endif
