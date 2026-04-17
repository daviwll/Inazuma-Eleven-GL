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

// Creates stadium state used for crowd animation and celebrations.
Stadium::Stadium() : crowdReady(false), celebrationTimer(0.0f), animationClock(0.0f), celebrationTeamSide(0) {}

// Ensures GL texture resources are released on destruction.
Stadium::~Stadium() {
    shutdown();
}

// Frees loaded crowd textures.
void Stadium::shutdown() {
    if (!crowdTextures.empty()) {
        glDeleteTextures(static_cast<GLsizei>(crowdTextures.size()), crowdTextures.data());
        crowdTextures.clear();
    }
}

// Advances crowd animation timers and celebration cooldowns.
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

// Starts a temporary crowd celebration for the scoring side.
void Stadium::triggerCrowdCelebration(int teamSide) {
    celebrationTeamSide = teamSide;
    celebrationTimer = 1.8f;
}

// Lazily loads crowd textures from candidate asset directories.
void Stadium::initializeCrowd() {
    if (crowdReady) {
        return;
    }

    constexpr std::array<const char*, 4> fileNames = {
        "fans/fans_blue_1.png",
        "fans/fans_blue_2.png",
        "fans/fans_red_1.png",
        "fans/fans_red_2.png"
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
        std::fprintf(stderr, "[Stadium] Crowd textures not found. Expected files in assets/fans/: fans_blue_1.png, fans_blue_2.png, fans_red_1.png, fans_red_2.png\n");
    } else {
        std::fprintf(stderr, "[Stadium] Loaded %zu crowd textures.\n", crowdTextures.size());
    }

    crowdReady = true;
}

// Draws one crowd strip (top or bottom) with animated spectators.
void Stadium::renderCrowdBand(float yMin, float yMax, bool mirrorY) {
    if (crowdTextures.empty()) {
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);

    const int rowCount = 4;
    const int crowdPerRow = 28;
    const float slotWidth = 2.0f / static_cast<float>(crowdPerRow);
    const float baseHeight = yMax - yMin;
    const float approxAspect = 0.55f;

    auto noise01 = [](int seed) {
        float value = std::sin(static_cast<float>(seed) * 12.9898f) * 43758.5453f;
        return value - std::floor(value);
    };

    for (int row = 0; row < rowCount; ++row) {
        for (int i = 0; i < crowdPerRow; ++i) {
            int crowdIndex = row * crowdPerRow + i;
            float n0 = noise01(crowdIndex + 11);
            float n1 = noise01(crowdIndex + 37);
            float n2 = noise01(crowdIndex + 73);
            float n3 = noise01(crowdIndex + 101);
            GLuint textureId = crowdTextures[static_cast<size_t>(n0 * static_cast<float>(crowdTextures.size())) % crowdTextures.size()];

            float rowXShift = ((row % 2 == 0) ? 0.0f : (slotWidth * 0.45f)) + ((noise01(row * 19 + 3) - 0.5f) * slotWidth * 0.25f);
            float jitter = slotWidth * ((n1 * 0.55f) - 0.275f);
            float xCenter = -1.0f + (slotWidth * (static_cast<float>(i) + 0.5f)) + rowXShift + jitter;

            float spriteHeight = baseHeight * (0.26f + n3 * 0.055f - static_cast<float>(row) * 0.014f);
            float spriteWidth = spriteHeight * approxAspect * (0.95f + n2 * 0.20f);

            if (spriteWidth > slotWidth * 1.35f) {
                spriteWidth = slotWidth * 1.35f;
            }

            float xMin = xCenter - (spriteWidth * 0.5f);
            float xMax = xCenter + (spriteWidth * 0.5f);

            float rowBase = 0.02f + static_cast<float>(row) * 0.22f;
            float overlapPull = static_cast<float>(row) * 0.06f;
            float rowJitter = (noise01(crowdIndex + 149) - 0.5f) * 0.06f;
            float spriteMinY = yMin + (baseHeight * (rowBase - overlapPull + rowJitter));
            float spriteMaxY = spriteMinY + spriteHeight;

            float wave = std::sin(animationClock * 7.0f + static_cast<float>(crowdIndex) * 0.35f);
            bool topBand = yMin > 0.0f;
            bool activeBand = (celebrationTeamSide == -1 && topBand) || (celebrationTeamSide == 1 && !topBand);

            if (celebrationTimer > 0.0f && activeBand) {
                float jump = (0.5f + 0.5f * wave) * (baseHeight * (0.16f - static_cast<float>(row) * 0.02f));
                spriteMinY += jump;
                spriteMaxY += jump;
                glColor3f(1.0f, 1.0f, 1.0f);
            } else {
                float idle = (0.5f + 0.5f * wave) * (baseHeight * 0.025f);
                spriteMinY += idle;
                spriteMaxY += idle;
                float shade = 0.92f - static_cast<float>(row) * 0.06f;
                glColor3f(shade, shade, shade);
            }

            if (spriteMaxY > yMax) {
                spriteMaxY = yMax;
            }
            if (spriteMinY < yMin) {
                float shift = yMin - spriteMinY;
                spriteMinY += shift;
                spriteMaxY += shift;
                if (spriteMaxY > yMax) {
                    spriteMaxY = yMax;
                }
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
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

// Renders the stadium background geometry and crowd layers.
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

    // Upper seating details
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

    // Lower seating details
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

// Helper that draws a simple 7-segment-like digit using lines.
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
    // Upper left
    if(digit!=1 && digit!=2 && digit!=3 && digit!=7) { glVertex2f(x-w, y+h); glVertex2f(x-w, y); }
    // Upper right
    if(digit!=5 && digit!=6) { glVertex2f(x+w, y+h); glVertex2f(x+w, y); }
    // Lower left
    if(digit==0 || digit==2 || digit==6 || digit==8) { glVertex2f(x-w, y); glVertex2f(x-w, y-h); }
    // Lower right
    if(digit!=2) { glVertex2f(x+w, y); glVertex2f(x+w, y-h); }
    glEnd();
}

// Draws the top-center scoreboard with current match score.
void Stadium::renderScoreboard(int scoreLeft, int scoreRight) {
    // Scoreboard background (TV)
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
        glVertex2f(-0.15f, 0.82f);
        glVertex2f( 0.15f, 0.82f);
        glVertex2f( 0.15f, 0.95f);
        glVertex2f(-0.15f, 0.95f);
    glEnd();

    // TV frame border
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

    // Digits
    glColor3f(1.0f, 1.0f, 0.0f); // Digital yellow
    glLineWidth(2.0f);
    drawDigit(-0.06f, 0.885f, scoreLeft % 10);
    drawDigit( 0.06f, 0.885f, scoreRight % 10);
}
