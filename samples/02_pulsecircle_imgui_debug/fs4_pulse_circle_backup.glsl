#version 450
layout(location = 0) out vec4 fragcolor;
uniform sampler2D image;
uniform vec2 uImageSize;
uniform float uWaveCount = 12.0;
uniform float uWaveAmp = 10.0;
uniform float uGray = 1.0;
uniform float uSizeRatio = 100.0;
// INSERT_UNIFORMS_HERE

float M_PI = 3.141592653589793238462643383;

// INSERT_PROTOTYPES_HERE

void main(void)
{
  vec2 pos = gl_FragCoord.xy;
  // テクスチャ座標を得る
  vec2 texCoord = vec2(pos / vec2(uImageSize));

  float amp = uWaveAmp;

  float cx, cy;
  cx = uImageSize.x / 2.0;
  cy = uImageSize.y / 2.0;

  // 中心からの距離（であり半径でもある）R1 を求める
  float rx, ry;
  rx = pos.x-cx;
  ry = pos.y-cy;
  float R1 = sqrt(rx*rx + ry*ry);

  float phi = atan(ry, rx);
  // M_PI/2.0 は 0度のところにカーブのピークをもってくるためのオペレーション
  float theta = phi * uWaveCount + M_PI / 2.0;

  // 変位前のソースを求める
  float R0 = R1 - sin(theta)*amp;
  // 画像サイズに収まるように
  R0 = R0 * (cx+amp)/cx;

  // アンプによるサイズ調整を100%として、サイズ調整を行う
  R0 = R0 * 100.0 / uSizeRatio;


  float rx0, ry0;
  rx0 = cos(phi)*R0;
  ry0 = sin(phi)*R0;

  float gray = uGray;
  float igray = abs(uGray - 1.0);

  vec3 col;
  if (R0 < cx) {
    col = vec3(gray);
  } else {
    col = vec3(igray);
  }

  fragcolor = vec4(col, 1.0);
}

// INSERT_DEBUG_FUNCTIONS_HERE
