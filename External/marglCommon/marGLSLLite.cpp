#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef GLEW
#include <GL/glew.h>
#elif GL3W
#include <GL/gl3w.h>
#endif

struct TShaderInfo {
  char *filename;
  unsigned int object;
  unsigned int shadertype;
  unsigned char *code; // Memory(Resource)のときに使用
};

using namespace std;

char *g_dir=NULL;

void marSetGLSLDir(const char *dir)
{
  int len = strlen(dir);
  g_dir = (char *)malloc(sizeof(char)*len+1);
  strcpy(g_dir, dir);
}

// シェーダのコンパイル（内部関数）
int marCompileShader(GLuint shader, const unsigned char *code)
{
  // シェーダオブジェクトにソースを入れる
  glShaderSource(shader, 1, (const GLchar **)&code, NULL);

  // シェーダをコンパイル
  glCompileShader(shader);
  GLint res;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
  if (res == GL_FALSE) {
//    printf("Error: glCompileShader()\n");

    GLint logLen;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
    if (logLen > 0) {
      char *log = (char *)malloc(logLen);
      GLsizei written;
      glGetShaderInfoLog(shader, logLen, &written, log);
      printf("シェーダのログ:\n%s\n", log);
      free(log);
    }
    return -1;
  }
  return 0;
}

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
      printf("プログラムのログ:\n%s\n", log);
      free(log);
    }
    return -1;
  } else {
    printf("リンクに成功しました。\n");
  }
  return 0;
}

void print_error_4createshader(unsigned int shadertype)
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

void setPath(char **si_filename, const char *shader_filename)
{
  if (g_dir==NULL) {
    *si_filename = (char *)shader_filename;
  } else {
    int len = strlen(g_dir) + strlen(shader_filename);
    char *tmp = (char *)malloc(sizeof(char)*len+1);
    strcpy(tmp, g_dir);
    strcat(tmp, shader_filename);
    *si_filename = tmp;
//    printf("shader_filename = %s\n", tmp);
  }
}

