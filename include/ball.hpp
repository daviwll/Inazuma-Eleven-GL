#ifndef BALL_HPP
#define BALL_HPP

class Player;

struct Ball {
    float x;
    float y;
    float dx;
    float dy;
    float friction;
    Player* owner;
};

#endif
