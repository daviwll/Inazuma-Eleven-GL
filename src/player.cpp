#include "player.hpp"
#include "constants.hpp"
#include <glad/glad.h>
#include <algorithm>
#include <cmath>

Player::Player(float startX, float startY, float startSpeed, int startSide, PlayerRole role)
    : x(startX), y(startY), startX(startX), startY(startY), 
      facingX(startSide), facingY(0.0f), speed(startSpeed), side(startSide), 
      role(role), is_targeting_ball(false), stunTimer(0.0f), kickPower(0.0f) {}

void Player::moveTowards(float targetX, float targetY, float currentSpeed, float deltaTime) {
    float dx = targetX - x;
    float dy = targetY - y;
    float mag = std::sqrt(dx*dx + dy*dy);
    if (mag > 0.005f) {
        x += (dx / mag) * currentSpeed * deltaTime;
        y += (dy / mag) * currentSpeed * deltaTime;
        facingX = dx / mag;
        facingY = dy / mag;
    }
}

void Player::update(float ballX, float ballY, bool is_team_possessing, Player* ballOwner, 
                    const std::vector<Player>& teammates, const std::vector<Player>& opponents, float deltaTime) {
    using namespace Constants;
    
    if (stunTimer > 0) stunTimer -= deltaTime;
    float currentSpeed = speed * ((stunTimer > 0) ? 0.3f : 1.0f);

    if (role == PlayerRole::GOALKEEPER) {
        float targetY = std::max(-GOAL_HALF_WIDTH, std::min(GOAL_HALF_WIDTH, ballY));
        float targetX = (side == -1) ? -FIELD_BOUNDARY_X : FIELD_BOUNDARY_X;
        moveTowards(targetX, targetY, currentSpeed, deltaTime);
        return;
    }

    if (is_team_possessing) {
        if (ballOwner == this) return;

        float attackDir = (float)-side; 
        float baseAdvancement = 0.0f;
        
        if (role == PlayerRole::DEFENDER) baseAdvancement = 0.3f; 
        else if (role == PlayerRole::MIDFIELDER) baseAdvancement = 0.55f;
        else if (role == PlayerRole::ATTACKER) baseAdvancement = 0.8f;

        // --- LÓGICA DE POSICIONAMENTO ---
        // 1. Mantém a largura da formação original (evita colarem no centro)
        float formationWeight = 0.5f;
        float ballWeight = 0.5f;
        
        float targetX = (startX + (attackDir * baseAdvancement)) * formationWeight + (ballX + attackDir * 0.15f) * ballWeight;
        // O Y agora preserva muito mais a posição original para manter o campo espalhado
        float targetY = (startY * 0.7f) + (ballY * 0.3f); 

        // 2. SEPARAÇÃO (Evita que fiquem colados uns nos outros)
        float sepX = 0, sepY = 0;
        float minSepDist = 0.15f; // Reduzido de 0.2f
        for(const auto& mate : teammates) {
            if(&mate == this) continue;
            float dx = x - mate.x;
            float dy = y - mate.y;
            float d2 = dx*dx + dy*dy;
            if(d2 < minSepDist*minSepDist && d2 > 0.0001f) { 
                float d = std::sqrt(d2);
                sepX += (dx/d) * (minSepDist - d);
                sepY += (dy/d) * (minSepDist - d);
            }
        }
        targetX += sepX * 2.5f;
        targetY += sepY * 2.5f;

        // 3. LIMITES
        if (attackDir > 0) targetX = std::min(FIELD_BOUNDARY_X - 0.05f, targetX);
        else targetX = std::max(-FIELD_BOUNDARY_X + 0.05f, targetX);
        targetY = std::max(-FIELD_BOUNDARY_Y + 0.05f, std::min(FIELD_BOUNDARY_Y - 0.05f, targetY));

        // 4. DESMARCAÇÃO (Foge dos oponentes)
        for(const auto& opp : opponents) {
            float dx = x - opp.x;
            float dy = y - opp.y;
            float d2 = dx*dx + dy*dy;
            if(d2 < 0.0144f && d2 > 0.0001f) { // Se menos de 0.12 de distância (Reduzido)
                float d = std::sqrt(d2);
                targetX += (dx/d) * 0.08f;
                targetY += (dy/d) * 0.08f;
            }
        }

        moveTowards(targetX, targetY, currentSpeed * 0.95f, deltaTime);
    } 
    else {
        // TIME ADVERSÁRIO COM A BOLA
        if (is_targeting_ball) {
            moveTowards(ballX, ballY, currentSpeed, deltaTime);
        } 
        else {
            // Marcação com distanciamento
            float targetX = startX + (ballX - startX) * 0.15f;
            float targetY = startY + (ballY - startY) * 0.25f;
            
            // Separação defensiva básica
            for(const auto& mate : teammates) {
                if(&mate == this) continue;
                float d2 = std::pow(x-mate.x,2) + std::pow(y-mate.y,2);
                if(d2 < 0.0064f) { targetY += (y > mate.y ? 0.04f : -0.04f); }
            }

            moveTowards(targetX, targetY, currentSpeed * 0.75f, deltaTime);
        }
    }
}

void Player::render() {
    if (side == -1) {
        if (role == PlayerRole::GOALKEEPER) glColor3f(1.0f, 0.5f, 0.0f);
        else glColor3f(1.0f, 0.0f, 0.0f);
    } else {
        if (role == PlayerRole::GOALKEEPER) glColor3f(0.0f, 0.5f, 1.0f);
        else glColor3f(0.0f, 0.0f, 1.0f);
    }
    if (stunTimer > 0) glColor3f(0.5f, 0.5f, 0.5f);

    glPointSize(role == PlayerRole::GOALKEEPER ? 12.0f : 10.0f); // Reduzido de 18/15
    glBegin(GL_POINTS);
        glVertex2d(x, y);
    glEnd();
}

void Player::renderPowerBar() {
    if (kickPower <= 0.01f) return;
    float barWidth = 0.08f;
    float barHeight = 0.01f;
    float barYOffset = -0.04f;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(x - barWidth/2, y + barYOffset);
        glVertex2f(x + barWidth/2, y + barYOffset);
        glVertex2f(x + barWidth/2, y + barYOffset + barHeight);
        glVertex2f(x - barWidth/2, y + barYOffset + barHeight);
    glEnd();
    glColor3f(kickPower, 1.0f - kickPower, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(x - barWidth/2, y + barYOffset);
        glVertex2f(x - barWidth/2 + (barWidth * kickPower), y + barYOffset);
        glVertex2f(x - barWidth/2 + (barWidth * kickPower), y + barYOffset + barHeight);
        glVertex2f(x - barWidth/2, y + barYOffset + barHeight);
    glEnd();
}
