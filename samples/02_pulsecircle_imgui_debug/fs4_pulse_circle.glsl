#version 450
layout(location = 0) out vec4 FragColor;
uniform sampler2D image;
uniform vec2 uImageSize;
uniform float uWaveCount = 12.0;
uniform float uWaveAmp = 10.0;
uniform float uPhaseDeg = 0.0;
uniform float uGray = 1.0;
uniform float uSizeRatio = 100.0;
// INSERT_UNIFORMS_HERE
layout(binding = 10) uniform sampler2D debug_digits; // Digit texture atlas
uniform int uDebugON=1; // Debug mode flag

float M_PI = 3.141592653589793238462643383;

// INSERT_PROTOTYPES_HERE
void db_locate(int x, int y); // Set cursor location for debug text
void db_print(float val, int alpha); // Print debug value at current cursor location
vec4 db_mix(vec4 col); // Mix debug layer with the given fragment

float getrad(float degree)
{
  return 2.0 * M_PI * degree / 360.0;
}

void main(void)
{
  vec2 pos = gl_FragCoord.xy;
  // テクスチャ座標を得る
  vec2 texCoord = vec2(pos / vec2(uImageSize));

  float amp = uWaveAmp;

  db_print(uWaveCount, 1);
  db_print(uWaveAmp, 1);
  db_print(uPhaseDeg, 1);

  float cx, cy;
  cx = uImageSize.x / 2.0;
  cy = uImageSize.y / 2.0;

  // 中心からの距離（であり半径でもある）R1 を求める
  float rx, ry;
  rx = pos.x-cx;
  ry = pos.y-cy;
  float R1 = sqrt(rx*rx + ry*ry);

  float phase_theta = getrad(uPhaseDeg);
  db_print(phase_theta, 1);

  float phi = atan(ry, rx);
  // M_PI/2.0 は 0度のところにカーブのピークをもってくるためのオペレーション
  float theta = (phi+phase_theta) * uWaveCount + M_PI / 2.0;

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


//  FragColor = vec4(col, 1.0);
  FragColor = db_mix(vec4(col, 1.0));
}

// INSERT_DEBUG_FUNCTIONS_HERE
const float FONT_SIZE = 64.0; // Font size for debug text
const float CONTRAST = 0.95; // Contrast for debug text
const int KETA = 5; // Number of digits to display
const int IS_3D = 0; // 3D mode flag

int digitsArr[KETA]; // Array to store digits
vec4 debugLayer; // Debug layer for mixing with the fragment
float loc_x = 0.0; // X-coordinate of cursor location
float loc_y = 0.0; // Y-coordinate of cursor location
int loc_reset = 0; // Reset flag for cursor location

// Function to set the display position
void db_locate(int x, int y)
{
  loc_x = x;
  loc_y = y;
  loc_reset = 0;
}

// Function to display debug values on the screen
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

// Function to mix the debug layer with the regular fragment
vec4 db_mix(vec4 yourFragment)
{
  return mix(yourFragment, debugLayer, debugLayer.a * uDebugON);
}
