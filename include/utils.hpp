#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>

unsigned int loadTextureFromPng(const char* path);
void renderTexturedQuad(unsigned int textureId, float xMin, float xMax, float yMin, float yMax);
std::vector<std::string> candidateBaseDirs();

#endif
