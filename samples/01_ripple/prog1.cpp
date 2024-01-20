#include <stdio.h>
#ifdef GLEW
#include <GL/glew.h>
#elif GL3W
#include <GL/gl3w.h>
#endif
#include <GLFW/glfw3.h>
#include "marTexture.h"
#include "marGLSLLite.h"
#include "marShader.h"
#include "marHRTimer.h"
#include <math.h>
#include <vector>

#ifdef _WIN32
const char *FILE_SRC = "../blowfish_w640.png";
const int ANIMSTEP = 3000;
#elif __linux
const char *FILE_SRC = "../blowfish_w640.png";
const int ANIMSTEP = 1;
#endif

enum {
  TEX_SRC=0,
  TEX_DST,
  TEX_MAX
};
TTextureInfo m_texture[TEX_MAX];
GLuint m_framebuffer;

const float AMP_MAX = 32;

enum {
  GLSL_IMAGE_PROC, // 単一シェーダによる画像処理のときは、GLSL_IMAGE_PROC を使用する(毎回リネームするのは大変だから）
  GLSL_UVMAP2D,
  GLSL_MAX
};
TShader shader[GLSL_MAX];

// GL_TRIANGLE_STRIP を使うので4頂点を用意
// 最初の三角形が反時計周りになるように
float w=1.0f;
GLfloat verts[] = {
  -w, -w, 0.0f,
  w, -w, 0.0f,
  -w, w, 0.0f,
  w, w, 0.0f,
};

GLfloat uvs[] = {
  0.0f, 1.0f, // (0, 1)
  1.0f, 1.0f, // (1, 1)
  0.0f, 0.0f, // (0, 0)
  1.0f, 0.0f, // (1, 0)
};

struct TBasicData {
  float img_size_x;
  float img_size_y;
  float dummy[2]; // 配列は、16バイトアラインなので
};
TBasicData basicdata;

const int RIPPLE_COUNT = 4;

struct TRippleInfo {
  float touch_x;
  float touch_y;
  float amp;
  float delta_kappa;
};
TRippleInfo ripples[RIPPLE_COUNT];

const int TOUCH_COUNT = RIPPLE_COUNT;
typedef TRippleInfo TTouch;
std::vector<TTouch> mTouch(TOUCH_COUNT);

enum {
  VAO_IMAGE_PROC,
  VAO_UVMAP,
  VAO_MAX
};
GLuint vao[VAO_MAX];

const int VERTS_COUNT4 = 4;

GLuint ubo_basics;
GLuint ubo_ripple;

int client_width = 800;
int client_height = 600;

float totalframe = 0.0;
int framecounter = 0;
float g_delta_kappa = 0.0;

THighRezoTimer HRTimer;
double delta_theta = 0.0;
double theta_sum = 0.0;
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

