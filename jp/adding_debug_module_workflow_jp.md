# デバッグモジュールを自身のプロジェクトに追加するまでの手順

## GLSL（シェーダー）での手順

1. **スクリプトのコピー**

    デバッグ対象のシェーダーと同じディレクトリにデバッグモジュールとスクリプトをコピーします。
    
    - デバッグモジュール: `debug_module_gl_jp.glsl`
    - スクリプト: `insert_debug_module_jp.sh`


2. **デバッグディレクティブの追加**

    デバッグ対象のシェーダーに３つのデバッグディレクティブを追加します。それぞれ、ユニフォーム用、プロトタイプ用、ファンクション用のディレクティブです。

    ```glsl
    // INSERT_UNIFORMS_HERE
    // INSERT_PROTOTYPES_HERE
    // INSERT_DEBUG_FUNCTIONS_HERE
    ```

    デバッグディレクティブは、デバッグモジュールの各パートを展開するためのものです。

3. **スクリプトの実行**

    スクリプトを実行します。
    
    ```bash
    sh insert_debug_module_jp.sh <your_shader_file>
    ```

4. **シェーダーファイルの編集**

    4.1 **db_mix() の追加**
    
    出力部分のコード、`FragColor = your_fragment` を `FragColor = db_mix(your_fragment)` に置き換えます。

    4.2 **db_print() の追加**
    
    `db_print(123.45, 1);`

5. **動作確認**

    まだ、C++ 側の準備がまだですが、この段階で実行してください。数字こそ描画されませんが、矩形が描画されてるエリアがあることを確認します。

## C++ での手順

数字テクスチャをシェーダーに渡すための処理を追加します。

1. **ファイル名を保持する定数を用意**
    ```c++
    const char *FILE_DEBUG_DIGITS = "your_db_digits.png";
    ```

2. **テクスチャIDを格納する変数を用意**
    ```c++
    gluint texture_debug_digits;
    ```

3. **テクスチャをロード**
    ```c++
    texture_debug_digits = loadTextureEx(FILE_DEBUG_DIGITS);
    ```

4. **シェーダーへテクスチャを渡す**

    シェーダーはデバッグ用のテクスチャをスロット10番で渡してもらうことを期待しています。

    ```c++
    glUniform1i(glGetUniformLocation(programID, "debug_digits"), 10);
    glActivateTexture(GL_TEXTURE0 + 10);
    glBindTexture(GL_TEXTURE_2D, texture_debug_digits);
    ```

以上がデバッグモジュールを追加するための作業手順となります。ビルドして、db_print()で指定した、123.45 が表示されることを確認します。
