#ifndef INPUT_HPP
#define INPUT_HPP

struct GLFWwindow;

struct InputState {
    float axisX;
    float axisY;
    bool spacePressed;
    bool spaceWasPressed;
    float mouseX;
    float mouseY;
};

void processInput(GLFWwindow* window, InputState& inputState, bool allowControls);

#endif
