#include "player.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include <glad/glad.h>
#include <algorithm>
#include <cmath>

Player::Player(float startX, float startY, float startSpeed, int startSide, PlayerRole role, 
               unsigned int texFace, unsigned int texBack, unsigned int texLeft, unsigned int texRight)
    : x(startX), y(startY), startX(startX), startY(startY), 
      facingX(startSide), facingY(0.0f), speed(startSpeed), side(startSide), 
      role(role), is_targeting_ball(false), stunTimer(0.0f), kickPower(0.0f), 
      texFace(texFace), texBack(texBack), texLeft(texLeft), texRight(texRight),
      animTimer(0.0f), isMoving(false) {}

void Player::moveTowards(float targetX, float targetY, float currentSpeed, float deltaTime) {
    float dx = targetX - x;
    float dy = targetY - y;
    float mag = std::sqrt(dx*dx + dy*dy);
    if (mag > 0.005f) {
        x += (dx / mag) * currentSpeed * deltaTime;
        y += (dy / mag) * currentSpeed * deltaTime;
        if (std::abs(dx) > 0.001f) facingX = dx / mag;
        if (std::abs(dy) > 0.001f) facingY = dy / mag;
        isMoving = true;
    }
    
    // Keep player within field boundaries
    float playerRadius = 0.03f;  // Half the player size
    x = std::max(-Constants::FIELD_BOUNDARY_X + playerRadius, std::min(Constants::FIELD_BOUNDARY_X - playerRadius, x));
    y = std::max(-Constants::FIELD_BOUNDARY_Y + playerRadius, std::min(Constants::FIELD_BOUNDARY_Y - playerRadius, y));
}

