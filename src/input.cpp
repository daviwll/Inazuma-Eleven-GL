#include "input.hpp"
#include <GLFW/glfw3.h>

// Reads keyboard/mouse input and maps it into normalized game controls.
void processInput(GLFWwindow* window, InputState& inputState, bool allowControls) {
    inputState.axisX = 0.0f;
    inputState.axisY = 0.0f;

    int width = 1;
    int height = 1;
    glfwGetWindowSize(window, &width, &height);

    double cursorX = 0.0;
    double cursorY = 0.0;
    glfwGetCursorPos(window, &cursorX, &cursorY);

    inputState.mouseX = static_cast<float>((cursorX / static_cast<double>(width)) * 2.0 - 1.0);
    inputState.mouseY = static_cast<float>(1.0 - (cursorY / static_cast<double>(height)) * 2.0);

    if (allowControls) {
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
    }

    if (inputState.axisX != 0.0f && inputState.axisY != 0.0f) {
        inputState.axisX *= 0.707f;
        inputState.axisY *= 0.707f;
    }

    inputState.spaceWasPressed = inputState.spacePressed;
    inputState.spacePressed = allowControls && (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
}
