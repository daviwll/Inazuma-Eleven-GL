#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ball.hpp"
#include "audio.hpp"
#include "field.hpp"
#include "game.hpp"
#include "game_logic.hpp"
#include "input.hpp"
#include "stadium.hpp"
#include "player.hpp"
#include "constants.hpp"
#include "utils.hpp"

#include <vector>
#include <string>

int runGame()
{
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1600, 900, "Inazuma Eleven GL", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    using namespace Constants;
    Field field; Stadium stadium;
    AudioPlayer audioPlayer;
    GameState gameState{2.0f};
    Ball ball{0.0f, 0.0f, 0.005f, 0.002f, 0.98f, nullptr};
    Score score{0, 0};
    InputState inputState{0.0f, 0.0f, false, false, 0.0f, 0.0f};
    const float initialPlayerSpeed = 0.2f;

    // Load Sprites
    unsigned int blueFace = 0, blueBack = 0, blueLeft = 0, blueRight = 0;
    unsigned int redFace = 0, redBack = 0, redLeft = 0, redRight = 0;
    unsigned int texGK = 0;
    std::vector<unsigned int> blueRunFramesRight;
    std::vector<unsigned int> blueRunFramesLeft;
    std::vector<unsigned int> redRunFramesRight;
    std::vector<unsigned int> redRunFramesLeft;
    
    std::vector<std::string> baseDirs = candidateBaseDirs();
    bool audioStarted = false;
    std::vector<std::string> kickSoundPaths;
    std::vector<std::string> refereeStartSoundPaths;
    for (const auto& baseDir : baseDirs) {
        kickSoundPaths.push_back(baseDir + "sound/kick.mp3");
        refereeStartSoundPaths.push_back(baseDir + "sound/referee-start.mp3");
        if (audioPlayer.initLoopingTrack(baseDir + "sound/background-sound.mp3")) {
            audioStarted = true;
            break;
        }
    }

    if (!audioStarted) {
        audioPlayer.initLoopingTrack("assets/sound/background-sound.mp3");
    }
    kickSoundPaths.push_back("assets/sound/kick.mp3");
    refereeStartSoundPaths.push_back("assets/sound/referee-start.mp3");

    auto onKick = [&audioPlayer, &kickSoundPaths]() {
        for (const std::string& kickPath : kickSoundPaths) {
            if (audioPlayer.playOneShot(kickPath)) {
                break;
            }
        }
    };

    auto onRefereeStart = [&audioPlayer, &refereeStartSoundPaths]() {
        for (const std::string& refereePath : refereeStartSoundPaths) {
            if (audioPlayer.playOneShot(refereePath)) {
                break;
            }
        }
    };

    for (const auto& baseDir : baseDirs) {
        if (blueFace == 0) blueFace = loadTextureFromPng((baseDir + "players_blue/face_blue.png").c_str());
        if (blueBack == 0) blueBack = loadTextureFromPng((baseDir + "players_blue/back_blue.png").c_str());
        if (blueLeft == 0) blueLeft = loadTextureFromPng((baseDir + "players_blue/left_blue.png").c_str());
        if (blueRight == 0) blueRight = loadTextureFromPng((baseDir + "players_blue/right_blue.png").c_str());

        if (redFace == 0) redFace = loadTextureFromPng((baseDir + "players_red/face_Red.png").c_str());
        if (redBack == 0) redBack = loadTextureFromPng((baseDir + "players_red/back_red.png").c_str());
        if (redLeft == 0) redLeft = loadTextureFromPng((baseDir + "players_red/left_red.png").c_str());
        if (redRight == 0) redRight = loadTextureFromPng((baseDir + "players_red/right_red.png").c_str());

        if (texGK == 0) texGK = loadTextureFromPng((baseDir + "removedbg3.png").c_str());
        
        if (blueRunFramesRight.empty()) {
            unsigned int f1 = loadTextureFromPng((baseDir + "players_blue/running/run_blue1.png").c_str());
            unsigned int f2 = loadTextureFromPng((baseDir + "players_blue/running/run_blue2.png").c_str());
            unsigned int f3 = loadTextureFromPng((baseDir + "players_blue/running/run_blue3.png").c_str());
            unsigned int f4 = loadTextureFromPng((baseDir + "players_blue/running/run_blue4.png").c_str());
            if (f1 && f2 && f3 && f4) {
                blueRunFramesRight = {f1, f2, f3, f4};
            }
        }
        if (blueRunFramesLeft.empty()) {
            unsigned int f1 = loadTextureFromPng((baseDir + "players_blue/running/run_blue1_left.png").c_str());
            unsigned int f2 = loadTextureFromPng((baseDir + "players_blue/running/run_blue2_left.png").c_str());
            unsigned int f3 = loadTextureFromPng((baseDir + "players_blue/running/run_blue3_left.png").c_str());
            unsigned int f4 = loadTextureFromPng((baseDir + "players_blue/running/run_blue_left4.png").c_str());
            if (f1 && f2 && f3 && f4) {
                blueRunFramesLeft = {f1, f2, f3, f4};
            }
        }
        if (redRunFramesRight.empty()) {
            unsigned int f1 = loadTextureFromPng((baseDir + "players_red/running/run_red_1.png").c_str());
            unsigned int f2 = loadTextureFromPng((baseDir + "players_red/running/run_red2.png").c_str());
            unsigned int f3 = loadTextureFromPng((baseDir + "players_red/running/run_red3.png").c_str());
            unsigned int f4 = loadTextureFromPng((baseDir + "players_red/running/run_red4.png").c_str());
            if (f1 && f2 && f3 && f4) {
                redRunFramesRight = {f1, f2, f3, f4};
            }
        }
        if (redRunFramesLeft.empty()) {
            unsigned int f2 = loadTextureFromPng((baseDir + "players_red/running/run_red_left2.png").c_str());
            unsigned int f3 = loadTextureFromPng((baseDir + "players_red/running/run_red_left3.png").c_str());
            unsigned int f4 = loadTextureFromPng((baseDir + "players_red/running/run_red_left4.png").c_str());
            if (f2 && f3 && f4) {
                redRunFramesLeft = {f2, f3, f4};
            }
        }
    }

    std::vector<Player> team1;
    std::vector<Player> team2;

    // Team 1 (Red)
    team1.push_back(Player(-FIELD_BOUNDARY_X,  0.00f, initialPlayerSpeed, -1, PlayerRole::GOALKEEPER, texGK, texGK, texGK, texGK));
    team1.push_back(Player(-0.65f,  0.25f, initialPlayerSpeed, -1, PlayerRole::DEFENDER, redFace, redBack, redLeft, redRight));
    team1.push_back(Player(-0.65f, -0.25f, initialPlayerSpeed, -1, PlayerRole::DEFENDER, redFace, redBack, redLeft, redRight));
    team1.push_back(Player(-0.60f,  0.50f, initialPlayerSpeed, -1, PlayerRole::DEFENDER, redFace, redBack, redLeft, redRight));
    team1.push_back(Player(-0.60f, -0.50f, initialPlayerSpeed, -1, PlayerRole::DEFENDER, redFace, redBack, redLeft, redRight));
    team1.push_back(Player(-0.35f,  0.00f, initialPlayerSpeed, -1, PlayerRole::MIDFIELDER, redFace, redBack, redLeft, redRight));
    team1.push_back(Player(-0.35f,  0.30f, initialPlayerSpeed, -1, PlayerRole::MIDFIELDER, redFace, redBack, redLeft, redRight));
    team1.push_back(Player(-0.35f, -0.30f, initialPlayerSpeed, -1, PlayerRole::MIDFIELDER, redFace, redBack, redLeft, redRight));
    team1.push_back(Player(-0.10f,  0.00f, initialPlayerSpeed, -1, PlayerRole::ATTACKER, redFace, redBack, redLeft, redRight));
    team1.push_back(Player(-0.10f,  0.40f, initialPlayerSpeed, -1, PlayerRole::ATTACKER, redFace, redBack, redLeft, redRight));
    team1.push_back(Player(-0.10f, -0.40f, initialPlayerSpeed, -1, PlayerRole::ATTACKER, redFace, redBack, redLeft, redRight));

    for (auto& p : team1) {
        if (p.role != PlayerRole::GOALKEEPER) {
            p.runFramesRight = redRunFramesRight;
            p.runFramesLeft = redRunFramesLeft;
        }
    }

    // Team 2 (Blue)
    team2.push_back(Player( FIELD_BOUNDARY_X,  0.00f, initialPlayerSpeed,  1, PlayerRole::GOALKEEPER, texGK, texGK, texGK, texGK));
    team2.push_back(Player( 0.65f,  0.25f, initialPlayerSpeed,  1, PlayerRole::DEFENDER, blueFace, blueBack, blueLeft, blueRight));
    team2.push_back(Player( 0.65f, -0.25f, initialPlayerSpeed,  1, PlayerRole::DEFENDER, blueFace, blueBack, blueLeft, blueRight));
    team2.push_back(Player( 0.60f,  0.50f, initialPlayerSpeed,  1, PlayerRole::DEFENDER, blueFace, blueBack, blueLeft, blueRight));
    team2.push_back(Player( 0.60f, -0.50f, initialPlayerSpeed,  1, PlayerRole::DEFENDER, blueFace, blueBack, blueLeft, blueRight));
    team2.push_back(Player( 0.35f,  0.00f, initialPlayerSpeed,  1, PlayerRole::MIDFIELDER, blueFace, blueBack, blueLeft, blueRight));
    team2.push_back(Player( 0.35f,  0.30f, initialPlayerSpeed,  1, PlayerRole::MIDFIELDER, blueFace, blueBack, blueLeft, blueRight));
    team2.push_back(Player( 0.35f, -0.30f, initialPlayerSpeed,  1, PlayerRole::MIDFIELDER, blueFace, blueBack, blueLeft, blueRight));
    team2.push_back(Player( 0.10f,  0.00f, initialPlayerSpeed,  1, PlayerRole::ATTACKER, blueFace, blueBack, blueLeft, blueRight));
    team2.push_back(Player( 0.10f,  0.40f, initialPlayerSpeed,  1, PlayerRole::ATTACKER, blueFace, blueBack, blueLeft, blueRight));
    team2.push_back(Player( 0.10f, -0.40f, initialPlayerSpeed,  1, PlayerRole::ATTACKER, blueFace, blueBack, blueLeft, blueRight));

    for (auto& p : team2) {
        if (p.role != PlayerRole::GOALKEEPER) {
            p.runFramesRight = blueRunFramesRight;
            p.runFramesLeft = blueRunFramesLeft;
        }
    }

    resetGame(ball, team1, team2, gameState, 1);
    onRefereeStart();

    float lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
        stadium.update(deltaTime);

        if (gameState.kickoffTimer > 0.0f) {
            gameState.kickoffTimer -= deltaTime;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window, inputState, gameState.kickoffTimer <= 0.0f);
        int scorerSide = updateBall(ball, score, team1, team2, gameState);
        if (scorerSide != 0) {
            stadium.triggerCrowdCelebration(scorerSide);
            onRefereeStart();
        }
        updateTeam(team1, team2, ball, true, deltaTime, inputState, gameState, onKick);
        updateTeam(team2, team1, ball, false, deltaTime, inputState, gameState, onKick);

        stadium.render(); stadium.renderScoreboard(score.left, score.right);
        field.render();

        glColor3f(1.0f, 1.0f, 1.0f); glPointSize(4.0f);
        glBegin(GL_POINTS); glVertex2f(ball.x, ball.y); glEnd();
        for(auto& p : team1) { p.render(); p.renderPowerBar(); }
        for(auto& p : team2) { p.render(); p.renderPowerBar(); }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    stadium.shutdown();
    audioPlayer.shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
