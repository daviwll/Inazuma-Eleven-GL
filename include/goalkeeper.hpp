#ifndef GOALKEEPER_HPP
#define GOALKEEPER_HPP

#include <vector>

class Goalkeeper {
public:
    float x, y, speed;
    float baseX, baseY;
    float r, g, b;
    float halfW, halfH;

    Goalkeeper(float startX, float startY, float startSpeed, float colorR, float colorG, float colorB, float widthHalf, float heightHalf);
    void update(float timeSeconds);
    void render();
};

std::vector<Goalkeeper> createDefaultGoalkeepers();

#endif
