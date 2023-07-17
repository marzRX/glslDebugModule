#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include "marMatrix2.h"
//#include <iostream>
//#include <iomanip>

double getrad(double degree)
{
  return 2.0 * M_PI * degree / 360.0;
}

double getdeg(double theta)
{
  return 360.0 * theta / (2.0 * M_PI);
}

void printMatrix(const smat4& matrix)
{
  const int N=4;
  for(int i = 0; i < N; ++i) {
    for(int j = 0; j < N; ++j) {
      printf("%10.2f ", matrix.m[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void printMatrix3(const smat3& matrix)
{
  const int N=3;
  for(int i = 0; i < N; ++i) {
    for(int j = 0; j < N; ++j) {
      printf("%10.2f ", matrix.m[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void printMatrix2(const smat2& matrix)
{
  const int N=2;
  for(int i = 0; i < N; ++i) {
    for(int j = 0; j < N; ++j) {
      printf("%10.2f ", matrix.m[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void printMatrix2d(const float (&m)[4][4])
{
  const int N=4;
  for(int i = 0; i < N; ++i) {
    for(int j = 0; j < N; ++j) {
      printf("%10.2f ", m[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

/*
void printMatrixOld(const smat4_old& matrix) {
  printf("%10.2f %10.2f %10.2f %10.2f\n", matrix.a0, matrix.a1, matrix.a2, matrix.a3);
  printf("%10.2f %10.2f %10.2f %10.2f\n", matrix.b0, matrix.b1, matrix.b2, matrix.b3);
  printf("%10.2f %10.2f %10.2f %10.2f\n", matrix.c0, matrix.c1, matrix.c2, matrix.c3);
  printf("%10.2f %10.2f %10.2f %10.2f\n", matrix.d0, matrix.d1, matrix.d2, matrix.d3);
  printf("\n");
}
*/

void matget_move(const float x, const float y, const float z, smat4& mat, const bool transpose)
{
  if (transpose) {
    for(int i = 0; i < 4; i++)
      for(int j = 0; j < 4; j++)
        mat.m[i][j] = 0;

    mat.m[0][0] = 1.0;
    mat.m[1][1] = 1.0;
    mat.m[2][2] = 1.0;
    mat.m[3][3] = 1.0;

    mat.m[3][0] = x;
    mat.m[3][1] = y;
    mat.m[3][2] = z;

  } else {
    for(int i = 0; i < 4; i++)
      for(int j = 0; j < 4; j++)
        mat.m[i][j] = 0;

    mat.m[0][0] = 1.0;
    mat.m[1][1] = 1.0;
    mat.m[2][2] = 1.0;
    mat.m[3][3] = 1.0;

    mat.m[0][3] = x;
    mat.m[1][3] = y;
    mat.m[2][3] = z;
  }
}

void matget_move(const float x, const float y, const float z, float (&m)[4][4], const bool transpose)
{
  if (transpose) {
    for(int i = 0; i < 4; i++)
      for(int j = 0; j < 4; j++)
        m[i][j] = 0;

    m[0][0] = 1.0;
    m[1][1] = 1.0;
    m[2][2] = 1.0;
    m[3][3] = 1.0;

    m[3][0] = x;
    m[3][1] = y;
    m[3][2] = z;

  } else {
    for(int i = 0; i < 4; i++)
      for(int j = 0; j < 4; j++)
        m[i][j] = 0;

    m[0][0] = 1.0;
    m[1][1] = 1.0;
    m[2][2] = 1.0;
    m[3][3] = 1.0;

    m[0][3] = x;
    m[1][3] = y;
    m[2][3] = z;
  }
}

// 拡大では、transpose は意味を持たない
void matget_zoom(const float x, const float y, const float z, smat4& mat, const bool transpose)
{
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      mat.m[i][j] = 0.0f;

  mat.m[0][0] = x;
  mat.m[1][1] = y;
  mat.m[2][2] = z;
  mat.m[3][3] = 1.0f;
}

// 拡大では、transpose は意味を持たない
void matget_zoom(const float x, const float y, const float z, float (&m)[4][4], const bool transpose)
{
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      m[i][j] = 0.0f;

  m[0][0] = x;
  m[1][1] = y;
  m[2][2] = z;
  m[3][3] = 1.0f;
}

void matget_rotate(const float theta, const bool is_axis_x, const bool is_axis_y, const bool is_axis_z, smat4& mat, const bool transpose)
{
  float sin_theta = sin(theta);
  float cos_theta = cos(theta);

  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      mat.m[i][j] = (i == j) ? 1.0f : 0.0f;

  if (is_axis_x) {
    mat.m[1][1] = cos_theta;
    mat.m[1][2] = transpose ? sin_theta : -sin_theta;
    mat.m[2][1] = transpose ? -sin_theta : sin_theta;
    mat.m[2][2] = cos_theta;
  } else if (is_axis_y) {
    mat.m[0][0] = cos_theta;
    mat.m[0][2] = transpose ? -sin_theta : sin_theta;
    mat.m[2][0] = transpose ? sin_theta : -sin_theta;
    mat.m[2][2] = cos_theta;
  } else if (is_axis_z) {
    mat.m[0][0] = cos_theta;
    mat.m[0][1] = transpose ? sin_theta : -sin_theta;
    mat.m[1][0] = transpose ? -sin_theta : sin_theta;
    mat.m[1][1] = cos_theta;
  }
}

void matget_rotate(const float theta, const bool is_axis_x, const bool is_axis_y, const bool is_axis_z, float (&m)[4][4], const bool transpose)
{
  if (transpose) {
    float sin_theta = sin(theta);
    float cos_theta = cos(theta);

    for(int i = 0; i < 4; i++)
      for(int j = 0; j < 4; j++)
        m[i][j] = (i == j) ? 1.0f : 0.0f;

    if (is_axis_x) {
      m[1][1] = cos_theta;
      m[1][2] = sin_theta;
      m[2][1] = -sin_theta;
      m[2][2] = cos_theta;
    } else if (is_axis_y) {
      m[0][0] = cos_theta;
      m[0][2] = -sin_theta;
      m[2][0] = sin_theta;
      m[2][2] = cos_theta;
    } else if (is_axis_z) {
      m[0][0] = cos_theta;
      m[0][1] = sin_theta;
      m[1][0] = -sin_theta;
      m[1][1] = cos_theta;
    }
  } else {
    float sin_theta = sin(theta);
    float cos_theta = cos(theta);

    for(int i = 0; i < 4; i++)
      for(int j = 0; j < 4; j++)
        m[i][j] = (i == j) ? 1.0f : 0.0f;

    if (is_axis_x) {
      m[1][1] = cos_theta;
      m[1][2] = -sin_theta;
      m[2][1] = sin_theta;
      m[2][2] = cos_theta;
    } else if (is_axis_y) {
      m[0][0] = cos_theta;
      m[0][2] = sin_theta;
      m[2][0] = -sin_theta;
      m[2][2] = cos_theta;
    } else if (is_axis_z) {
      m[0][0] = cos_theta;
      m[0][1] = -sin_theta;
      m[1][0] = sin_theta;
      m[1][1] = cos_theta;
    }
  }
}

void matget_lookat(const vvec3 &cameraPos, const vvec3 &lookAtPos, const vvec3 &up, smat4 &mat, const bool transpose)
{
  // カメラ前方へのベクトル
  vvec3 f = lookAtPos - cameraPos;
  f.normalize();

  // カメラ横方向へのベクトル
  vvec3 s;
  if (abs(f.x) < 0.001f && abs(f.z) < 0.001f) {  // If the camera is looking straight up or down
    s = cross_product(vvec3(0.0f, 0.0f, 1.0f), f);  // Use the z-axis as the up vector
  } else {
    s = cross_product(f, up);
  }
  s.normalize();

  // アップベクトル
  // 引数のアップベクトルは、アップベクトルとして期待しているものの、それほど信用してはいない
  vvec3 u = cross_product(s, f);

  smat4 m_cam;

  if (transpose) {
    m_cam.m[0][0] = s.x;
    m_cam.m[0][1] = s.y;
    m_cam.m[0][2] = s.z;
    m_cam.m[1][0] = u.x;
    m_cam.m[1][1] = u.y;
    m_cam.m[1][2] = u.z;
    m_cam.m[2][0] = -f.x;
    m_cam.m[2][1] = -f.y;
    m_cam.m[2][2] = -f.z;
    m_cam.m[3][3] = 1.0;
  } else {
    m_cam.m[0][0] = s.x;
    m_cam.m[1][0] = s.y;
    m_cam.m[2][0] = s.z;
    m_cam.m[0][1] = u.x;
    m_cam.m[1][1] = u.y;
    m_cam.m[2][1] = u.z;
    m_cam.m[0][2] = -f.x;
    m_cam.m[1][2] = -f.y;
    m_cam.m[2][2] = -f.z;
    m_cam.m[3][3] = 1.0;
  }

  smat4 m_offset;
  m_offset.identity_self();
  if (transpose) {
    m_offset.m[3][0] = -cameraPos.x;
    m_offset.m[3][1] = -cameraPos.y;
    m_offset.m[3][2] = -cameraPos.z;
    m_offset.m[3][3] = 1.0;
  } else {
    m_offset.m[0][3] = -cameraPos.x;
    m_offset.m[1][3] = -cameraPos.y;
    m_offset.m[2][3] = -cameraPos.z;
    m_offset.m[3][3] = 1.0;
  }

  mat = m_offset * m_cam;
}

void matget_ortho(float left, float right,
                  float bottom, float top,
                  float z_near, float z_far, smat4& mat, const bool transpose)
{
  // Avoid division by zero
  if (top==bottom) {
    top=bottom+1E+12;
  }
  if (right==left) {
    right=left+1E+12;
  }
  if (z_far==z_near) {
    z_far=z_near+1E+12;
  }

  mat = smat4();
  if(transpose) {
      mat.m[0][0] = 2.0/(right-left);
      mat.m[1][1] = 2.0/(top-bottom);
      mat.m[2][2] = -2.0/(z_far-z_near);
      mat.m[3][0] = -(right+left)/(right-left);
      mat.m[3][1] = -(top+bottom)/(top-bottom);
      mat.m[3][2] = -(z_far+z_near)/(z_far-z_near);
      mat.m[3][3] = 1.0;
  } else {
      mat.m[0][0] = 2.0/(right-left);
      mat.m[0][3] = -(right+left)/(right-left);
      mat.m[1][1] = 2.0/(top-bottom);
      mat.m[1][3] = -(top+bottom)/(top-bottom);
      mat.m[2][2] = -2.0/(z_far-z_near);
      mat.m[2][3] = -(z_far+z_near)/(z_far-z_far);
      mat.m[3][3] = 1.0;
  }
}

void matget_frustum(float left, float right,
                    float bottom, float top,
                    float z_near, float z_far, smat4 &mat, bool transpose)
{
    mat.clear();

    if (transpose) {
        mat.m[0][0] = 2.0f*z_near/(right-left);
        mat.m[2][0] = (right+left)/(right-left);
        mat.m[1][1] = 2.0f*z_near/(top-bottom);
        mat.m[2][1] = (top+bottom)/(top-bottom);
        mat.m[2][2] = -(z_far+z_near)/(z_far-z_near);
        mat.m[3][2] = -2.0f*z_far*z_near/(z_far-z_near);
        mat.m[2][3] = -1.0f;
    } else {
        mat.m[0][0] = 2.0f*z_near/(right-left);
        mat.m[0][2] = (right+left)/(right-left);
        mat.m[1][1] = 2.0f*z_near/(top-bottom);
        mat.m[1][2] = (top+bottom)/(top-bottom);
        mat.m[2][2] = -(z_far+z_near)/(z_far-z_near);
        mat.m[2][3] = -2.0f*z_far*z_near/(z_far-z_near);
        mat.m[3][2] = -1.0f;
    }
}

void matget_perspective(float fovY, float aspect, float z_near, float z_far, smat4 &m, bool transpose)
{
    float rad_fovy = (fovY / 360.0f) * 2 * M_PI;
    float f = 1.0f / tan(rad_fovy / 2.0f);

    m.clear();

    if (transpose) {
        m.m[0][0] = f / aspect;
        m.m[1][1] = f;
        m.m[2][2] = (z_far + z_near) / (z_near - z_far);
        m.m[2][3] = 2.0f * z_far * z_near / (z_near - z_far);
        m.m[3][2] = -1.0f;
    } else {
        m.m[0][0] = f / aspect;
        m.m[1][1] = f;
        m.m[2][2] = (z_far + z_near) / (z_near - z_far);
        m.m[3][2] = 2.0f * z_far * z_near / (z_near - z_far);
        m.m[2][3] = -1.0f;
    }
}

void matget_identity(float (&m)[4][4]) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

void mat_copy(float src[4][4], float dest[4][4])
{
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      dest[i][j] = src[i][j];
    }
  }
}

void mat_mul(float a[4][4], float b[4][4], float result[4][4])
{
  float temp[4][4] = {0};
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        temp[i][j] += a[i][k] * b[k][j];
      }
    }
  }
  mat_copy(temp, result);  // resultに計算結果をコピー
}

// Function for matrix transpose
smat4 transpose(smat4 mat)
{
  smat4 result;
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      result.m[j][i] = mat.m[i][j];
  return result;
}

smat3 transpose3(smat3 mat)
{
  smat3 result;
  for(int i = 0; i < 3; i++)
    for(int j = 0; j < 3; j++)
      result.m[j][i] = mat.m[i][j];
  return result;
}

smat2 transpose2(smat2 mat)
{
  smat2 result;
  for(int i = 0; i < 2; i++)
    for(int j = 0; j < 2; j++)
      result.m[j][i] = mat.m[i][j];
  return result;
}

float dot_product(vvec3 v1, vvec3 v2)
{
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vvec3 cross_product(vvec3 v1, vvec3 v2)
{
  vvec3 ret;
  ret.x = v1.y * v2.z - v1.z * v2.y;
  ret.y = v1.z * v2.x - v1.x * v2.z;
  ret.z = v1.x * v2.y - v1.y * v2.x;
  return ret;
}
