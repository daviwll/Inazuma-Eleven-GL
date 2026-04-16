#ifndef BALL_HPP
#define BALL_HPP

#include <vector>

class Player;

struct Ball {
    float x;
    float y;
    float dx;
    float dy;
    float friction;
    Player* owner;

    bool isSuperShot = false;
    float rotation = 0.0f;
    int currentFrame = 0;
    float animationTimer = 0.0f;
    
    // Physics with spin and curve
    float spinX = 0.0f;   // Spin rotation (affects curve)
    float spinY = 0.0f;   // Spin rotation (affects curve)
    float spinZ = 0.0f;   // Spin rotation (affects curve)
    float magnusForceScale = 0.8f;  // Strength of Magnus effect
    
    // Motion blur trail
    std::vector<float> trailX;
    std::vector<float> trailY;
    static constexpr int TRAIL_LENGTH = 8;

    std::vector<unsigned int> normalTextures;
    unsigned int superTexture = 0;
    
    // Hissatsu (Special Shot) charging effect
    float chargingPower = 0.0f;  // 0.0 to 1.0 when charging

    void loadTextures();
    void update(float deltaTime);
    void render();
    void renderMotionBlur();
    void renderHissatsuEffect(float kickPower);
};

#endif
