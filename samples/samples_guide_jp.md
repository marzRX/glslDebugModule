# glslDebugModule Sample Programs

glslDebugModuleの使い方を理解をできるように3つのサンプルを用意しました。

## サンプルプログラム一覧

### 2D プログラム

- `01_ripple`: 2D プログラム
- `02_pulsecircle`: ImGuiを使用した2D プログラム

### 3D プログラム

- `03_hellotriangle`: 3D プログラム
  - `03_hellotriangle_color_rot_debug1_clipping` : glslDebugModuleを適用しただけのバージョンで、クリッピング問題が発生します。
  - `03_hellotriangle_color_rot_debug2_clipping` : デバッグ情報の表示位置やフォントサイズを調整し、クリッピングを回避しようとしたバージョンです。
  - `03_hellotriangle_color_rot_debug3` : C++側のコード変更を伴い、背景と三角形の両方でデバッグ情報を表示することが可能です。この変更により、本プロジェクトではクリッピング問題を大幅に回避しています。ただし、この方法がすべての3Dプロジェクトで同様に効果的であるとは限らない点に注意してください。

それぞれのプログラムには、`_debug`接尾辞がついているものとそうでないものがあります。
`_debug`接尾辞がついているものは、glslDebugModuleを適用したプロジェクトを示します。
逆に、接尾辞がついていないものは、デバッグモジュール適用前のプロジェクトです。

## ビルド方法

OpenGL拡張を選ぶことができます。好きなものを選んでビルドできます。

### LinuxまたはMinGWでのビルド方法

#### GLFW + GLEW
GLEWを使用してビルドするには、以下のコマンドを実行します。

```bash
make
```

#### GLFW + GL3W
GL3Wを使用してビルドする場合は、以下のコマンドを使用します。

```bash
make GL3W=1
```

