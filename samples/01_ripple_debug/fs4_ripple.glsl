#version 450
layout(binding = 0) uniform sampler2D image;
layout(std140, binding = 0) uniform uBasicdata {
  vec2 img_size;
  vec2 dummy;
};

struct TRippleInfo {
  vec2 touch;
  float amp;
  float delta_kappa;
};

layout(std140, binding = 1) uniform uRippleset {
  TRippleInfo ripples[2];
};

uniform int uRippleCount;
// INSERT_UNIFORMS_HERE
layout(binding = 10) uniform sampler2D debug_digits; // Digit texture atlas
uniform int uDebugON=1; // Debug mode flag

layout(location = 0) out vec4 FragColor;

// INSERT_PROTOTYPES_HERE
void db_locate(int x, int y); // Set cursor location for debug text
void db_print(float val, int alpha); // Print debug value at current cursor location
vec4 db_mix(vec4 col); // Mix debug layer with the given fragment

float M_PI = 3.141592653589793238462643383;
float wavelength = 32.0;
//float amp = 32.0;

void main(void)
{
  vec2 pos = gl_FragCoord.xy;
  vec2 uv_base = vec2(pos / img_size); // テクスチャ座標を得る

  vec2 uv[8];
  vec2 uv_sum = vec2(0.0);

  db_locate(15, 6);
  //----------------------------------------------------------------------
  for (int i=0; i<uRippleCount; i++) {
    // 中心からの距離 R1 を求める // R1 は半径でもある
    vec2 rvec;
    rvec = pos-ripples[i].touch;
    float R1 = sqrt(rvec.x*rvec.x + rvec.y*rvec.y);

    db_print(ripples[i].touch.x, 1);
    db_print(ripples[i].touch.y, 1);

    // R1 と距離からθを得る
    float kappa = 2 * M_PI * R1 / wavelength;
    kappa -= ripples[i].delta_kappa;
    // 角度と振幅から新たな R1 を得る
    // R1' = R1 + sin(θ) * amp
//    float R11 = R1 + sin(kappa) * amp;
    float R11 = R1 + sin(kappa) * ripples[i].amp;

    // ベクトル(rx, ry) の角度を求める atan(y,x) = arctan2(x,y)
    float phi = atan(rvec.y, rvec.x);

    uv[i] = uv_base;
    // 「/ img_size.y」 は、ピクセル値からテクスチャ座標（0.0 ? 1.0）へのノーマライズ
    uv[i].x = ripples[i].touch.x/img_size.x + R11 * cos(phi) / img_size.x;
    uv[i].y = ripples[i].touch.y/img_size.y + R11 * sin(phi) / img_size.y;

    uv_sum = uv_sum + uv[i];
  }


  // ピクセル値を得る
  vec3 col = texture(image, uv_sum/uRippleCount).rgb;

//  FragColor = vec4(col, 1.0);
  FragColor = db_mix(vec4(col, 1.0));
}

// INSERT_DEBUG_FUNCTIONS_HERE
const float FONT_SIZE = 32.0; // Font size for debug text
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
