#include "input.hpp"
#include <GLFW/glfw3.h>

void processInput(GLFWwindow* window, InputState& inputState) {
    inputState.axisX = 0.0f;
    inputState.axisY = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        inputState.axisY += 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        inputState.axisY -= 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        inputState.axisX -= 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        inputState.axisX += 1.0f;
    }

    if (inputState.axisX != 0.0f && inputState.axisY != 0.0f) {
        inputState.axisX *= 0.707f;
        inputState.axisY *= 0.707f;
    }

    inputState.spaceWasPressed = inputState.spacePressed;
    inputState.spacePressed = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
}
