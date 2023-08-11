#include <cstdio>
#include "marTexture.h"
#include <iostream>
#include <string>
#include <cstring>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

TextureInfo createEmptyTexture(int width, int height, GLenum interp_mode)
{
  TextureInfo textureInfo = {0, 0, 0};

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

TextureInfo loadTextureEx(const char *filepath, GLenum interp_mode)
{
  TextureInfo textureInfo = {0, 0, 0};

  const char* extension = strrchr(filepath, '.');

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  GLsizei width, height;
  GLubyte *imageData;

  GLenum format;
  int channels;

  // ここでファイルの拡張子に基づいて適切なデコード関数を呼び出す
  if (extension) {
    if (strcmp(extension, ".png") == 0) {
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
      /*
	} else if (strcmp(extension, ".jpg") == 0 ||
	strcmp(extension, ".jpeg") == 0) {
      */
    } // png
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

bool saveTextureAsPNG(GLuint texture, const char* filepath, int width, int height)
{
  glBindTexture(GL_TEXTURE_2D, texture);

  std::vector<unsigned char> data(width * height * 4);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

  int result = stbi_write_png(filepath, width, height, 4, data.data(), width * 4);

  glBindTexture(GL_TEXTURE_2D, 0);

  return result != 0;
}

bool saveFramebufferAsPNG(GLuint framebuffer, const char* filepath, int width, int height)
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


bool saveTexture(GLuint texture, const char* filepath, int width, int height)
{
  return saveTextureAsPNG(texture, filepath, width, height);
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
