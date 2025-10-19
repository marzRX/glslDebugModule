# glslDebugModule

glslDebugModule is a GLSL debugging assistance module intended for OpenGL 2D programs. By integrating this module into your program under development, it aims to aid in the debugging of GLSL. It's particularly suited for applications like 2D image processing. glslDebugModule operates on both Windows and Linux.

(Note: Although it can be used with 3D programs, debugging information may be limited due to clipping.)

## Screenshots
![Pulse Circle Demo](pulsecircle_demo.gif)

## Setup
The setup mainly consists of two steps: GLSL setup and C++ setup.

Also, for a detailed workflow on making your developing GLSL debuggable, please refer to [adding_debug_module_workflow.md](adding_debug_module_workflow.md).

### GLSL Setup:
You will need to add the debug module to your GLSL. By using the debug module insertion script, you can easily add the debug module to GLSL. For specific methods, please refer to [insert_debug_module_manual.md](scripts/insert_debug_module_manual.md).

### C++ Setup:
On the C++ side, you will need to pass a number texture atlas to GLSL. This process is a necessary step to display the GLSL debug information properly. Code examples will be shown at the end of this manual.

## Functions provided by glslDebugModule
- db_locate(): Specifies the position where you want to display the debug information.
- db_print(): Specifies the value you want to display as debug information.
- db_mix(): Replaces the fragment color definition FragColor = your_fragment with FragColor = db_mix(your_fragment). This function ensures that the debug information is properly displayed on the screen.

## Usage
First, use db_mix() to replace the fragment color definition FragColor = your_fragment with FragColor = db_mix(your_fragment).

Next, call db_print() for the data (float type) you want to display as debug information. For example, it would be like this:
```cpp
db_print(123.45, 1);
```
Since the position for displaying debug information automatically moves to the next line, there is no need to explicitly call db_locate().

## Specifications

- Use it in a fragment shader.
- The value has 3 digits in the integer part and 2 digits in the decimal part. The displayable value range is -999.99 to +999.99.
- The sign can be identified by the color of the characters. Green for positive and pink for negative.

## Precautions

- Only uniquely determined values can be checked. You cannot see values that differ for each fragment, such as `gl_FragCoord.x`.
- It cannot be used inside if statements.
- If the same shader is called multiple times, each will perform a debug print, causing the display to break. To avoid this, use the uniform variable `uDebugON`. Set `uDebugON=1` only for the draw call you want to enable debug printing, and set `uDebugON=0` for all other draw calls.

## C++ Code Example

```cpp
// Pass the uniform variables
GLuint debug_digits_location = glGetUniformLocation(shader[GLSL_YOUR_SHADER].program, "debug_digits");
glUniform1i(debug_digits_location, 10);
glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, m_texture[TEX_DEBUG_DIGITS]);
glBindSampler(1, m_sampler);

GLuint uDebugON_location = glGetUniformLocation(shader[GLSL_YOUR_SHADER].program, "uDebugON");
glUniform1i(uDebugON_location, 1);
```

Assume that the texture atlas of numbers is loaded in m_texture[TEX_DEBUG_DIGITS].

## Additional screenshots
![Ripple Demo](ripple_demo.gif)<br>
*In this demo, the mouse click position is displayed in GLSL.(Linux Mint)*
<br><br><br>
![Hello Triangle Demo](hellotriangle_demo.gif)<br>
*In 3D programs, the debug information is clipped.(Windows 7)*

## Acknowledgements

This software uses the following open source libraries:

- [ImGui](https://github.com/ocornut/imgui)
- [stb](https://github.com/nothings/stb)
- [gl3w](https://github.com/skaslev/gl3w)

In addition, it uses the digital font [mplus-1p-medium.ttf](https://fonts.google.com/specimen/M+PLUS+1p).  
The sample image used in the demonstration was sourced from Pixabay. We extend our gratitude to the Pixabay community and its contributors.