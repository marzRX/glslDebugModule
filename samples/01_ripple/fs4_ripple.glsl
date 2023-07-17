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

layout(location = 0) out vec4 FragColor;

float M_PI = 3.141592653589793238462643383;
float wavelength = 32.0;
//float amp = 32.0;

void main(void)
{
  vec2 pos = gl_FragCoord.xy;
  vec2 uv_base = vec2(pos / img_size); // テクスチャ座標を得る

  vec2 uv[8];
  vec2 uv_sum = vec2(0.0);

  //----------------------------------------------------------------------
  for (int i=0; i<uRippleCount; i++) {
    // 中心からの距離 R1 を求める // R1 は半径でもある
    vec2 rvec;
    rvec = pos-ripples[i].touch;
    float R1 = sqrt(rvec.x*rvec.x + rvec.y*rvec.y);

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

  FragColor = vec4(col, 1.0);
}
