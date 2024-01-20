#include <cstdio>
#include "marTexture.h"
#include <iostream>
#include <string>
#include <cstring>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
#include <ctype.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void toLower(char *str) {
  for (; *str; ++str) {
    *str = tolower((unsigned char)*str);
  }
}

bool isSupportedBySTB(const char* filepath)
{
  const char *ext = strrchr(filepath, '.');
  if (!ext) return false; // 拡張子がない
  ext += 1; // ピリオドをスキップ

  char lowercaseExt[8]; // 一般的な拡張子の長さに余裕を持たせる
  strncpy(lowercaseExt, ext, sizeof(lowercaseExt) - 1);
  lowercaseExt[sizeof(lowercaseExt) - 1] = '\0'; // 終端文字を保証
  toLower(lowercaseExt);

  // 拡張子のチェック
  if (strcmp(lowercaseExt, "png") == 0 || strcmp(lowercaseExt, "bmp") == 0 ||
      strcmp(lowercaseExt, "jpg") == 0 || strcmp(lowercaseExt, "jpeg") == 0 ||
      strcmp(lowercaseExt, "psd") == 0 || strcmp(lowercaseExt, "tga") == 0 ||
      strcmp(lowercaseExt, "gif") == 0 || strcmp(lowercaseExt, "hdr") == 0 ||
      strcmp(lowercaseExt, "ppm") == 0 || strcmp(lowercaseExt, "pic") == 0) {
    return true;
  }
  return false;
}

TTextureInfo createEmptyTexture(int width, int height, GLenum interp_mode)
{
  TTextureInfo textureInfo = {0, 0, 0};

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interp_mode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interp_mode);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  textureInfo.id = texture;
  textureInfo.width = width;
  textureInfo.height = height;

  return textureInfo;
}

TTextureInfo loadTextureEx(const char *filepath, GLenum interp_mode)
{
  TTextureInfo textureInfo = {0, 0, 0};

  if (!isSupportedBySTB(filepath)) {
    printf("対応形式ではありません");
    return textureInfo;
  }

  const char* extension = strrchr(filepath, '.');

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  GLsizei width, height;
  GLubyte *imageData;

  GLenum format;
  int channels;

  imageData = stbi_load(filepath, &width, &height, &channels, 0);

  if (!imageData) {
    std::cerr << "Failed to load texture: " << filepath << std::endl;
    return textureInfo;
  }
	
  if (channels == 1)
    format = GL_RED;
  else if (channels == 3)
    format = GL_RGB;
  else if (channels == 4)
    format = GL_RGBA;
  else {
    std::cerr << "Unsupported number of channels: " << channels << std::endl;
    stbi_image_free(imageData);
    return textureInfo;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interp_mode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interp_mode);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //  画像処理向き
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //  画像処理向き

  textureInfo.id = texture;
  textureInfo.width = width;
  textureInfo.height = height;
  return textureInfo;
}

bool saveFramebufferAsPNG(const char* filepath, int width, int height, GLuint framebuffer)
{
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

  std::vector<unsigned char> data(width * height * 4);
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

  // Flip the image vertically since OpenGL reads pixels from the bottom-left corner
  int row_size = width * 4;
  for (int y = 0; y < height / 2; y++) {
    for (int x = 0; x < row_size; x++) {
      std::swap(data[y * row_size + x], data[(height - y - 1) * row_size + x]);
    }
  }
  int result = stbi_write_png(filepath, width, height, 4, data.data(), width * 4);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return result != 0;
}


bool saveTextureAsPNG(const char* filepath, int width, int height, GLuint texture)
{
  glBindTexture(GL_TEXTURE_2D, texture);

  std::vector<unsigned char> data(width * height * 4);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

  int result = stbi_write_png(filepath, width, height, 4, data.data(), width * 4);

  glBindTexture(GL_TEXTURE_2D, 0);

  return result != 0;
}

bool saveTexture(const char* filepath, int width, int height, GLuint texture)
{
  return saveTextureAsPNG(filepath, width, height, texture);
}

// 上下反転
void vflip(unsigned char *image, unsigned int img_width, unsigned int img_height)
{
  int halfTheHeightInPixels = img_height / 2;
  int heightInPixels = img_height;
  int widthInChars = img_width * 4;

  unsigned char *top;// = NULL;
  unsigned char *bottom;// = NULL;
  unsigned char temp;// = 0;

  for( int h = 0; h < halfTheHeightInPixels; ++h )
  {
    top = image + h * widthInChars;
    bottom = image + (heightInPixels - h - 1) * widthInChars;

    for( int w = 0; w < widthInChars; ++w )
    {
      // Swap the chars around.
      temp = *top;
      *top = *bottom;
      *bottom = temp;

      ++top;
      ++bottom;
    }
  }
}

bool saveScreen(const char* filepath, int screen_width, int screen_height)
{
  unsigned char* imageRGBA;
  imageRGBA = (unsigned char*)malloc(screen_width * screen_height * 4);

  glReadBuffer(GL_FRONT);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  // glReadPixels 左下を原点として読み込むので上下逆さま画像を得ることに
  glReadPixels(0, 0, screen_width, screen_height, GL_RGBA, GL_UNSIGNED_BYTE, imageRGBA);
  vflip(imageRGBA, screen_width, screen_height);

  int result = stbi_write_png(filepath, screen_width, screen_height, 4, imageRGBA, screen_width * 4);

  free(imageRGBA);

  return result != 0;
}