// UBO バッファを生成しデータを与える
unsigned int makeUBO(int datasize, void *data)
{
  GLuint ubo;

  glGenBuffers(1, &ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
//  glBufferData(GL_UNIFORM_BUFFER, datasize, data, GL_STATIC_DRAW);
  glBufferData(GL_UNIFORM_BUFFER, datasize, data, GL_DYNAMIC_DRAW);

  return ubo;
}

void clear_touch()
{
  for (int i=0; i<TOUCH_COUNT; i++) {
    mTouch[i].touch_x = 0.0;
    mTouch[i].touch_y = 0.0;
    mTouch[i].amp = 0.0;
    mTouch[i].delta_kappa = 0.0;
  }
}

void push_touch(float touch_x, float touch_y, float amp, float delta_kappa)
{
  for (int i=0; i<TOUCH_COUNT-1; i++) {
    mTouch[i].touch_x = mTouch[i+1].touch_x;
    mTouch[i].touch_y = mTouch[i+1].touch_y;
    mTouch[i].amp = mTouch[i+1].amp;
    mTouch[i].delta_kappa = mTouch[i+1].delta_kappa;
  }
  mTouch[TOUCH_COUNT-1].touch_x = touch_x;
  mTouch[TOUCH_COUNT-1].touch_y = touch_y;
  mTouch[TOUCH_COUNT-1].amp = amp;
  mTouch[TOUCH_COUNT-1].delta_kappa = delta_kappa;
}

void setup_touch() {
  for (int i=0; i<TOUCH_COUNT; i++) {
    ripples[i].touch_x = mTouch[i].touch_x;
    ripples[i].touch_y = mTouch[i].touch_y;
    ripples[i].amp = mTouch[i].amp;
    ripples[i].delta_kappa = mTouch[i].delta_kappa;
  }
}

void print_touch() {
  for (int i=0; i<TOUCH_COUNT; i++) {
    if (mTouch[i].amp == 0.0) {
      printf("%d: not active\n", i);
    } else {
      printf("%d: active pos(%f, %f)\n", i, mTouch[i].touch_x,  mTouch[i].touch_y);
    }
/*
    mTouch[i].touch_x = mTouch[i+1].touch_x;
    mTouch[i].touch_y = mTouch[i+1].touch_y;
    mTouch[i].amp = mTouch[i+1].amp;
    mTouch[i].delta_kappa = mTouch[i+1].delta_kappa;
*/
  }
}


float dekay_amp(float amp)
{
  // amp が小さくなるほど、小さくする割合を小さくしたい
  double k=amp;
  if (k>1.0) {
    k=1.0;
  }
  k = pow(k, 10.0);
  amp -= delta_theta * pow(1.1, k);

  if (amp<=0) {
    amp = 0.0;
  }
  return amp;
}

void update_touch() {
  for (int i=0; i<TOUCH_COUNT; i++) {
    if (mTouch[i].amp != 0) {
      mTouch[i].amp = dekay_amp(mTouch[i].amp);
      mTouch[i].delta_kappa += delta_theta;
//    mTouch[i].delta_kappa += delta_theta * mTouch[i].amp / AMP_MAX;
    }
  }
}



// initialization
int init()
{
  // GLSLプログラムを作成する
  marCreateGLSLProgram(&shader[GLSL_IMAGE_PROC].program, "vs4_imageproc.glsl", "fs4_ripple.glsl");
  marCreateGLSLProgram(&shader[GLSL_UVMAP2D].program, "vs4_uvmap2d.glsl", "fs4_uvmapxd.glsl");

  // 背景色を指定する
  glClearColor(0.3, 0.4, 0.7, 1.0);

  // 裏面を表示しない
  glEnable(GL_CULL_FACE);

  // フレームバッファを作成
  glGenFramebuffers(1, &m_framebuffer);

  // loadTexture を使って PNG をロード
  m_texture[TEX_SRC] = loadTextureEx(FILE_SRC);
  m_texture[TEX_DST] = createEmptyTexture(m_texture[TEX_SRC].width, m_texture[TEX_SRC].height);

  //----------------------------------------------------------------------
  // Location
  //----------------------------------------------------------------------
  shader[GLSL_UVMAP2D].Enable();
  {
    printf("shader[GLSL_UVMAP2D].program = %d\n", shader[GLSL_UVMAP2D].program);

    // シェーダからロケーションの取得
    shader[GLSL_UVMAP2D].SetLocation("aPosition", 0);
    shader[GLSL_UVMAP2D].SetLocation("aUV", 1);

    if (shader[GLSL_UVMAP2D].PrintLocations() != 0) {
      printf("Error: Unable to retrieve a shader location.\n");
      return -1;
    }
  }
  shader[GLSL_UVMAP2D].Disable();

  shader[GLSL_IMAGE_PROC].Enable();
  {
    printf("shader[GLSL_IMAGE_PROC].program = %d\n", shader[GLSL_IMAGE_PROC].program);

    // シェーダからロケーションの取得
    shader[GLSL_IMAGE_PROC].SetLocation("aPosition", 0);

    if (shader[GLSL_IMAGE_PROC].PrintLocations() != 0) {
      printf("Error: Unable to retrieve a shader location.\n");
      return -1;
    }
  }
  shader[GLSL_IMAGE_PROC].Disable();

  //----------------------------------------------------------------------
  // VAO
  //----------------------------------------------------------------------
  glGenVertexArrays(1, &vao[VAO_IMAGE_PROC]);
  glGenVertexArrays(1, &vao[VAO_UVMAP]);

  //----------------------------------------------------------------------
  // VAO Binding
  //----------------------------------------------------------------------
  GLuint vbo;
  GLuint vbo_uv;

  glBindVertexArray(vao[VAO_IMAGE_PROC]);
  {
    vbo = makeVBO(sizeof(verts), verts);
    glVertexAttribPointer(shader[GLSL_IMAGE_PROC].GetLocation("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(shader[GLSL_IMAGE_PROC].GetLocation("aPosition"));
  }
  glBindVertexArray(0);

  // VAO Binding
  glBindVertexArray(vao[VAO_UVMAP]);
  {
    vbo = makeVBO(sizeof(verts), verts);
    glVertexAttribPointer(shader[GLSL_UVMAP2D].GetLocation("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(shader[GLSL_UVMAP2D].GetLocation("aPosition"));

    vbo_uv = makeVBO(sizeof(uvs), uvs);
    glVertexAttribPointer(shader[GLSL_UVMAP2D].GetLocation("aUV"), 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(shader[GLSL_UVMAP2D].GetLocation("aUV"));
  }
  glBindVertexArray(0);

  //----------------------------------------------------------------------
  // Uniform変数初期値
  int src_width, src_height;
  src_width = m_texture[TEX_SRC].width;
  src_height = m_texture[TEX_SRC].height;

  basicdata.img_size_x = src_width;
  basicdata.img_size_y = src_height;
  clear_touch();
  push_touch(src_width/2.0-128.0, src_height/2.0-128.0, AMP_MAX, delta_theta);
  push_touch(src_width/2.0+128.0, src_height/2.0-128.0, AMP_MAX, delta_theta);
  push_touch(src_width/2.0, src_height/2.0+128.0, AMP_MAX, delta_theta);

  ubo_basics = makeUBO(sizeof(basicdata), &basicdata);
  ubo_ripple = makeUBO(sizeof(TRippleInfo) * RIPPLE_COUNT, &ripples);

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
  framecounter++;

  // glClear() で指定した背景色で塗りつぶす
  glClear(GL_COLOR_BUFFER_BIT);

  // 画像サイズと同じビューポートを作り画像処理を行う
  glViewport(0, 0, m_texture[TEX_SRC].width, m_texture[TEX_SRC].height);
  {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer);
    {
      glBindVertexArray(vao[VAO_IMAGE_PROC]);
      {
        // 出力を受けるテクスチャーを指定する
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, m_texture[TEX_DST].id, 0);
        // シェーダプログラムの指定
        shader[GLSL_IMAGE_PROC].Enable();
        {
          bind_sampler(shader[GLSL_IMAGE_PROC].program, "image", 0, m_texture[TEX_SRC].id);

          glUniform1i(glGetUniformLocation(shader[GLSL_IMAGE_PROC].program, "uRippleCount"), RIPPLE_COUNT);

          setup_touch();

          // Uniform Block 対応コード
          GLuint bindingPoint;

          bindingPoint = 0;
          glBindBuffer(GL_UNIFORM_BUFFER, ubo_basics);
          glBufferData(GL_UNIFORM_BUFFER, sizeof(TBasicData), &basicdata, GL_DYNAMIC_DRAW);
          glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo_basics);

          bindingPoint = 1;
          glBindBuffer(GL_UNIFORM_BUFFER, ubo_ripple);
          glBufferData(GL_UNIFORM_BUFFER, sizeof(TRippleInfo) * RIPPLE_COUNT, &ripples, GL_DYNAMIC_DRAW);
          glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo_ripple);

          // drawcall
          glDrawArrays(GL_TRIANGLE_STRIP, 0, VERTS_COUNT4);
        }
        shader[GLSL_IMAGE_PROC].Disable();
      }
      glBindVertexArray(0);
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  }

//  saveTexture("test.png", m_texture[TEX_SRC].width, m_texture[TEX_SRC].height, m_texture[TEX_DST].id);

  // 表示用のビューポートを作成（ここでは同じ位置）
  glViewport(0, 0, m_texture[TEX_DST].width, m_texture[TEX_DST].height);
  {
    glBindVertexArray(vao[VAO_UVMAP]);
    {
      // シェーダプログラムの指定
      shader[GLSL_UVMAP2D].Enable();
      {
        bind_sampler(shader[GLSL_UVMAP2D].program, "image", 0, m_texture[TEX_DST].id);

        // drawcall
        glDrawArrays(GL_TRIANGLE_STRIP, 0, VERTS_COUNT4);
      }
      shader[GLSL_UVMAP2D].Disable();
    }
    glBindVertexArray(0);
  }
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

    case GLFW_KEY_F2:
      saveTexture("dst.png", m_texture[TEX_DST].width, m_texture[TEX_DST].height,  m_texture[TEX_DST].id);
      break;

    }
  }
}

void cursor_position_callback(GLFWwindow *window, double xPos, double yPos)
{
  //  printf("Mouse Down\n");
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
  if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS)) {
    printf("Mouse is Left\n");

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    // Calc Image coords
    float touch_x = x;
    float touch_y = y-(client_height - m_texture[TEX_SRC].height);

    push_touch(touch_x, touch_y, AMP_MAX, delta_theta);
    print_touch();
  }
}

