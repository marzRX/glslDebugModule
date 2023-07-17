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
bool saveTextureAsPNG(GLuint texture, const char* filepath, int width, int height);
bool saveFramebufferAsPNG(GLuint framebuffer, const char* filepath, int width, int height);

bool saveTexture(GLuint texture, const char* filepath, int width, int height);
bool saveScreen(const char* filepath, int screen_width, int screen_height);
