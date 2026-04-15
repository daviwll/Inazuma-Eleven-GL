#include "stadium.hpp"
#include "constants.hpp"
#include "utils.hpp"
#include <glad/glad.h>

#include <array>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <string>
#include <unistd.h>

Stadium::Stadium() : crowdReady(false), celebrationTimer(0.0f), animationClock(0.0f), celebrationTeamSide(0) {}

Stadium::~Stadium() {
    shutdown();
}

void Stadium::shutdown() {
    if (!crowdTextures.empty()) {
        glDeleteTextures(static_cast<GLsizei>(crowdTextures.size()), crowdTextures.data());
        crowdTextures.clear();
    }
}

void Stadium::update(float deltaTime) {
    animationClock += deltaTime;
    if (celebrationTimer > 0.0f) {
        celebrationTimer -= deltaTime;
        if (celebrationTimer <= 0.0f) {
            celebrationTimer = 0.0f;
            celebrationTeamSide = 0;
        }
    }
}

void Stadium::triggerCrowdCelebration(int teamSide) {
    celebrationTeamSide = teamSide;
    celebrationTimer = 1.8f;
}

void Stadium::initializeCrowd() {
    if (crowdReady) {
        return;
    }

    constexpr std::array<const char*, 3> fileNames = {
        "removedbg.png",
        "removedbg2.png",
        "removedbg3.png"
    };
    const std::vector<std::string> baseDirs = candidateBaseDirs();

    for (const char* fileName : fileNames) {
        for (const std::string& baseDir : baseDirs) {
            std::string fullPath = baseDir + fileName;
            GLuint textureId = loadTextureFromPng(fullPath.c_str());
            if (textureId != 0U) {
                crowdTextures.push_back(textureId);
                break;
            }
        }
    }

    if (crowdTextures.empty()) {
        std::fprintf(stderr, "[Stadium] Crowd textures not found. Expected files in assets/: removedbg.png, removedbg2.png, removedbg3.png\n");
    } else {
        std::fprintf(stderr, "[Stadium] Loaded %zu crowd textures.\n", crowdTextures.size());
    }

    crowdReady = true;
}

void Stadium::renderCrowdBand(float yMin, float yMax, bool mirrorY) {
    if (crowdTextures.empty()) {
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);

    const int crowdCount = 56;
    const float slotWidth = 2.0f / static_cast<float>(crowdCount);
    const float baseHeight = yMax - yMin;

    for (int i = 0; i < crowdCount; ++i) {
        GLuint textureId = crowdTextures[static_cast<size_t>(i) % crowdTextures.size()];
        float jitter = slotWidth * ((i % 5 == 0) ? -0.18f : ((i % 5 == 1) ? -0.08f : ((i % 5 == 2) ? 0.0f : ((i % 5 == 3) ? 0.09f : 0.17f))));
        float xCenter = -1.0f + (slotWidth * (static_cast<float>(i) + 0.5f)) + jitter;
        float spriteWidth = slotWidth * ((i % 2 == 0) ? 0.72f : 0.60f);
        float spriteHeight = baseHeight * ((i % 3 == 0) ? 0.42f : 0.34f);
        float xMin = xCenter - (spriteWidth * 0.5f);
        float xMax = xCenter + (spriteWidth * 0.5f);
        float rowOffset = baseHeight * ((i % 4 == 0) ? 0.06f : ((i % 4 == 1) ? 0.16f : ((i % 4 == 2) ? 0.24f : 0.30f)));
        float spriteMinY = yMin + rowOffset;
        float spriteMaxY = spriteMinY + spriteHeight;

        float wave = std::sin(animationClock * 7.0f + static_cast<float>(i) * 0.35f);
        bool topBand = yMin > 0.0f;
        bool activeBand = (celebrationTeamSide == -1 && topBand) || (celebrationTeamSide == 1 && !topBand);

        if (celebrationTimer > 0.0f && activeBand) {
            float jump = (0.5f + 0.5f * wave) * (baseHeight * 0.18f);
            spriteMinY += jump;
            spriteMaxY += jump;
            glColor3f(1.0f, 1.0f, 1.0f);
        } else {
            float idle = (0.5f + 0.5f * wave) * (baseHeight * 0.03f);
            spriteMinY += idle;
            spriteMaxY += idle;
            glColor3f(0.85f, 0.85f, 0.85f);
        }

        if (spriteMaxY > yMax) {
            spriteMaxY = yMax;
        }

        if (!mirrorY) {
            renderTexturedQuad(textureId, xMin, xMax, spriteMinY, spriteMaxY);
        } else {
            glBindTexture(GL_TEXTURE_2D, textureId);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(xMin, spriteMinY);
            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(xMax, spriteMinY);
            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(xMax, spriteMaxY);
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(xMin, spriteMaxY);
            glEnd();
        }
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void Stadium::render() {
    initializeCrowd();

    using namespace Constants;

    // Arquibancada Superior
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS);
        glVertex2f(-1.0f,  FIELD_HALF_HEIGHT);
        glVertex2f( 1.0f,  FIELD_HALF_HEIGHT);
        glVertex2f( 1.0f,  1.0f);
        glVertex2f(-1.0f,  1.0f);
    glEnd();

    // Detalhes assentos superior
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
        for(float y = FIELD_HALF_HEIGHT + 0.05f; y < 1.0f; y += 0.05f) {
            glVertex2f(-1.0f, y); glVertex2f( 1.0f, y);
        }
    glEnd();

    renderCrowdBand(FIELD_HALF_HEIGHT + 0.02f, 0.98f, false);

    // Arquibancada Inferior
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -FIELD_HALF_HEIGHT);
        glVertex2f( 1.0f, -FIELD_HALF_HEIGHT);
        glVertex2f( 1.0f, -1.0f);
        glVertex2f(-1.0f, -1.0f);
    glEnd();

    // Detalhes assentos inferior
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
        for(float y = -FIELD_HALF_HEIGHT - 0.05f; y > -1.0f; y -= 0.05f) {
            glVertex2f(-1.0f, y); glVertex2f( 1.0f, y);
        }
    glEnd();

    renderCrowdBand(-0.98f, -FIELD_HALF_HEIGHT - 0.02f, true);

    // Laterais
    glColor3f(0.35f, 0.35f, 0.35f);
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f); glVertex2f(-FIELD_HALF_WIDTH, -FIELD_HALF_HEIGHT);
        glVertex2f(-FIELD_HALF_WIDTH,  FIELD_HALF_HEIGHT); glVertex2f(-1.0f,  1.0f);
    glEnd();
    glBegin(GL_QUADS);
        glVertex2f( FIELD_HALF_WIDTH, -FIELD_HALF_HEIGHT); glVertex2f( 1.0f, -1.0f);
        glVertex2f( 1.0f,  1.0f); glVertex2f( FIELD_HALF_WIDTH,  FIELD_HALF_HEIGHT);
    glEnd();
}

