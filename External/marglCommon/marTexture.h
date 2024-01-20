#pragma once

#ifdef GLEW
#include <GL/glew.h>
#elif GL3W
#include <GL/gl3w.h>
#endif

struct TTextureInfo {
    GLuint id;
    GLsizei width;
    GLsizei height;
};

TTextureInfo createEmptyTexture(int width, int height, GLenum interp_mode=GL_NEAREST);

TTextureInfo loadTextureEx(const char *filepath, GLenum interp_mode=GL_NEAREST);

bool saveFramebufferAsPNG(const char* filepath, int width, int height, GLuint framebuffer);
bool saveTextureAsPNG(const char* filepath, int width, int height, GLuint texture);
bool saveTexture(const char* filepath, int width, int height, GLuint texture);
bool saveScreen(const char* filepath, int screen_width, int screen_height);