void Player::update(float ballX, float ballY, bool is_team_possessing, Player* ballOwner, 
                    const std::vector<Player>& teammates, const std::vector<Player>& opponents, float deltaTime) {
    using namespace Constants;
    
    // Reset isMoving flag - we'll set it to true during movement if we actually move
    bool wasMoving = isMoving;
    isMoving = false;
    
    if (stunTimer > 0) stunTimer -= deltaTime;
    float currentSpeed = speed * ((stunTimer > 0) ? 0.3f : 1.0f);

    if (role == PlayerRole::GOALKEEPER) {
        float areaLimitX = FIELD_BOUNDARY_X - PENALTY_AREA_WIDTH;
        bool isBallInArea = false;
        if (side == -1) { // Left team
            isBallInArea = (ballX < -areaLimitX && std::abs(ballY) < PENALTY_AREA_HEIGHT);
        } else { // Right team
            isBallInArea = (ballX > areaLimitX && std::abs(ballY) < PENALTY_AREA_HEIGHT);
        }

        float targetX, targetY;
        if (isBallInArea) {
            targetX = ballX;
            targetY = ballY;
            // Limit goalkeeper within his penalty area
            if (side == -1) {
                targetX = std::max(-FIELD_BOUNDARY_X, std::min(-areaLimitX, targetX));
            } else {
                targetX = std::min(FIELD_BOUNDARY_X, std::max(areaLimitX, targetX));
            }
            targetY = std::max(-PENALTY_AREA_HEIGHT, std::min(PENALTY_AREA_HEIGHT, targetY));
        } else {
            targetX = (side == -1) ? -FIELD_BOUNDARY_X : FIELD_BOUNDARY_X;
            targetY = std::max(-GOAL_HALF_WIDTH, std::min(GOAL_HALF_WIDTH, ballY));
        }
        moveTowards(targetX, targetY, currentSpeed, deltaTime);
        if (isMoving) animTimer += deltaTime * 10.0f;
        return;
    }

    float areaLimitX = FIELD_BOUNDARY_X - PENALTY_AREA_WIDTH;

    // Se o goleiro está segurando a bola, os adversários devem se afastar da área
    bool isGkHoldingBall = (ballOwner != nullptr && ballOwner->role == PlayerRole::GOALKEEPER);
    
    bool shouldMoveAway = false;
    if (isGkHoldingBall && ballOwner->side != side) {
        shouldMoveAway = true;
    }

    if (shouldMoveAway) {
        // Define um alvo fixo logo fora da área penal para evitar movimento infinito
        float targetX = (side == 1) ? -areaLimitX + 0.2f : areaLimitX - 0.2f;
        float targetY = y;
        
        // Só se move para se afastar se estiver muito perto ou dentro da área
        bool tooClose = (side == 1 && x < targetX) || (side == -1 && x > targetX);
        if (tooClose) {
            moveTowards(targetX, targetY, currentSpeed * 0.5f, deltaTime);
            if (isMoving) animTimer += deltaTime * 10.0f;
            return;
        }
    }

    if (is_team_possessing) {
        if (ballOwner == this) {
            // Se o jogador tem a bola e não é controlado pelo usuário (IA driblando),
            // a lógica de drible em game_logic.cpp que define isMoving.
            // Mas aqui Player::update é chamado para os outros jogadores.
            return;
        }

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
    if (isMoving) animTimer += deltaTime * 10.0f;
}

void Player::render() {
    unsigned int tId = 0;
    
    if (isMoving) {
        // Determine direction and use appropriate run frames
        bool movingRight = (facingX > 0);
        bool movingLeft = (facingX < 0);
        
        if (movingRight && !runFramesRight.empty()) {
            int frameIdx = static_cast<int>(animTimer) % runFramesRight.size();
            tId = runFramesRight[frameIdx];
        } else if (movingLeft && !runFramesLeft.empty()) {
            int frameIdx = static_cast<int>(animTimer) % runFramesLeft.size();
            tId = runFramesLeft[frameIdx];
        } else if (!runFramesRight.empty()) {
            // Moving vertically or no direction - use right frames as default
            int frameIdx = static_cast<int>(animTimer) % runFramesRight.size();
            tId = runFramesRight[frameIdx];
        }
        
        // If still no tId despite moving, use first run frame if available
        if (tId == 0 && !runFramesRight.empty()) {
            tId = runFramesRight[0];
        }
    }
    
    // Fallback to static sprite if not moving or no run frames
    if (tId == 0) {
        if (std::abs(facingX) > std::abs(facingY)) {
            tId = (facingX > 0) ? texRight : texLeft;
        } else {
            tId = (facingY > 0) ? texBack : texFace;
        }
        if (tId == 0) tId = texFace;
    }

    // Render shadow first
    float height = (role == PlayerRole::GOALKEEPER ? 0.08f : 0.06f);
    float width = height * 0.65f;
    float shadowY = y - height/2 - 0.005f;  // Position below player
    float shadowRadiusX = width * 0.55f;  // Slightly smaller than player width
    float shadowRadiusY = 0.008f;  // Thin ellipse
    
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.35f);  // Dark shadow with alpha
    
    // Draw shadow as an ellipse using triangle fan
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, shadowY);  // Center
    for (int i = 0; i <= 16; i++) {
        float angle = (float)i / 16.0f * 3.14159265f * 2.0f;
        float vx = x + std::cos(angle) * shadowRadiusX;
        float vy = shadowY + std::sin(angle) * shadowRadiusY;
        glVertex2f(vx, vy);
    }
    glEnd();
    
    glDisable(GL_BLEND);

    if (tId != 0) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_TEXTURE_2D);
        
        glColor3f(1.0f, 1.0f, 1.0f);

        renderTexturedQuad(tId, x - width/2, x + width/2, y - height/2, y + height/2);
        
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    } else {
        if (side == -1) {
            if (role == PlayerRole::GOALKEEPER) glColor3f(1.0f, 0.5f, 0.0f);
            else glColor3f(1.0f, 0.0f, 0.0f);
        } else {
            if (role == PlayerRole::GOALKEEPER) glColor3f(0.0f, 0.5f, 1.0f);
            else glColor3f(0.0f, 0.0f, 1.0f);
        }

        glPointSize(role == PlayerRole::GOALKEEPER ? 12.0f : 10.0f); 
        glBegin(GL_POINTS);
            glVertex2d(x, y);
        glEnd();
    }
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
