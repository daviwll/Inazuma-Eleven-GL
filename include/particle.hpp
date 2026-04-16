#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>


struct Particle {
    float x, y;           // Position
    float dx, dy;         // Velocity
    float life;           // Remaining lifetime (0-1)
    float maxLife;        // Original lifetime
    float size;           // Particle size
    float r, g, b, a;     // Color with alpha
    
    Particle(float px, float py, float vx, float vy, float lifetime, float s, float pr, float pg, float pb)
        : x(px), y(py), dx(vx), dy(vy), life(lifetime), maxLife(lifetime), size(s), r(pr), g(pg), b(pb), a(1.0f) {}
    
    void update(float deltaTime) {
        // Apply gravity
        dy -= deltaTime * 0.5f;  // Gravity effect
        
        // Update position
        x += dx * deltaTime;
        y += dy * deltaTime;
        
        // Apply friction
        dx *= 0.95f;
        dy *= 0.95f;
        
        // Update life
        life -= deltaTime;
        if (life < 0.0f) life = 0.0f;
        
        // Fade alpha
        a = life / maxLife;
    }
    
    bool isAlive() const { return life > 0.0f; }
};

class ParticleSystem {
public:
    std::vector<Particle> particles;
    
    ParticleSystem() = default;
    
    void emit(float x, float y, float vx, float vy, int count, float lifetime, float size, float r, float g, float b) {
        for (int i = 0; i < count; ++i) {
            float angle = (float)i / count * 6.28318f;  // Spread in circle
            float spread = 0.3f;
            float px = x + (vx + std::cos(angle) * spread) * 0.1f;
            float py = y + (vy + std::sin(angle) * spread) * 0.1f;
            float dvx = vx * 0.5f + std::cos(angle) * spread;
            float dvy = vy * 0.5f + std::sin(angle) * spread;
            
            particles.emplace_back(px, py, dvx, dvy, lifetime, size, r, g, b);
        }
    }
    
    void update(float deltaTime) {
        // Update all particles
        for (auto& p : particles) {
            p.update(deltaTime);
        }
        
        // Remove dead particles
        particles.erase(
            std::remove_if(particles.begin(), particles.end(),
                [](const Particle& p) { return !p.isAlive(); }),
            particles.end()
        );
    }
    
    void render();
    
    void clear() {
        particles.clear();
    }
};

#endif
