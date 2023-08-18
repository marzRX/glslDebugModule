#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef GLEW
#include <GL/glew.h>
#elif GL3W
#include <GL/gl3w.h>
#endif
#include "marGLSLLite.h"

// コンパイル成功時、リンク成功時のメッセージを抑制
// Global debug flag
bool MAR_GLSL_DEBUG = true;

struct TShaderInfoDesc {
  const char *filename;
  unsigned int shadertype;
  unsigned int object;
  const unsigned char *code; // Memory(Resource)のときに使用
  size_t codesize; // Memory(Resource)のときに使用 // constはつけない
};

using namespace std;

char *g_dir=NULL;


// GLSL/SPIR-V のディレクトリをセットします
// Sets the directory for GLSL/SPIR-V
void marSetGLSLDir(const char *dir)
{
  int len = strlen(dir);
  g_dir = (char *)malloc(sizeof(char)*len+1);
  strcpy(g_dir, dir);
}

// シェーダのコンパイル（内部関数）
// Compiles the given shader (Internal function)
int marCompileShader(const char *filename, GLuint shader, const unsigned char *code, size_t codesize = 0)
{
  const GLchar* sourceArray[1] = {(const GLchar*)code};
  GLint lengths[1];

  // シェーダオブジェクトにソースを入れる
  if (codesize > 0) {
    lengths[0] = static_cast<GLint>(codesize);
    glShaderSource(shader, 1, sourceArray, lengths);
  } else {
    glShaderSource(shader, 1, sourceArray, NULL);
  }

  // シェーダをコンパイル
  glCompileShader(shader);
  GLint res;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
  if (res == GL_FALSE) {

    GLint logLen;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
    if (logLen > 0) {
      char *log = (char *)malloc(logLen);
      GLsizei written;
      glGetShaderInfoLog(shader, logLen, &written, log);
      printf("[%s]\n", filename);
      printf("%s\n", log);
      free(log);
    }
    return -1;
  } else {
    // コンパイルに成功
    if (MAR_GLSL_DEBUG) {
      printf("[%s] Compile Successful\n", filename);
    }
  }
  return 0;
}

// プログラムのリンク
// Links the given program
int marLinkProgram(GLuint *programID)
{
  GLint status;
  glLinkProgram(*programID);

  // リンクのステータス
  glGetProgramiv(*programID, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    printf("シェーダプログラムのリンクに失敗しました。\n");

    GLint logLen;
    glGetProgramiv(*programID, GL_INFO_LOG_LENGTH, &logLen);
    if (logLen > 0) {
      char *log = (char *)malloc(logLen);
      GLsizei written;
      glGetProgramInfoLog(*programID, logLen, &written, log);
      printf("Link Error:\n");
      printf("%s\n", log);
      free(log);
    }
    return -1;
  } else {
    if (MAR_GLSL_DEBUG) {
      printf("Link Successful\n");
    }
  }
  return 0;
}

// シェーダの作成に関するエラーを表示する
void printShaderCreationError(unsigned int shadertype)
{
  switch (shadertype) {
  case GL_VERTEX_SHADER:
    printf("Error: glCreateShader(GL_VERTEX_SHADER)\n");
    break;
  case GL_FRAGMENT_SHADER:
    printf("Error: glCreateShader(GL_FRAGMENT_SHADER)\n");
    break;
  case GL_TESS_CONTROL_SHADER:
    printf("Error: glCreateShader(GL_TESS_CONTROL_SHADER)\n");
    break;
  case GL_TESS_EVALUATION_SHADER:
    printf("Error: glCreateShader(GL_TESS_EVALUATION_SHADER)\n");
    break;
  case GL_GEOMETRY_SHADER:
    printf("Error: glCreateShader(GL_GEOMETRY_SHADER)\n");
    break;
  }
}

// シェーダプログラムの作成
int marCreateGLSLProgram(unsigned int *programID,
                         const char *vs_filename,
                         const char *fs_filename,
                         std::vector<TShaderInfo> shaders_4gt)
{
  int res;
  FILE *fp;
  int filesize;
  unsigned char *code;

  std::vector<TShaderInfoDesc> si;
  // 頂点シェーダーとフラグメントシェーダーをシェーダー管理リストに追加
  si.push_back(TShaderInfoDesc{vs_filename, GL_VERTEX_SHADER, 0, NULL, 0});
  si.push_back(TShaderInfoDesc{fs_filename, GL_FRAGMENT_SHADER, 0, NULL, 0});

  // geometry, tesselation シェーダーを追加
  TShaderInfoDesc info;
  for (const auto& shader : shaders_4gt) {
    info = TShaderInfoDesc{shader.filename, shader.shadertype, 0, NULL, 0};
    si.push_back(info);
  }


  // パスのプレフィックスが設定されている場合、それをすべてのシェーダのパスに追加
  if (g_dir != NULL) {
    for (int i = 0; i < si.size(); i++) {
      size_t total_length = strlen(g_dir) + strlen(si[i].filename) + 1; // +1 for null terminator
      char* combined_path = new char[total_length];
      strcpy(combined_path, g_dir);
      strcat(combined_path, si[i].filename);

      si[i].filename = combined_path;
    }
  }

  for (int i=0; i<si.size(); i++) {
    // シェーダオブジェクト（ソースを入れる器）の作成
    si[i].object = glCreateShader(si[i].shadertype);
    if (si[i].object == 0) {
      printShaderCreationError(si[i].shadertype);

      // メモリ解放
      if (g_dir != NULL) {
        for (int i = 0; i < si.size(); i++) {
          delete[] si[i].filename;
        }
      }
      return -1;
    }

    fp = fopen(si[i].filename, "rb");
    {
      if (fp == NULL) {
        printf("!!! ERROR: could not open file: %s\n", si[i].filename);
        return -1;
      }
      // ファイルサイズを得る
      fseek(fp, 0, SEEK_END);
      filesize = ftell(fp);
      fseek(fp, 0, SEEK_SET);

      code = (unsigned char *)malloc(filesize+1); // +1 for null terminate
      {
        fread(code, filesize, 1, fp);
        code[filesize] = 0; // null terminate
        res = marCompileShader(si[i].filename, si[i].object, code);
      }
      free(code);
    }
    fclose(fp);

    if (res != 0) {
      glDeleteShader(si[i].object);
      return -1;
    }
  } // for

  //======================================================================
  // リンクして、シェーダプログラムを作る
  //======================================================================
  // 空のプログラムを用意
  *programID = glCreateProgram();
  if (*programID == 0) {
    printf("Error: glCreateProgram()\n");

    for (int i=0; i<si.size(); i++)
      glDeleteShader(si[i].object);
    return -1;
  }

  // リンクするシェーダをアタッチ
  for (int i=0; i<si.size(); i++)
    glAttachShader(*programID, si[i].object);

  // リンクする
  res = marLinkProgram(programID);
  if (res != 0) {
    for (int i=0; i<si.size(); i++)
      glDeleteShader(si[i].object);
    return -1;
  }

  for (int i=0; i<si.size(); i++)
    glDetachShader(*programID, si[i].object);

  for (int i=0; i<si.size(); i++)
    glDeleteShader(si[i].object);

  // メモリ解放
  if (g_dir != NULL) {
    for (int i = 0; i < si.size(); i++) {
      delete[] si[i].filename;
    }
  }

  return 0;
}
