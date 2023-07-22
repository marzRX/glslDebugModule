# glslDebugModule Sample Programs

To help you understand how to use glslDebugModule, we have prepared three sample programs.

## Sample Programs List

### 2D programs

- `01_ripple`: 2D program.
- `02_pulsecircle`: 2D program with ImGui.

### 3D programs

- `03_hellotriangle`: 3D program.
  - `03_hellotriangle_color_rot_debug1_clipping`: This version simply applies the glslDebugModule, and a clipping problem occurs.
  - `03_hellotriangle_color_rot_debug2_clipping`: This version attempts to avoid clipping by adjusting the display position and font size of debug information.
  - `03_hellotriangle_color_rot_debug3`: This version involves changes to the C++ side of the code, allowing for the display of debug information on both the background and the triangle. This modification significantly avoids the clipping problem in this project. However, please note that this method may not be equally effective in all 3D projects.

Each program has versions with and without the `_debug` suffix. 
The ones with the `_debug` suffix are projects with the glslDebugModule applied.
On the other hand, those without the suffix are projects before the debug module is applied.

## Build Instructions

You can choose the OpenGL extension. You can build with whichever you prefer.

### Building on Linux or MinGW

#### GLFW + GLEW
To build with GLEW, execute the following command:

```bash
make
```
#### GLFW + GL3W
To build with GL3W, execute the following command:

```bash
make GL3W=1
```

