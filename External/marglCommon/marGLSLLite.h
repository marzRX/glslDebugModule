#pragma once

#include <vector>

// Global debug flag
extern bool MAR_GLSL_DEBUG;

// Shader information structs
struct TShaderInfo {
  const char* filename;
  unsigned int shadertype;
};

// Create GLSL program
int marCreateGLSLProgram(unsigned int *programID,
                         const char *vs_filename,
                         const char *fs_filename,
                         std::vector<TShaderInfo> shaders_4gt = std::vector<TShaderInfo>());

// Function to set the GLSL directory
void marSetGLSLDir(const char *dir);

/*
  テッセレーションシェーダーの略語（Tessellation Shader Abbreviations）:

  テッセレーション シェーダには、glslangValidator による TCS/TESE など、さまざまな略語がありますが、
  DirectX シェーダーの命名規則を採用して簡素化します。

  While there are various abbreviations for tessellation shaders, such as TCS/TESE by glslangValidator, 
  we will simplify it by adopting the DirectX shader naming conventions:
  

  Tessellation Control Shader (TESC in glslangValidator) -> hull shader: hs
  Tessellation Evaluation Shader (TESE in glslangValidator) -> domain shader: ds
*/
