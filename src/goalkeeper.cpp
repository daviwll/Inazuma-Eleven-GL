#include "goalkeeper.hpp"
#include <glad/glad.h>
#include <vector>
#include <cmath>

Goalkeeper::Goalkeeper(float startX, float startY, float startSpeed, float colorR, float colorG, float colorB, float widthHalf, float heightHalf)
    : x(startX), y(startY), speed(startSpeed), baseX(startX), baseY(startY), r(colorR), g(colorG), b(colorB), halfW(widthHalf), halfH(heightHalf) {}

void Goalkeeper::update(float timeSeconds)
{
    const float amplitude = 0.04f;
    x = baseX + std::cos(timeSeconds * speed) * amplitude;
    y = baseY + std::sin(timeSeconds * speed) * amplitude;
}

void Goalkeeper::render()
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex3f(x - halfW, y - halfH, 0.05f);
        glVertex3f(x + halfW, y - halfH, 0.05f);
        glVertex3f(x + halfW, y + halfH, 0.05f);
        glVertex3f(x - halfW, y + halfH, 0.05f);
    glEnd();

    glLineWidth(2.0f);
    glColor3f(0.95f, 0.95f, 0.95f);
    glBegin(GL_LINE_LOOP);
        glVertex3f(x - halfW, y - halfH, 0.06f);
        glVertex3f(x + halfW, y - halfH, 0.06f);
        glVertex3f(x + halfW, y + halfH, 0.06f);
        glVertex3f(x - halfW, y + halfH, 0.06f);
    glEnd();
}

std::vector<Goalkeeper> createDefaultGoalkeepers()
{
    std::vector<Goalkeeper> keepers;
    const float halfW = 0.018f;
    const float halfH = 0.045f;

    keepers.emplace_back(-0.84f, 0.0f, 1.2f, 0.2f, 0.65f, 0.95f, halfW, halfH);
    keepers.emplace_back(0.84f, 0.0f, 1.2f, 0.95f, 0.55f, 0.2f, halfW, halfH);

    return keepers;
}