// シェーダプログラムの作成
int marCreateGLSLProgram(GLuint *programID, const char *vs_filename, const char *fs_filename)
{
  const int STAGE=2;

  int res;
  FILE *fp;
  int filesize;
  unsigned char *code;

  TShaderInfo si[STAGE];
  setPath((char **)&si[0].filename, vs_filename);
  setPath((char **)&si[1].filename, fs_filename);
  
  si[0].shadertype = GL_VERTEX_SHADER;
  si[1].shadertype = GL_FRAGMENT_SHADER;

  for (int i=0; i<STAGE; i++) {
    // シェーダオブジェクト（ソースを入れる器）の作成
    si[i].object = glCreateShader(si[i].shadertype);
    if (si[i].object == 0) {
      print_error_4createshader(si[i].shadertype);
      return -1;
    }

    fp = fopen(si[i].filename, "rb");
    {
      if (fp == NULL) {
	printf("could not open file: %s\n", si[i].filename);
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
	res = marCompileShader(si[i].object, code);
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

    for (int i=0; i<STAGE; i++)
      glDeleteShader(si[i].object);
    return -1;
  }

  // リンクするシェーダをアタッチ
  for (int i=0; i<STAGE; i++)
    glAttachShader(*programID, si[i].object);

  // リンクする
  res = marLinkProgram(programID);
  if (res != 0) {
    for (int i=0; i<STAGE; i++)
      glDeleteShader(si[i].object);
    return -1;
  }

  for (int i=0; i<STAGE; i++)
    glDetachShader(*programID, si[i].object);

  for (int i=0; i<STAGE; i++)
    glDeleteShader(si[i].object);

  return 0;
}

int marCreateGLSLProgram_withG(GLuint *programID,
                               const char *vs_filename,
                               const char *fs_filename,
                               const char *gs_filename)
{
  const int STAGE=3;

  int res;
  FILE *fp;
  int filesize;
  unsigned char *code;

  TShaderInfo si[STAGE];
  setPath((char **)&si[0].filename, vs_filename);
  setPath((char **)&si[1].filename, fs_filename);
  setPath((char **)&si[2].filename, gs_filename);

  si[0].shadertype = GL_VERTEX_SHADER;
  si[1].shadertype = GL_FRAGMENT_SHADER;
  si[2].shadertype = GL_GEOMETRY_SHADER;

  for (int i=0; i<STAGE; i++) {
    // シェーダオブジェクト（ソースを入れる器）の作成
    si[i].object = glCreateShader(si[i].shadertype);
    if (si[i].object == 0) {
      print_error_4createshader(si[i].shadertype);
      return -1;
    }

    fp = fopen(si[i].filename, "rb");
    {
      if (fp == NULL) {
        printf("could not open file: %s\n", si[i].filename);
        return -1;
      }
      fseek(fp, 0, SEEK_END);
      filesize = ftell(fp);
      fseek(fp, 0, SEEK_SET);

      code = (unsigned char *)malloc(filesize+1); // +1 for null terminate
      {
        fread(code, filesize, 1, fp);
        code[filesize] = 0; // null terminate
        res = marCompileShader(si[i].object, code);
      }
      free(code);
    }
    fclose(fp);

    if (res!=0) {
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
    printf("プログラムオブジェクトの作成で失敗しました。\n");

    for (int i=0; i<STAGE; i++)
      glDeleteShader(si[i].object);
    return -1;
  }

  // リンクするシェーダをアタッチ
  for (int i=0; i<STAGE; i++)
    glAttachShader(*programID, si[i].object);

  // リンクする
  res = marLinkProgram(programID);
  if (res!=0) {
    for (int i=0; i<STAGE; i++)
      glDeleteShader(si[i].object);
    return -1;
  }

  for (int i=0; i<STAGE; i++)
    glDetachShader(*programID, si[i].object);

  for (int i=0; i<STAGE; i++)
    glDeleteShader(si[i].object);

  return 0;
}

int marCreateGLSLProgram_withT(GLuint *programID,
			       const char *vs_filename,
			       const char *fs_filename,
			       const char *hs_filename,
			       const char *ds_filename)
{
  const int STAGE=4;

  int res;
  FILE *fp;
  int filesize;
  unsigned char *code;

  TShaderInfo si[STAGE];
  setPath((char **)&si[0].filename, vs_filename);
  setPath((char **)&si[1].filename, fs_filename);
  setPath((char **)&si[2].filename, hs_filename);
  setPath((char **)&si[3].filename, ds_filename);

  si[0].shadertype = GL_VERTEX_SHADER;
  si[1].shadertype = GL_FRAGMENT_SHADER;
  si[2].shadertype = GL_TESS_CONTROL_SHADER;
  si[3].shadertype = GL_TESS_EVALUATION_SHADER;

  for (int i=0; i<STAGE; i++) {
    // シェーダオブジェクト（ソースを入れる器）の作成
    si[i].object = glCreateShader(si[i].shadertype);
    if (si[i].object == 0) {
      print_error_4createshader(si[i].shadertype);
      return -1;
    }

    fp = fopen(si[i].filename, "rb");
    {
      if (fp == NULL) {
        printf("could not open file: %s\n", si[i].filename);
        return -1;
      }
      fseek(fp, 0, SEEK_END);
      filesize = ftell(fp);
      fseek(fp, 0, SEEK_SET);

      code = (unsigned char *)malloc(filesize+1); // +1 for null terminate
      {
        fread(code, filesize, 1, fp);
        code[filesize] = 0; // null terminate
        res = marCompileShader(si[i].object, code);
      }
      free(code);
    }
    fclose(fp);

    if (res!=0) {
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
    printf("プログラムオブジェクトの作成で失敗しました。\n");

    for (int i=0; i<STAGE; i++)
      glDeleteShader(si[i].object);
    return -1;
  }

  // リンクするシェーダをアタッチ
  for (int i=0; i<STAGE; i++)
    glAttachShader(*programID, si[i].object);

  // リンクする
  res = marLinkProgram(programID);
  if (res!=0) {
    for (int i=0; i<STAGE; i++)
      glDeleteShader(si[i].object);
    return -1;
  }

  for (int i=0; i<STAGE; i++)
    glDetachShader(*programID, si[i].object);

  for (int i=0; i<STAGE; i++)
    glDeleteShader(si[i].object);

  return 0;
}

int marCreateGLSLProgram_withTG(GLuint *programID,
				const char *vs_filename,
				const char *fs_filename,
				const char *hs_filename,
				const char *ds_filename,
				const char *gs_filename)
{
  const int STAGE=5;

  int res;
  FILE *fp;
  int filesize;
  unsigned char *code;

  TShaderInfo si[STAGE];
  setPath((char **)&si[0].filename, vs_filename);
  setPath((char **)&si[1].filename, fs_filename);
  setPath((char **)&si[2].filename, hs_filename);
  setPath((char **)&si[3].filename, ds_filename);
  setPath((char **)&si[4].filename, gs_filename);

  si[0].shadertype = GL_VERTEX_SHADER;
  si[1].shadertype = GL_FRAGMENT_SHADER;
  si[2].shadertype = GL_TESS_CONTROL_SHADER;
  si[3].shadertype = GL_TESS_EVALUATION_SHADER;
  si[4].shadertype = GL_GEOMETRY_SHADER;

  for (int i=0; i<STAGE; i++) {
    // シェーダオブジェクト（ソースを入れる器）の作成
    si[i].object = glCreateShader(si[i].shadertype);
    if (si[i].object == 0) {
      print_error_4createshader(si[i].shadertype);
      return -1;
    }

    fp = fopen(si[i].filename, "rb");
    {
      if (fp == NULL) {
        printf("could not open file: %s\n", si[i].filename);
        return -1;
      }
      fseek(fp, 0, SEEK_END);
      filesize = ftell(fp);
      fseek(fp, 0, SEEK_SET);

      code = (unsigned char *)malloc(filesize+1); // +1 for null terminate
      {
        fread(code, filesize, 1, fp);
        code[filesize] = 0; // null terminate
        res = marCompileShader(si[i].object, code);
      }
      free(code);
    }
    fclose(fp);

    if (res!=0) {
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
    printf("プログラムオブジェクトの作成で失敗しました。\n");

    for (int i=0; i<STAGE; i++)
      glDeleteShader(si[i].object);
    return -1;
  }

  // リンクするシェーダをアタッチ
  for (int i=0; i<STAGE; i++)
    glAttachShader(*programID, si[i].object);

  // リンクする
  res = marLinkProgram(programID);
  if (res!=0) {
    for (int i=0; i<STAGE; i++)
      glDeleteShader(si[i].object);
    return -1;
  }

  for (int i=0; i<STAGE; i++)
    glDetachShader(*programID, si[i].object);

  for (int i=0; i<STAGE; i++)
    glDeleteShader(si[i].object);

  return 0;
}

// シェーダプログラムの作成
int marCreateGLSLProgramMemory(GLuint *programID,
			       const unsigned char *vs_code,
			       const unsigned char *fs_code,
			       const int vs_codesize,
			       const int fs_codesize)
{
  const int STAGE=2;

  int res;

  TShaderInfo si[STAGE];
  si[0].shadertype = GL_VERTEX_SHADER;
  si[1].shadertype = GL_FRAGMENT_SHADER;

  si[0].code = (unsigned char *)malloc(vs_codesize+1);
  memcpy(si[0].code, vs_code, vs_codesize);
  si[0].code[vs_codesize] = 0; // null terminate
  
  si[1].code = (unsigned char *)malloc(fs_codesize+1);
  memcpy(si[1].code, fs_code, fs_codesize);
  si[1].code[fs_codesize] = 0; // null terminate

  for (int i=0; i<STAGE; i++) {
    // シェーダオブジェクト（ソースを入れる器）の作成
    si[i].object = glCreateShader(si[i].shadertype);
    if (si[i].object == 0) {
      print_error_4createshader(si[i].shadertype);
      return -1;
    }

    res = marCompileShader(si[i].object, si[i].code);

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

    for (int i=0; i<STAGE; i++)
      glDeleteShader(si[i].object);
    return -1;
  }

  // リンクするシェーダをアタッチ
  for (int i=0; i<STAGE; i++)
    glAttachShader(*programID, si[i].object);

  // リンクする
  res = marLinkProgram(programID);
  if (res != 0) {
    for (int i=0; i<STAGE; i++)
      glDeleteShader(si[i].object);
    return -1;
  }

  for (int i=0; i<STAGE; i++)
    glDetachShader(*programID, si[i].object);

  for (int i=0; i<STAGE; i++)
    glDeleteShader(si[i].object);

  return 0;
}
