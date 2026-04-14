#include "stadium.hpp"
#include "constants.hpp"
#include <glad/glad.h>

void Stadium::render() {
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
