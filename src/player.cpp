#include "player.hpp"
#include <glad/glad.h>
#include <cmath>
#include <vector>

Player::Player(float startX, float startY, float startSpeed, float colorR, float colorG, float colorB, float startRadius, int startTeamId)
    : x(startX), y(startY), speed(startSpeed), baseX(startX), baseY(startY), r(colorR), g(colorG), b(colorB), radius(startRadius), teamId(startTeamId), stamina(1.0f), maxStamina(1.0f){}

void Player::move(float dx, float dy)
{
    x += dx;
    y += dy;
}

void Player::moveToward(float targetX, float targetY, float moveSpeed, float dtSeconds)
{
    float dx = targetX - x;
    float dy = targetY - y;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.0001f) return;
    dx /= len;
    dy /= len;
    x += dx * moveSpeed * dtSeconds;
    y += dy * moveSpeed * dtSeconds;
}

void Player::setBaseToCurrent()
{
    baseX = x;
    baseY = y;
}

void Player::render()
{
    const int segments = 24;

    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(x, y, 0.05f);
        for (int i = 0; i <= segments; ++i)
        {
            float a = (2.0f * 3.1415926f * i) / segments;
            glVertex3f(x + std::cos(a) * radius, y + std::sin(a) * radius, 0.05f);
        }
    glEnd();

    glLineWidth(1.5f);
    glColor3f(0.95f, 0.95f, 0.95f);
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < segments; ++i)
        {
            float a = (2.0f * 3.1415926f * i) / segments;
            glVertex3f(x + std::cos(a) * radius, y + std::sin(a) * radius, 0.06f);
        }
    glEnd();
}

std::vector<Player> createDefaultPlayers()
{
    std::vector<Player> players;
    const float playerRadius = 0.03f;
    const float baseSpeed = 0.7f;

    // Left team (red)
    players.emplace_back(-0.55f, 0.35f, baseSpeed * 0.95f, 0.85f, 0.2f, 0.2f, playerRadius, 0);
    players.emplace_back(-0.55f, 0.0f, baseSpeed * 1.05f, 0.85f, 0.2f, 0.2f, playerRadius, 0);
    players.emplace_back(-0.55f, -0.35f, baseSpeed * 0.9f, 0.85f, 0.2f, 0.2f, playerRadius, 0);
    players.emplace_back(-0.25f, 0.2f, baseSpeed * 1.1f, 0.85f, 0.2f, 0.2f, playerRadius, 0);
    players.emplace_back(-0.25f, -0.2f, baseSpeed * 1.0f, 0.85f, 0.2f, 0.2f, playerRadius, 0);

    // Right team (yellow)
    players.emplace_back(0.55f, 0.35f, baseSpeed * 1.0f, 0.95f, 0.85f, 0.1f, playerRadius, 1);
    players.emplace_back(0.55f, 0.0f, baseSpeed * 0.95f, 0.95f, 0.85f, 0.1f, playerRadius, 1);
    players.emplace_back(0.55f, -0.35f, baseSpeed * 1.05f, 0.95f, 0.85f, 0.1f, playerRadius, 1);
    players.emplace_back(0.25f, 0.2f, baseSpeed * 1.1f, 0.95f, 0.85f, 0.1f, playerRadius, 1);
    players.emplace_back(0.25f, -0.2f, baseSpeed * 0.9f, 0.95f, 0.85f, 0.1f, playerRadius, 1);

    return players;
}
