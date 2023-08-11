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
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

/*
#ifdef _WIN32
//const char *FILE_SRC = "c:/_myResource/Parrots.png";
//const char *FILE_SRC = "c:/_myResource/Lenna512x512_border.png";
#elif __linux
//const char *FILE_SRC = "/usr/local/share/image/Parrots.png";
#endif
*/

const char* FONT_SRC = "../mplus-1p-medium.ttf"; // 日本語のフォント

enum {
  TEX_SRC=0,
  TEX_DST,
  TEX_MAX
};
TextureInfo m_texture[TEX_MAX];
GLuint m_framebuffer;

enum {
  GLSL_IMAGE_PROC, // 単一シェーダによる画像処理のときは、GLSL_IMAGE_PROC を使用する(毎回リネームするのは大変だから）
  GLSL_UVMAP,
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

enum {
  VAO_IMAGE_PROC,
  VAO_UVMAP,
  VAO_MAX
};
GLuint vao[VAO_MAX];

const int VERTS_COUNT4 = 4;

int client_width = 800;
int client_height = 800;

// プロトタイプ宣言
void quit_opengl();

GLFWwindow* window;

int vali_wave_count = 12;
float val_wave_amp = 30.0;
float val_phase_deg = 0.0;

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
  marCreateGLSLProgram(&shader[GLSL_IMAGE_PROC].program, "vs4_imageproc.glsl", "fs4_pulse_circle.glsl");
  marCreateGLSLProgram(&shader[GLSL_UVMAP].program, "vs4_uvmap.glsl", "fs4_uvmapxd.glsl");

  // 背景色を指定する
  glClearColor(0.3, 0.4, 0.7, 1.0);

  // 裏面を表示しない
  glEnable(GL_CULL_FACE);

  // フレームバッファを作成
  glGenFramebuffers(1, &m_framebuffer);

  // loadTexture を使って PNG をロード
  m_texture[TEX_SRC] = createEmptyTexture(512, 512);

  m_texture[TEX_DST] = createEmptyTexture(m_texture[TEX_SRC].width, m_texture[TEX_SRC].height);

  //----------------------------------------------------------------------
  shader[GLSL_UVMAP].Enable();
  {
    printf("shader[GLSL_UVMAP].program = %d\n", shader[GLSL_UVMAP].program);

    // シェーダからロケーションの取得
    shader[GLSL_UVMAP].SetLocation("aPosition", 0);
    shader[GLSL_UVMAP].SetLocation("aUV", 1);

    if (shader[GLSL_UVMAP].PrintLocations() != 0) {
      printf("Error: Unable to retrieve a shader location.\n");
      return -1;
    }
  }
  shader[GLSL_UVMAP].Disable();

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

  // VAO
  glGenVertexArrays(1, &vao[VAO_IMAGE_PROC]);
  glGenVertexArrays(1, &vao[VAO_UVMAP]);

  GLuint vbo;
  GLuint vbo_uv;

