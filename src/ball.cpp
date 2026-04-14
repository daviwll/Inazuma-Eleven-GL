#include "ball.hpp"
#include <glad/glad.h>
#include <cmath>

Ball::Ball(float startX, float startY, float startRadius)
    : x(startX), y(startY), vx(0.0f), vy(0.0f), radius(startRadius) {}

void Ball::setPosition(float nx, float ny)
{
    x = nx;
    y = ny;
}

void Ball::stop()
{
    vx = 0.0f;
    vy = 0.0f;
}

void Ball::kickTo(float targetX, float targetY, float speed)
{
    float dx = targetX - x;
    float dy = targetY - y;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len > 0.0001f)
    {
        vx = (dx / len) * speed;
        vy = (dy / len) * speed;
    }
}

void Ball::update(float dtSeconds)
{
    x += vx * dtSeconds;
    y += vy * dtSeconds;

    const float friction = 0.9f;
    vx *= std::pow(friction, dtSeconds * 60.0f);
    vy *= std::pow(friction, dtSeconds * 60.0f);

    if (std::fabs(vx) < 0.001f) vx = 0.0f;
    if (std::fabs(vy) < 0.001f) vy = 0.0f;
}

void Ball::render()
{
    const int segments = 24;
    glColor3f(0.98f, 0.98f, 0.98f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(x, y, 0.08f);
        for (int i = 0; i <= segments; ++i)
        {
            float a = (2.0f * 3.1415926f * i) / segments;
            glVertex3f(x + std::cos(a) * radius, y + std::sin(a) * radius, 0.08f);
        }
    glEnd();

    glLineWidth(1.5f);
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < segments; ++i)
        {
            float a = (2.0f * 3.1415926f * i) / segments;
            glVertex3f(x + std::cos(a) * radius, y + std::sin(a) * radius, 0.09f);
        }
    glEnd();
}
