#include "particle.hpp"
#include <glad/glad.h>

// Draws active particles as blended points.
void ParticleSystem::render() {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (auto& p : particles) {
        glColor4f(p.r, p.g, p.b, p.a);
        glPointSize(p.size);
        glBegin(GL_POINTS);
        glVertex2f(p.x, p.y);
        glEnd();
    }
    
    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
}