// Helper para desenhar números simples com linhas
void drawDigit(float x, float y, int digit) {
    float w = 0.02f;
    float h = 0.04f;
    glBegin(GL_LINES);
    // Topo
    if(digit!=1 && digit!=4) { glVertex2f(x-w, y+h); glVertex2f(x+w, y+h); }
    // Meio
    if(digit!=1 && digit!=7 && digit!=0) { glVertex2f(x-w, y); glVertex2f(x+w, y); }
    // Fundo
    if(digit!=1 && digit!=4 && digit!=7) { glVertex2f(x-w, y-h); glVertex2f(x+w, y-h); }
    // Superior Esquerdo
    if(digit!=1 && digit!=2 && digit!=3 && digit!=7) { glVertex2f(x-w, y+h); glVertex2f(x-w, y); }
    // Superior Direito
    if(digit!=5 && digit!=6) { glVertex2f(x+w, y+h); glVertex2f(x+w, y); }
    // Inferior Esquerdo
    if(digit==0 || digit==2 || digit==6 || digit==8) { glVertex2f(x-w, y); glVertex2f(x-w, y-h); }
    // Inferior Direito
    if(digit!=2) { glVertex2f(x+w, y); glVertex2f(x+w, y-h); }
    glEnd();
}

void Stadium::renderScoreboard(int scoreLeft, int scoreRight) {
    // Fundo do Placar (Televisão)
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
        glVertex2f(-0.15f, 0.82f);
        glVertex2f( 0.15f, 0.82f);
        glVertex2f( 0.15f, 0.95f);
        glVertex2f(-0.15f, 0.95f);
    glEnd();

    // Borda da TV
    glColor3f(0.6f, 0.6f, 0.6f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.15f, 0.82f);
        glVertex2f( 0.15f, 0.82f);
        glVertex2f( 0.15f, 0.95f);
        glVertex2f(-0.15f, 0.95f);
    glEnd();

    // Divisor central
    glBegin(GL_LINES);
        glVertex2f(0.0f, 0.84f); glVertex2f(0.0f, 0.93f);
    glEnd();

    // Números
    glColor3f(1.0f, 1.0f, 0.0f); // Amarelo digital
    glLineWidth(2.0f);
    drawDigit(-0.06f, 0.885f, scoreLeft % 10);
    drawDigit( 0.06f, 0.885f, scoreRight % 10);
}
