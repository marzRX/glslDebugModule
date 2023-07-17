# Debug Module Integration Script Manual

## Usage

1. Copy the script and the debug module to the same directory as the shader.
2. Add directives to the shader.
3. Run the script.
sh insert_debug_module.sh <shader_file>

* `<shader_file>`: The shader file you want to integrate the debug module into.

## Description

This script automatically inserts the appropriate debug module (either OpenGL or OpenGL ES) into the specified shader file. The debug module should be placed in the same directory as the script.

The script searches for the following directives in the shader file:

- `// INSERT_UNIFORMS_HERE`: This directive will be replaced with the uniform variables from the debug module.
- `// INSERT_PROTOTYPES_HERE`: This directive will be replaced with the function prototypes from the debug module.
- `// INSERT_DEBUG_FUNCTIONS_HERE`: This directive will be replaced with the debug functions from the debug module.

After running the script, the original shader file will be updated with the debug module's code, and a backup of the original shader file will be created with a `_backup` suffix before the file extension.


## Notes

- The script will be converted to CRLF in Windows environments such as MinGW or Cygwin, and will remain as LF in other environments.
- Use the debug module according to the target. For OpenGL ES, use the script and debug_module_gles.glsl.