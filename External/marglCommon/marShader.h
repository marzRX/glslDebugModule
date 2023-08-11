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

  // ＠＠既存のコードをサポートするために残しておく
  [[deprecated("Use SetLocation() and GetLocation() instead.")]]
  void Activate(const char* name, int loc = -1) {
    SetLocation(name, loc);
  }

  [[deprecated("Use GetLocation() instead.")]]
  int operator[](const char* name) {
    return GetLocation(name);
  }

private:
  std::map<std::string, int> location;
public:
  unsigned int program;
};
