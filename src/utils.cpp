#include "utils.hpp"
#include "stb_image.h"
#include <glad/glad.h>
#include <vector>
#include <string>
#include <filesystem>
#include <unistd.h>

unsigned int loadTextureFromPng(const char* path) {
    int width = 0;
    int height = 0;
    int channels = 0;

    stbi_set_flip_vertically_on_load(1);
    unsigned char* pixels = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    if (!pixels) {
        return 0;
    }

    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    stbi_image_free(pixels);
    return textureId;
}

void renderTexturedQuad(unsigned int textureId, float xMin, float xMax, float yMin, float yMax) {
    glBindTexture(GL_TEXTURE_2D, textureId);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(xMin, yMin);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(xMax, yMin);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(xMax, yMax);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(xMin, yMax);
    glEnd();
}

std::vector<std::string> candidateBaseDirs() {
    std::vector<std::string> dirs = {
        "assets/",
        "../assets/",
        "../../assets/",
        "../../../assets/"
    };

    char exePath[4096] = {0};
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len > 0) {
        std::filesystem::path exeDir = std::filesystem::path(std::string(exePath, static_cast<size_t>(len))).parent_path();
        dirs.push_back((exeDir / "assets").string() + "/");
        dirs.push_back((exeDir / "../assets").string() + "/");
    }

    return dirs;
}