  // VAO Binding
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
    glVertexAttribPointer(shader[GLSL_UVMAP].GetLocation("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(shader[GLSL_UVMAP].GetLocation("aPosition"));

    vbo_uv = makeVBO(sizeof(uvs), uvs);
    glVertexAttribPointer(shader[GLSL_UVMAP].GetLocation("aUV"), 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(shader[GLSL_UVMAP].GetLocation("aUV"));
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
  GLuint location = glGetUniformLocation(program, name);
//  assert(glGetError() == GL_NO_ERROR);
  glUniform1i(location, unit_no);
//  assert(glGetError() == GL_NO_ERROR);
  glActiveTexture(GL_TEXTURE0 + unit_no); // テクスチャユニット unit_no
  glBindTexture(GL_TEXTURE_2D, texture);
//  glBindSampler(unit_no, m_sampler);
}

void render_imgui()
{
  static bool open_dialog = false;

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  if (! open_dialog) {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(500.0, 220.0));
  }

  // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
  {
    static float f = 0.0f;
    static int counter = 0;

    open_dialog = ImGui::Begin("Parameters");
    ImGui::SliderInt("Wave Count", &vali_wave_count, 3.0f, 30.0f, "%d");
    ImGui::SliderFloat("Wave Amplitude", &val_wave_amp, 1.0f, 50.0f, "%.2f");
    ImGui::SliderFloat("Phase", &val_phase_deg, -90.0f, +90.0f, "%.2f");
    ImGui::End();
  }

  // Rendering
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void render_frame()
{
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

          glUniform2f(glGetUniformLocation(shader[GLSL_IMAGE_PROC].program, "uImageSize"),
                      m_texture[TEX_SRC].width,
                      m_texture[TEX_SRC].height);

          glUniform1f(glGetUniformLocation(shader[GLSL_IMAGE_PROC].program, "uWaveCount"), (float)vali_wave_count);
          glUniform1f(glGetUniformLocation(shader[GLSL_IMAGE_PROC].program, "uWaveAmp"), val_wave_amp);
          glUniform1f(glGetUniformLocation(shader[GLSL_IMAGE_PROC].program, "uPhaseDeg"), val_phase_deg);

          // drawcall
          glDrawArrays(GL_TRIANGLE_STRIP, 0, VERTS_COUNT4);
        }
        shader[GLSL_IMAGE_PROC].Disable();
      }
      glBindVertexArray(0);
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  }

//  saveTexture("test.png", m_texture[TEX_DST].width, m_texture[TEX_DST].height, m_texture[TEX_DST].id);

  // 表示用のビューポートを作成（ここでは同じ位置）
  glViewport(0, 0, m_texture[TEX_DST].width, m_texture[TEX_DST].height);
  {
    glBindVertexArray(vao[VAO_UVMAP]);
    {
      // シェーダプログラムの指定
      shader[GLSL_UVMAP].Enable();
      {
        bind_sampler(shader[GLSL_UVMAP].program, "image", 0, m_texture[TEX_DST].id);

        // drawcall
        glDrawArrays(GL_TRIANGLE_STRIP, 0, VERTS_COUNT4);
      }
      shader[GLSL_UVMAP].Disable();
    }
    glBindVertexArray(0);
  }

  // 後処理
//  glUseProgram(0);
//  glBindTexture(GL_TEXTURE_2D, 0);

  render_imgui();
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
      saveTexture("dst.png", m_texture[TEX_DST].width, m_texture[TEX_DST].height, m_texture[TEX_DST].id);
      break;

    }
  }
}

int main(void)
{
  if (! glfwInit() ) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    return 1;
  }

  const char* glsl_version = "#version 450";
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
  // glutCreateWindow() の後に、GPU情報を取得する。
  printf("OpenGL Vendor  = %s\n", glGetString(GL_VENDOR));
  printf("OpenGL GPU     = %s\n", glGetString(GL_RENDERER));
  printf("OpenGL Version = %s\n", glGetString(GL_VERSION));
  printf("GLSL   Version = %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif

#ifdef GLEW
  // OpenGL 拡張機能を使えるように！！
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

  //----------------------------------------------------------------------
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
//    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  //----------------------------------------------------------------------
  float text_fontsize = 20.0f; // フォントサイズ
  float icon_fontsize = 17.0f; // フォントサイズ
  ImFontConfig icons_config;

  // .ttfの文字の中からGetGlyphRangesJapaneseで指定された漢字などの文字をimguiの中で使えるようにします。
  io.Fonts->AddFontFromFileTTF(FONT_SRC, text_fontsize, NULL, io.Fonts->GetGlyphRangesJapanese());

  icons_config.MergeMode = true;
  icons_config.PixelSnapH = true;
  //----------------------------------------------------------------------

  while ( ! glfwWindowShouldClose (window) ) {
    render_frame();

    glfwSwapBuffers(window);
//    glfwPollEvents();
    glfwWaitEvents();
  }

  quit_opengl();
  glfwTerminate();
  return 0;
}

// 終了処理
void quit_opengl()
{
  printf("quit\n");

  // Cleanup ImGui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

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
