#include <iostream>
#include <map>
#include <string>
#include <cassert>
#include <GL/gl.h>

class TShader {
public:
  TShader() = default;
  ~TShader() {
    location.clear();
  }

  void Enable() {
    glUseProgram(program);
  }

  void Disable() {
    glUseProgram(0);
  }

  void SetLocation(const std::string& name, int loc = -1) {
    if (loc == -1) {
      if (name[0] == 'a') {
        loc = glGetAttribLocation(program, name.c_str());
      } else if (name[0] == 'u') {
        loc = glGetUniformLocation(program, name.c_str());
      }
    }
    if (location.count(name) == 0) {
      location[name] = loc;
    }
  }

  int GetLocation(const std::string& name) {
    if (location.count(name) > 0) {
      return location[name];
    }

    // Print an error message and assert (if debugging)
    printf("Error: Could not find location for name: %s\n", name.c_str());
    assert(false);  // Will cause program to stop if debugging

    return -1;
  }

  // PrintLocations(): ロケーションのマッピング状態を表示し、エラーの有無をチェック
  // 戻り値はエラーがあれば -1、なければ 0
  int PrintLocations() {
    bool hasError = false;

    // Loop over the map
    for (auto const& pair: location) {
        printf("Name: %s, Location: %d\n", pair.first.c_str(), pair.second);
        if (pair.second < 0) {
            hasError = true;
        }
    }
    return hasError ? -1 : 0;
  }

  // 簡単のためユニフォーム変数へのアクセスはこちらを使用する
  // 例) glUniform2f(shader[GLSL_IMAGE_PROC]["uImageSize"], img_width, img_height);
  int operator[](const char* name) {
    if (location.count(name) > 0) {
      return location[name];
    } else {
      SetLocation(name);
      return GetLocation(name);
    }
  }

  //----------------------------------------------------------------------
  // deprecated として分類した関数群（既存のコードをサポートするために残しておく）
  //----------------------------------------------------------------------
  [[deprecated("SetLocation() を使用してください。")]]
    // Use SetLocation() instead.
    void Activate(const char* name, int loc = -1) {
      SetLocation(name, loc);
    }

  [[deprecated("PrintLocations() を使用してください。")]]
    // Use PrintLocations() instead.
    int Print() {
      return PrintLocations();
    }

  [[deprecated("PrintLocations()の戻り値をチェックしてエラーを判定してください。")]]
    // Please check the return value of PrintLocations() to determine errors.
    int CheckError() {
      // ダミーで正常を返却
      return 0;
    }

  [[deprecated("clear()は不要です。リソースはデストラクタによって解放されます。")]]
    // clear() is no longer required as resources are released by the destructor.
    void clear() {
      // 何もしない
    }


private:
  std::map<std::string, int> location;
public:
  unsigned int program;
};
