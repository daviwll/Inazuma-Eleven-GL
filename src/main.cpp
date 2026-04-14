#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>


int main()
{
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1920,1080, "NULL", NULL, NULL);

    glfwMakeContextCurrent(window);
    if(!window)
    {
        glfwTerminate();
    }


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        // Cor do gramado (R, G, B, A)
        glClearColor(0.2f, 0.5f, 0.1f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
        glPointSize(10.0f);
        glBegin(GL_POINTS);
            glVertex2f(ball.ballX, ball.ballY);
        glEnd();


        // Troca o desenho que estava sendo feito "atrás" pela frente
        glfwSwapBuffers(window);
        
        // Verifica se você clicou no X ou apertou algo
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}