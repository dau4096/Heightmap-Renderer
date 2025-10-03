#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "includes.h"
#include "utils.h"
#include <array>

namespace graphics {
    GLFWwindow* initializeWindow(int width, int height, const char* title);
    GLuint createShaderProgram(std::string name, bool hasVertexSource=true);


    void createConstUBO();


    GLuint createTexture(int width, int height, GLint imageFormat=GL_RGBA32F);
    GLuint loadTexture(std::string fileName);
    GLuint createTexture2DArray(bool isHeight=false);


    GLuint getVAO();
}

#endif
