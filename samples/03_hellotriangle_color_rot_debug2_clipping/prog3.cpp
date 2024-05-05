#include <stdio.h>
#ifdef GLEW
#include <GL/glew.h>
#elif GL3W
#include <GL/gl3w.h>
#endif
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include "marTexture.h"
#include "marGLSLLite.h"
#include "marShader.h"
#include "marHRTimer.h"
#include "marMatrix2.h"
#include <math.h>

const char *FILE_DEBUG_DIGITS = "../number_texture_mplus.png";

enum {
  TEX_SRC=0,
  TEX_DEBUG_DIGITS,
  TEX_MAX
};
TTextureInfo m_texture[TEX_MAX];

enum {
  GLSL_TRIANGLE, // 単一シェーダによる画像処理のときは、GLSL_IMAGE_PROC を使用する(毎回リネームするのは大変だから）
  GLSL_MAX
};
TShader shader[GLSL_MAX];

float w = 1.0f;
GLfloat verts[] = {
  -w, -w, 0.0,
  w, -w, 0.0,
  0.0, w, 0.0,
};
GLfloat colors[] = {
  1.0, 0.0, 0.0, // R
  0.0, 1.0, 0.0, // G
  0.0, 0.0, 1.0, // B
};

GLuint vao;
const int VERTS_COUNT3 = 3;

int client_width = 800;
int client_height = 600;

int running;

THighRezoTimer HRTimer;
double delta_theta = 0.0;
double acc_theta = 0.0; // Accumlated theta: 累積のシータ
float move_step;

// プロトタイプ宣言
void quit_opengl();

GLFWwindow* window;

// VBO バッファを生成しデータを与える
unsigned int makeVBO(int datasize, float *data)
{
  GLuint vbo;

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, datasize, data, GL_STATIC_DRAW);

  return vbo;
}

