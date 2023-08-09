# Procedure to Add the Debug Module to Your Project

## Procedures in GLSL (Shaders)

1. **Script Copying**

    Copy the script and the debug module to the same directory as the shader you want to debug.
    
    - Script: `insert_debug_module.sh`
    - Debug Module: `debug_module_gl.glsl`

2. **Adding Debug Directives**

    Add three debug directives to the shader you want to debug. These are for uniforms, prototypes, and functions, respectively.

    ```glsl
    // INSERT_UNIFORMS_HERE
    // INSERT_PROTOTYPES_HERE
    // INSERT_DEBUG_FUNCTIONS_HERE
    ```

    Debug directives are used to expand each part of the debug module.

3. **Script Execution**

    Execute the script.

    ```bash
    sh insert_debug_module.sh <your_shader_file>
    ```

4. **Shader File Editing**

    4.1 **Addition of db_mix()**
    
    Replace the output part of the code, `FragColor = your_fragment`, with `FragColor = db_mix(your_fragment)`.

    4.2 **Addition of db_print()**
    
    `db_print(123.45, 1);`

5. **Operation Confirmation**

    Even though the preparation on the C++ side is not ready yet, run at this stage. You won't see any numbers drawn, but make sure there is an area where a rectangle is drawn.

## Procedures in C++

Add processes to pass the numeric texture to the shader.

1. **Prepare a constant to hold the file name**
    ```c++
    const char *FILE_DEBUG_DIGITS = "your_db_digits.png";
    ```

2. **Prepare a variable to store the texture ID**
    ```c++
    GLuint texture_debug_digits;
    ```

3. **Load the texture**
    ```c++
    texture_debug_digits = loadTextureEx(FILE_DEBUG_DIGITS);
    ```

4. **Pass the texture to the shader**

    The shader expects to receive the debug texture at slot 10.

    ```c++
    glUniform1i(glGetUniformLocation(programID, "debug_digits"), 10);
    glActivateTexture(GL_TEXTURE0 + 10);
    glBindTexture(GL_TEXTURE_2D, texture_debug_digits);
    ```

This completes the procedure to add the debug module. Build and confirm that the number 123.45, specified with db_print(), is displayed.
