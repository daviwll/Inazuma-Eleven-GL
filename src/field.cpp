#include "field.hpp"
#include "constants.hpp"
#include <glad/glad.h>
#include <cmath>

// Draws the pitch, markings, penalty boxes, and goals.
void Field::render() {
    using namespace Constants;

    // Pitch with alternating grass stripes
    int stripes = 12;
    float stripeWidth = (FIELD_HALF_WIDTH * 2.0f) / stripes;
    for(int i = 0; i < stripes; i++) {
        if(i % 2 == 0) glColor3f(0.2f, 0.5f, 0.1f);
        else glColor3f(0.22f, 0.55f, 0.12f);
        
        float x_start = -FIELD_HALF_WIDTH + (i * stripeWidth);
        glBegin(GL_QUADS);
            glVertex2f(x_start, -FIELD_HALF_HEIGHT);
            glVertex2f(x_start + stripeWidth, -FIELD_HALF_HEIGHT);
            glVertex2f(x_start + stripeWidth,  FIELD_HALF_HEIGHT);
            glVertex2f(x_start,  FIELD_HALF_HEIGHT);
        glEnd();
    }

    // White field lines
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);

    // Outer boundary
    glBegin(GL_LINE_LOOP);
        glVertex2f(-FIELD_BOUNDARY_X, -FIELD_BOUNDARY_Y);
        glVertex2f( FIELD_BOUNDARY_X, -FIELD_BOUNDARY_Y);
        glVertex2f( FIELD_BOUNDARY_X,  FIELD_BOUNDARY_Y);
        glVertex2f(-FIELD_BOUNDARY_X,  FIELD_BOUNDARY_Y);
    glEnd();

    // Midfield line
    glBegin(GL_LINES);
        glVertex2f(0.0f, -FIELD_BOUNDARY_Y);
        glVertex2f(0.0f,  FIELD_BOUNDARY_Y);
    glEnd();

    // Center circle
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < 360; i++) {
        float theta = i * 3.14159f / 180.0f;
        glVertex2f(0.18f * cos(theta), 0.18f * sin(theta));
    }
    glEnd();

    // Left penalty area
    glBegin(GL_LINE_STRIP);
        glVertex2f(-FIELD_BOUNDARY_X,  PENALTY_AREA_HEIGHT);
        glVertex2f(-FIELD_BOUNDARY_X + PENALTY_AREA_WIDTH,  PENALTY_AREA_HEIGHT);
        glVertex2f(-FIELD_BOUNDARY_X + PENALTY_AREA_WIDTH, -PENALTY_AREA_HEIGHT);
        glVertex2f(-FIELD_BOUNDARY_X, -PENALTY_AREA_HEIGHT);
    glEnd();

    // Right penalty area
    glBegin(GL_LINE_STRIP);
        glVertex2f( FIELD_BOUNDARY_X,  PENALTY_AREA_HEIGHT);
        glVertex2f( FIELD_BOUNDARY_X - PENALTY_AREA_WIDTH,  PENALTY_AREA_HEIGHT);
        glVertex2f( FIELD_BOUNDARY_X - PENALTY_AREA_WIDTH, -PENALTY_AREA_HEIGHT);
        glVertex2f( FIELD_BOUNDARY_X, -PENALTY_AREA_HEIGHT);
    glEnd();

    // Goals
    glLineWidth(3.0f);
    // Left goal
    glBegin(GL_LINE_STRIP);
        glVertex2f(-FIELD_BOUNDARY_X,  GOAL_HALF_WIDTH);
        glVertex2f(-FIELD_BOUNDARY_X - GOAL_DEPTH,  GOAL_HALF_WIDTH);
        glVertex2f(-FIELD_BOUNDARY_X - GOAL_DEPTH, -GOAL_HALF_WIDTH);
        glVertex2f(-FIELD_BOUNDARY_X, -GOAL_HALF_WIDTH);
    glEnd();

    // Right goal
    glBegin(GL_LINE_STRIP);
        glVertex2f( FIELD_BOUNDARY_X,  GOAL_HALF_WIDTH);
        glVertex2f( FIELD_BOUNDARY_X + GOAL_DEPTH,  GOAL_HALF_WIDTH);
        glVertex2f( FIELD_BOUNDARY_X + GOAL_DEPTH, -GOAL_HALF_WIDTH);
        glVertex2f( FIELD_BOUNDARY_X, -GOAL_HALF_WIDTH);
    glEnd();
}
