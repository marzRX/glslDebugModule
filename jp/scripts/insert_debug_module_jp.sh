#!/bin/sh

# 引数の確認
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <shader_file>"
    exit 1
fi

shader_file="$1"
backup_shader_file="${shader_file%.*}_backup.${shader_file##*.}"

# OpenGL 用デバッグモジュールと OpenGL ES 用デバッグモジュール
debug_module_gl="debug_module_gl_jp.glsl"
debug_module_gles="debug_module_gles_jp.glsl"

# 適切なデバッグモジュールを選択
debug_module_file=""
if [ -e "$debug_module_gl" ]; then
    debug_module_file="$debug_module_gl"
elif [ -e "$debug_module_gles" ]; then
    debug_module_file="$debug_module_gles"
else
    echo "Error: No debug module found."
    exit 1
fi

# デバッグモジュールの内容を取得
prototypes=$(sed -n '/^\/\/ BEGIN_PROTOTYPES$/,/^\/\/ END_PROTOTYPES$/p' "$debug_module_file" | sed '1d;$d')
debug_functions=$(sed -n '/^\/\/ BEGIN_DEBUG_FUNCTIONS$/,/^\/\/ END_DEBUG_FUNCTIONS$/p' "$debug_module_file" | sed '1d;$d')
uniforms=$(sed -n '/^\/\/ BEGIN_UNIFORMS$/,/^\/\/ END_UNIFORMS$/p' "$debug_module_file" | sed '1d;$d')

# バックアップを作成
cp "$shader_file" "$backup_shader_file"

# プロトタイプ宣言の挿入
sed -i "/\/\/ INSERT_PROTOTYPES_HERE/ {
    r /dev/stdin
}" "$shader_file" <<-EOF1
$prototypes
EOF1

# デバッグ関数の挿入
sed -i "/\/\/ INSERT_DEBUG_FUNCTIONS_HERE/ {
    r /dev/stdin
}" "$shader_file" <<-EOF2
$debug_functions
EOF2

# ユニフォーム変数の挿入
sed -i "/\/\/ INSERT_UNIFORMS_HERE/ {
    r /dev/stdin
}" "$shader_file" <<-EOF3
$uniforms
EOF3

# MinGW または Cygwin の場合、LF を CRLF に変換
unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    CYGWIN*)    machine=Cygwin;;
    MINGW*)     machine=MinGw;;
    *)          machine="UNKNOWN:${unameOut}"
esac

if [ "$machine" = "Cygwin" ] || [ "$machine" = "MinGw" ]; then
    sed -i 's/$/\x0d/g' "$shader_file"
fi
