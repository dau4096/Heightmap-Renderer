#ifndef RENDER_H
#define RENDER_H

#include "includes.h"
#include "utils.h"
#include <array>

namespace render {
    GLFWwindow* initializeWindow(int width, int height, const char* title);
    GLuint createShaderProgram(std::string name, bool hasVertexSource=true);


    void createConstUBO();


    GLuint createTexture(int width, int height);
    GLuint loadTexture(std::string fileName);


    GLuint getVAO();
}

#endif
