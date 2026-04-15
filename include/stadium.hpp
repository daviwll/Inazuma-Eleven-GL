#ifndef STADIUM_HPP
#define STADIUM_HPP

#include <vector>

class Stadium {
public:
    Stadium();
    ~Stadium();

    void update(float deltaTime);
    void triggerCrowdCelebration(int teamSide);
    void render();
    void renderScoreboard(int scoreLeft, int scoreRight);
    void shutdown();

private:
    bool crowdReady;
    std::vector<unsigned int> crowdTextures;
    float celebrationTimer;
    float animationClock;
    int celebrationTeamSide;

    void initializeCrowd();
    void renderCrowdBand(float yMin, float yMax, bool mirrorY);
};

#endif