void animate()
{
  totalframe += 1.0;

  // アニメーション関係のパラメータをここで計算する
  // framecountのカウントアップや、移動や回転のパラメータの計算もここで行う

  double delta_time = HRTimer.getELT();
  //  printf("delta_time = %.2f\n", delta_time);

  const float SPEED_CTL = 0.8;

  // １秒で 2PI 動いてね
  delta_theta = 2.0*M_PI * delta_time;
  // theta のトータル
  theta_sum += delta_theta;

  // 時間をコントロールする係数
  move_step = (sin(theta_sum * SPEED_CTL)+1)/2.0; // move_step = range[0.0 .. 1.0]
  move_step = 0.5 * move_step + 0.4; // move_step = range[0.4 .. 0.9]

  update_touch();
}

int main(void)
{
  if (! glfwInit() ) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // higher 3.2

  window = glfwCreateWindow(client_width, client_height, "OpenGL sample", NULL, NULL);
  if (! window) {
    fprintf(stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);

#ifdef GPUINFO
  // gl**CreateWindow() の後に、GPU情報を取得する。
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
    printf("Error: %s\n", glewGetErrorString(err));
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

  // 拡張命令を有効にしてから初期化処理を呼ぶ
  int result = init();
  if (result != 0) {
    return result;
  }

  // callback
  // 描画処理でシェーダを使うのでシェーダコンパイル後にコールバックの設定をする
  glfwSetWindowSizeCallback(window, window_size_callback);
  glfwSetWindowRefreshCallback(window, window_refresh_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  while ( ! glfwWindowShouldClose (window) ) {
    render_frame();

    animate();

    glfwSwapBuffers(window);
    glfwPollEvents();
//    glfwWaitEvents();
  }

  quit_opengl();
  glfwTerminate();
  return 0;
}

// 終了処理
void quit_opengl()
{
  printf("quit\n");

  glDisable(GL_CULL_FACE);

  // フレームバッファの削除
  glDeleteFramebuffers(1, &m_framebuffer);

  // テクスチャの削除
  for (int i=0; i<TEX_MAX; i++) {
    glDeleteTextures(1, &m_texture[i].id);
  }

  // GLSLプログラムの削除
  for (int i=0; i<GLSL_MAX; i++) {
    glDeleteProgram(shader[i].program);
  }
}
