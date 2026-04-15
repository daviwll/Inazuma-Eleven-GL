#ifndef INPUT_HPP
#define INPUT_HPP

struct GLFWwindow;

struct InputState {
    float axisX;
    float axisY;
    bool spacePressed;
    bool spaceWasPressed;
};

void processInput(GLFWwindow* window, InputState& inputState);

#endif
