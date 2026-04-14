#include "player.hpp"
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

using namespace std;


struct Ball
{
    float ballX, ballY;
};

int main()
{
    Ball ball{0.0f, 0.0f};
    vector <Player> team1[11];
    vector <Player> team2[11];

    //Team 1
    team1->push_back(Player(0.9f,0.0f, 0.3f)); //GoalKeeper
    
    return 0;
}