#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include "player.hpp"
#include "goalkeeper.hpp"
#include "match.hpp"
#include "stadium.hpp"





int main()
{
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    GLFWwindow* window = glfwCreateWindow(800, 600, "NULL", NULL, NULL);

    glfwMakeContextCurrent(window);
    if(!window)
    {
        glfwTerminate();
    }


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    Match match;
    float lastTime = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window)) {
        // Cor do gramado (R, G, B, A)
        glClearColor(0.2f, 0.5f, 0.1f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLoadIdentity();

        drawField();

        float timeSeconds = static_cast<float>(glfwGetTime());
        float dt = timeSeconds - lastTime;
        lastTime = timeSeconds;

        match.update(dt, timeSeconds, window);
        match.render();
        match.renderHud();


        // Troca o desenho que estava sendo feito "atrás" pela frente
        glfwSwapBuffers(window);
        
        // Verifica se você clicou no X ou apertou algo
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
