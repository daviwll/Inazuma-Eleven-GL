#ifndef BALL_HPP
#define BALL_HPP

class Ball {
public:
    float x, y;
    float vx, vy;
    float radius;

    Ball(float startX, float startY, float startRadius);
    void setPosition(float nx, float ny);
    void stop();
    void kickTo(float targetX, float targetY, float speed);
    void update(float dtSeconds);
    void render();
};

#endif
