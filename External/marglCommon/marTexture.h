#pragma one

#ifdef GLEW
#include <GL/glew.h>
#elif GL3W
#include <GL/gl3w.h>
#endif

struct TextureInfo {
    GLuint id;
    GLsizei width;
    GLsizei height;
};

TextureInfo createEmptyTexture(int width, int height, GLenum interp_mode=GL_NEAREST);

TextureInfo loadTextureEx(const char *filepath, GLenum interp_mode=GL_NEAREST);

bool saveFramebufferAsPNG(const char* filepath, int width, int height, GLuint framebuffer);
bool saveTextureAsPNG(const char* filepath, int width, int height, GLuint texture);
bool saveTexture(const char* filepath, int width, int height, GLuint texture);
bool saveScreen(const char* filepath, int screen_width, int screen_height);
