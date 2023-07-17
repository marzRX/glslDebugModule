// BEGIN_UNIFORMS
layout(binding = 10) uniform sampler2D debug_digits; // デバッグ用の数字テクスチャ
uniform int uDebugON=1; // デバッグモードの ON/OFF
// END_UNIFORMS

// BEGIN_PROTOTYPES
void db_locate(int x, int y); // デバッグ表示位置を設定
void db_print(float val, int alpha); // デバッグ値を画面に表示
vec4 db_mix(vec4 col); // デバッグレイヤーと通常のフラグメントを合成
// END_PROTOTYPES

// BEGIN_DEBUG_FUNCTIONS
const float FONT_SIZE = 64.0; // フォントサイズ
const float CONTRAST = 0.95; // コントラスト
const int KETA = 5; // 桁数 [DO NOT EDIT]
const int IS_3D = 0; // 3D表示かどうか

int digitsArr[KETA]; // 桁ごとの数字を格納
vec4 debugLayer; // デバッグレイヤー
float loc_x = 0.0; // 表示位置 x
float loc_y = 0.0; // 表示位置 y
int loc_reset = 0; // 表示位置リセット用フラグ

// 表示位置を設定する関数
void db_locate(int x, int y)
{
  loc_x = x;
  loc_y = y;
  loc_reset = 0;
}

// デバッグ値を画面に表示する関数
void db_print(float debugValue, int alpha)
{
  loc_y += float(uint(int(loc_reset--)) >> 31);

  vec2 uv1 = gl_FragCoord.xy / vec2(FONT_SIZE * 5.0, FONT_SIZE) + vec2(-loc_x/5.0,-loc_y);
  uv1.y = IS_3D * (1.0-uv1.y * 2.0)+uv1.y;
  int inRect = int((abs(uv1.x-0.5)<=0.5) && (abs(uv1.y-0.5)<=0.5));

  float x_float = (abs(debugValue) + 0.005) * 100.0;
  uint flow = uint(x_float >= 99999.0);
  int x = int(x_float) * (int(flow)^1)+99999 * int(flow);

  int ketamask_list[] = {10000, 1000, 100, 10, 1};
  int sum = 0;
  for (int i=0; i<KETA; i++) {
    int ketamask = ketamask_list[i];
    digitsArr[i] = (x - sum) / ketamask;
    sum += digitsArr[i] * ketamask;
  }

  int digit = digitsArr[int(uv1.x * KETA)];
  vec2 uv0 = (vec2(fract(uv1.x * KETA), uv1.y) + vec2(mod(int(digit), 4), int(digit) / 4)) * 0.25;

  float gray = texture(debug_digits, uv0).g;
  gray += (1.0-2.0 * gray) * (uint(int((0.6-uv1.x) * 1e06)) >> 31);
  uint minusFlag = uint(abs(debugValue) != debugValue);
  float redAndBlue = gray * minusFlag;
  float green = gray * (minusFlag^1);
  debugLayer += vec4(vec3(redAndBlue, green, redAndBlue) * CONTRAST, (1.0-(1.0-gray) * alpha)) * inRect;
}

// デバッグレイヤーと通常のフラグメントを合成する関数
vec4 db_mix(vec4 yourFragment)
{
  return mix(yourFragment, debugLayer, debugLayer.a * uDebugON);
}
// END_DEBUG_FUNCTIONS