// initialization
int init()
{
  // GLSLプログラムを作成する
  marCreateGLSLProgram(&shader[GLSL_TRIANGLE].program, "vs4_color.glsl", "fs4_color.glsl");

  // 背景色を指定する
  glClearColor(0.3, 0.4, 0.7, 1.0);

  // 裏面を表示しない
//  glEnable(GL_CULL_FACE);

  m_texture[TEX_DEBUG_DIGITS] = loadTextureEx(FILE_DEBUG_DIGITS);

  //----------------------------------------------------------------------
  // Location
  //----------------------------------------------------------------------
  shader[GLSL_TRIANGLE].Enable();
  {
    printf("shader[GLSL_TRIANGLE].program = %d\n", shader[GLSL_TRIANGLE].program);

    // シェーダからロケーションの取得
    shader[GLSL_TRIANGLE].SetLocation("aPosition", 0);
    shader[GLSL_TRIANGLE].SetLocation("aColor", 1);
    shader[GLSL_TRIANGLE].SetLocation("uMVPMatrix");

    if (shader[GLSL_TRIANGLE].PrintLocations() != 0) {
      printf("Error: Unable to retrieve a shader location.\n");
      return -1;
    }
  }
  shader[GLSL_TRIANGLE].Disable();

  //----------------------------------------------------------------------
  // VAO
  //----------------------------------------------------------------------
  glGenVertexArrays(1, &vao);

  //----------------------------------------------------------------------
  // VAO Binding
  //----------------------------------------------------------------------
  GLuint vbo;
  GLuint vbo_color;

  glBindVertexArray(vao);
  {
    vbo = makeVBO(sizeof(verts), verts);
    glVertexAttribPointer(shader[GLSL_TRIANGLE].GetLocation("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(shader[GLSL_TRIANGLE].GetLocation("aPosition"));

    vbo_color = makeVBO(sizeof(colors), colors);
    glVertexAttribPointer(shader[GLSL_TRIANGLE].GetLocation("aColor"), 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(shader[GLSL_TRIANGLE].GetLocation("aColor"));
  }
  glBindVertexArray(0);

  return 0;
}

/*
  program: シェーダプログラム
  name: uniform で glsl プログラムに渡す名前
  unit_no: テクスチャーユニットの番号
  texture: m_texture[]
*/
void bind_sampler(GLuint program, const char *name, GLint unit_no, GLuint texture)
{
//  GLuint location = glGetUniformLocation(program, name);
//  glUniform1i(location, unit_no);
  glActiveTexture(GL_TEXTURE0 + unit_no); // テクスチャユニット unit_no
  glBindTexture(GL_TEXTURE_2D, texture);
}

void render_frame()
{
  // glClear() で指定した背景色で塗りつぶす
//  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindVertexArray(vao);
  {
    shader[GLSL_TRIANGLE].Enable();
    {
      bind_sampler(shader[GLSL_TRIANGLE].program, "debug_digits", 10, m_texture[TEX_DEBUG_DIGITS].id);

      smat4 m_rot;
//      float m_rot[4][4];
//      matget_rotate(getdeg(delta_theta/10.0), false, true, false, m_rot, true);
      matget_rotate(acc_theta, false, true, false, m_rot);
//      matget_identity(m_rot);

      // ワールド変換行列
      glUniformMatrix4fv(shader[GLSL_TRIANGLE].GetLocation("uMVPMatrix"), 1, GL_FALSE, &m_rot.m[0][0]);

      glDrawArrays(GL_TRIANGLES, 0, VERTS_COUNT3);
    }
    shader[GLSL_TRIANGLE].Disable();
  }
  glBindVertexArray(0);

//  glfwSwapBuffers(window);
}

void window_refresh_callback(GLFWwindow* window)
{
  render_frame();
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
  client_width = width;
  client_height = height;

  glViewport(0, 0, client_width, client_height);

  render_frame();
  glfwSwapBuffers(window);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);

  if (action == GLFW_PRESS) {
    switch(key) {
    case GLFW_KEY_F12:
    case GLFW_KEY_PRINT_SCREEN:
      // ウィンドウキャプチャ（[P]rint screen）= Screen shot
      saveScreen("screenshot.png", client_width, client_height);
      break;

    }
  }
}

void animate()
{
//  totalframe += 1.0;

  // アニメーション関係のパラメータをここで計算する
  // framecountのカウントアップや、移動や回転のパラメータの計算もここで行う

  double delta_time = HRTimer.getELT();
  //  printf("delta_time = %.2f\n", delta_time);

  const float SPEED_CTL = 0.8;

  // １秒で 2PI 動いてね
//  delta_theta = 2.0*M_PI * delta_time;
  delta_theta = M_PI * delta_time;

  // theta のトータル
  acc_theta += delta_theta;
  if (acc_theta > 2.0 * M_PI) {
    acc_theta -= 2.0 * M_PI;
  }

  // 時間をコントロールする係数
  move_step = (sin(acc_theta * SPEED_CTL)+1)/2.0; // move_step = range[0.0 .. 1.0]
  move_step = 0.5 * move_step + 0.4; // move_step = range[0.4 .. 0.9]
}

int main(int argc, char *argv[])
{
  if (! glfwInit() ) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  window = glfwCreateWindow(client_width, client_height, "OpenGL sample", NULL, NULL);
  if (! window) {
    fprintf(stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);

#ifdef GPUINFO
  printf("OpenGL Vendor  = %s\n", glGetString(GL_VENDOR));
  printf("OpenGL GPU     = %s\n", glGetString(GL_RENDERER));
  printf("OpenGL Version = %s\n", glGetString(GL_VERSION));
  printf("GLSL   Version = %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif

  // OpenGL 拡張機能を使えるように！！
#ifdef GLEW
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    fprintf(stderr, "Error: '%s'\n", glewGetErrorString(err));
    return 1;
  }
#elif GL3W
  if (gl3wInit()) {
    fprintf(stderr, "failed to initialize OpenGL\n");
    return -1;
  }
  if (!gl3wIsSupported(3, 2)) {
    fprintf(stderr, "OpenGL 3.2 not supported\n");
    return -1;
  }
#endif
  printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
         glGetString(GL_SHADING_LANGUAGE_VERSION));

  // OpenGL 拡張を有効にしてから初期化処理を呼ぶ
  // init() 内で、programID を使用するので、シェーダプログラムを作成してから呼ぶこと
  init();

  // callback
  // 描画処理でシェーダを使うのでシェーダコンパイル後にコールバックの設定をする
  glfwSetWindowSizeCallback(window, window_size_callback);
  glfwSetWindowRefreshCallback(window, window_refresh_callback);
  glfwSetKeyCallback(window, key_callback);

  while ( ! glfwWindowShouldClose(window) ) {

    render_frame();

    animate();

    glfwSwapBuffers(window);
//    glfwWaitEvents();
    glfwPollEvents();
  }

  quit_opengl();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

// 終了処理
void quit_opengl()
{
  glDisable(GL_CULL_FACE);

  glDisableVertexAttribArray(0);

  // テクスチャの削除
  glDeleteTextures(1, &m_texture[TEX_DEBUG_DIGITS].id);

  // GLSLプログラムの削除
  for (int i=0; i<GLSL_MAX; i++) {
    glDeleteProgram(shader[i].program);
  }
}
