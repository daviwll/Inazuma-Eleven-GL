#ifndef PLAYER_HPP
#define PLAYER_HPP

class Player {
public:
    float x, y, speed;

    Player(float startX, float startY, float startSpeed);
    void update();
    void render(); 
};

#endif