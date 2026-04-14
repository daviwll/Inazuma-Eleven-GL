#include "stadium.hpp"
#include <glad/glad.h>
#include <cmath>

void drawField()
{
    const float left = FIELD_LEFT;
    const float right = FIELD_RIGHT;
    const float top = FIELD_TOP;
    const float bottom = FIELD_BOTTOM;

    const float penaltyWidth = 0.28f;
    const float penaltyDepth = 0.18f;

    glLineWidth(3.0f);
    glColor3f(0.95f, 0.95f, 0.95f);

    // Outer lines
    glBegin(GL_LINE_LOOP);
        glVertex3f(left, bottom, 0.0f);
        glVertex3f(right, bottom, 0.0f);
        glVertex3f(right, top, 0.0f);
        glVertex3f(left, top, 0.0f);
    glEnd();

    // Midline
    glBegin(GL_LINES);
        glVertex3f(0.0f, bottom, 0.0f);
        glVertex3f(0.0f, top, 0.0f);
    glEnd();

    // Center circle
    const float radius = 0.15f;
    const int segments = 64;
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < segments; ++i)
        {
            float a = (2.0f * 3.1415926f * i) / segments;
            glVertex3f(std::cos(a) * radius, std::sin(a) * radius, 0.0f);
        }
    glEnd();

    // Center spot
    glPointSize(6.0f);
    glBegin(GL_POINTS);
        glVertex3f(0.0f, 0.0f, 0.01f);
    glEnd();

    // Left penalty area
    glBegin(GL_LINE_LOOP);
        glVertex3f(left, -penaltyWidth, 0.0f);
        glVertex3f(left + penaltyDepth, -penaltyWidth, 0.0f);
        glVertex3f(left + penaltyDepth, penaltyWidth, 0.0f);
        glVertex3f(left, penaltyWidth, 0.0f);
    glEnd();

    // Right penalty area
    glBegin(GL_LINE_LOOP);
        glVertex3f(right, -penaltyWidth, 0.0f);
        glVertex3f(right - penaltyDepth, -penaltyWidth, 0.0f);
        glVertex3f(right - penaltyDepth, penaltyWidth, 0.0f);
        glVertex3f(right, penaltyWidth, 0.0f);
    glEnd();
}
