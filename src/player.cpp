#include "player.hpp"
#include <glad/glad.h>

Player::Player(float startX, float startY, float startSpeed)
    : x(startX), y(startY), speed(startSpeed){}

void Player::render()
{
    glPointSize(15.0f);
    glBegin(GL_POINTS);
        glVertex2d(x, y);
    glEnd();
}