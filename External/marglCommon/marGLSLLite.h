int marCreateGLSLProgram(GLuint *programID,
                         const char *vs_filename,
                         const char *fs_filename);

int marCreateGLSLProgram_withG(GLuint *programID,
                               const char *vs_filename,
                               const char *fs_filename,
                               const char *gs_filename);

int marCreateGLSLProgram_withT(GLuint *programID,
			       const char *vs_filename,
			       const char *fs_filename,
			       const char *hs_filename,
			       const char *ds_filename);

int marCreateGLSLProgram_withTG(GLuint *programID,
				const char *vs_filename,
				const char *fs_filename,
				const char *hs_filename,
				const char *ds_filename,
				const char *gs_filename);

//----------------------------------------------------------------------
// marCreateGLSLProgramMemory* for Memory(Resource)
//----------------------------------------------------------------------

int marCreateGLSLProgramMemory(GLuint *programID,
			       const unsigned char *vs_code,
			       const unsigned char *fs_code,
			       const int vs_codesize,
			       const int fs_codesize);

// int marCreateGLSLProgram_fragment(GLuint *programID, const char *fs_filename); // 仕様では動作不定
// SPIRV は、marSPIRV.{cpp,h}で
// int marCreateGLSLProgramSPIRV(GLuint *programID, const char *vs_filename, const char *fs_filename);

//int marLinkProgram(GLuint *programID);
void print_error_4createshader(unsigned int shadertype);

void marSetGLSLDir(const char *dir);


/*
  tessellation

  tessellation control shader: TCS? glslangValidator では、TESC
  tessellation evalution shader: TES? glslangValidator では、TESE

  ややこしいので、

  tessellation control shader: hull shader: hs
  tessellation evalution shader: domain shader: ds

  */
