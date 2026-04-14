#include "hud.hpp"
#include <glad/glad.h>

static void drawRect(float x0, float y0, float x1, float y1, float z)
{
    glBegin(GL_QUADS);
        glVertex3f(x0, y0, z);
        glVertex3f(x1, y0, z);
        glVertex3f(x1, y1, z);
        glVertex3f(x0, y1, z);
    glEnd();
}

static void drawDigit(int digit, float x, float y, float w, float h, float thick, float z)
{
    static const int segments[10] = {
        0b1111110,
        0b0110000,
        0b1101101,
        0b1111001,
        0b0110011,
        0b1011011,
        0b1011111,
        0b1110000,
        0b1111111,
        0b1111011
    };

    int mask = (digit >= 0 && digit <= 9) ? segments[digit] : 0;
    float x0 = x;
    float x1 = x + w;
    float y0 = y;
    float y1 = y + h;
    float t = thick;

    if (mask & 0b1000000) drawRect(x0 + t, y1 - t, x1 - t, y1, z);         // top
    if (mask & 0b0100000) drawRect(x1 - t, y0 + h * 0.5f, x1, y1 - t, z);   // upper right
    if (mask & 0b0010000) drawRect(x1 - t, y0 + t, x1, y0 + h * 0.5f, z);   // lower right
    if (mask & 0b0001000) drawRect(x0 + t, y0, x1 - t, y0 + t, z);          // bottom
    if (mask & 0b0000100) drawRect(x0, y0 + t, x0 + t, y0 + h * 0.5f, z);   // lower left
    if (mask & 0b0000010) drawRect(x0, y0 + h * 0.5f, x0 + t, y1 - t, z);   // upper left
    if (mask & 0b0000001) drawRect(x0 + t, y0 + h * 0.5f - t * 0.5f, x1 - t, y0 + h * 0.5f + t * 0.5f, z); // middle
}

static void drawNumber(int value, float x, float y, float w, float h, float thick, float spacing, float z)
{
    if (value < 0) value = 0;
    if (value > 99) value = 99;
    int tens = value / 10;
    int ones = value % 10;
    drawDigit(tens, x, y, w, h, thick, z);
    drawDigit(ones, x + w + spacing, y, w, h, thick, z);
}

static void drawColon(float x, float y, float size, float z)
{
    float d = size;
    drawRect(x, y + d * 1.2f, x + d, y + d * 2.2f, z);
    drawRect(x, y, x + d, y + d, z);
}

void drawHud(int scoreLeft, int scoreRight, float passCharge)
{
    float barLeft = -0.9f;
    float barRight = -0.3f;
    float barTop = 0.88f;
    float barBottom = 0.84f;

    if (passCharge < 0.0f) passCharge = 0.0f;
    if (passCharge > 1.0f) passCharge = 1.0f;

    glLineWidth(2.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex3f(barLeft, barBottom, 0.2f);
        glVertex3f(barRight, barBottom, 0.2f);
        glVertex3f(barRight, barTop, 0.2f);
        glVertex3f(barLeft, barTop, 0.2f);
    glEnd();

    float fillRight = barLeft + (barRight - barLeft) * passCharge;
    glColor3f(0.2f, 0.8f, 0.2f);
    drawRect(barLeft, barBottom, fillRight, barTop, 0.21f);

    float digitW = 0.04f;
    float digitH = 0.08f;
    float thick = 0.01f;
    float spacing = 0.01f;
    float scoreY = 0.82f;

    glColor3f(0.9f, 0.2f, 0.2f);
    drawNumber(scoreLeft, -0.14f, scoreY, digitW, digitH, thick, spacing, 0.2f);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawColon(-0.01f, scoreY + 0.01f, 0.01f, 0.2f);

    glColor3f(0.95f, 0.85f, 0.1f);
    drawNumber(scoreRight, 0.04f, scoreY, digitW, digitH, thick, spacing, 0.2f);

    int chargePercent = static_cast<int>(passCharge * 100.0f + 0.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawNumber(chargePercent, -0.24f, 0.835f, 0.03f, 0.06f, 0.008f, 0.007f, 0.2f);
}
