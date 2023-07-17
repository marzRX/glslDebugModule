#pragma one

#include <cstdio>
#include <math.h>
#include <initializer_list>

double getrad(double degree);
double getdeg(double theta);

//----------------------------------------------------------------------
// Versatile VECtor: 多次元量変数
//----------------------------------------------------------------------
struct vvec4
{
  float x,y,z,w;
};

// xyz(rgb) components
struct vvec3
{
  float x,y,z;

  // 3つのfloat値を引数に取るコンストラクタ
  vvec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) 
       : x(x), y(y), z(z) {}
/*
  vvec3() {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
  }
*/
  // 四則演算
  vvec3 operator + (const vvec3 &v) const {
    return vvec3{x+v.x, y+v.y, z+v.z};
  }
  vvec3 operator - (const vvec3 &v) const {
    return vvec3{x-v.x, y-v.y, z-v.z};
  }
  vvec3 operator * (const vvec3 &v) const {
    return vvec3{x*v.x, y*v.y, z*v.z};
  }
  vvec3 operator / (const vvec3 &v) const {
    return vvec3{x/v.x, y/v.y, z/v.z};
  }

  // 代入
  vvec3 &operator = (const vvec3 &v) {
    x=v.x;
    y=v.y;
    z=v.z;
    return *this;
  } // 何故かセミコロンがないと次からのインデントが狂う
  vvec3 &operator += (const vvec3 &v) {
    x+=v.x;
    y+=v.y;
    z+=v.z;
    return *this;
  }
  vvec3 &operator -= (const vvec3 &v) {
    x-=v.x;
    y-=v.y;
    z-=v.z;
    return *this;
  }
  vvec3 &operator *= (const vvec3 &v) {
    x*=v.x;
    y*=v.y;
    z*=v.z;
    return *this;
  }
  vvec3 &operator /= (const vvec3 &v) {
    x/=v.x;
    y/=v.y;
    z/=v.z;
    return *this;
  }

  // 比較
  bool operator == (const vvec3 &v) const {
    return (x==v.x && y==v.y && z==v.z);
  }

  // ベクトルの長さを計算するメソッド
  float length() {
    //    return std::sqrt(x * x + y * y + z * z);
    return sqrt(x * x + y * y + z * z);
  }

  // ベクトルを正規化するメソッド
  void normalize() {
    float len = length();
    if (len > 0) { // 0で割ることを防ぐ
      x /= len;
      y /= len;
      z /= len;
    }
  }

  // コピー関数
  vvec3& copy(const vvec3& other) {
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
  }

  // プリント関数
  void print() const {
    printf("vvec3(%.2f, %.2f, %.2f)\n", x, y, z);
  }
};

// struct の演算子のオーバーロードに関しては主にここを参考に作成
// https://stackoverflow.com/questions/14047191/overloading-operators-in-typedef-structs-c

struct vvec2
{
  float x,y;

  // 四則演算
  vvec2 operator + (const vvec2 &v) const {
    return vvec2{x+v.x, y+v.y};
  }
  vvec2 operator - (const vvec2 &v) const {
    return vvec2{x-v.x, y-v.y};
  }
  vvec2 operator * (const vvec2 &v) const {
    return vvec2{x*v.x, y*v.y};
  }
  vvec2 operator / (const vvec2 &v) const {
    return vvec2{x/v.x, y/v.y};
  }

  // 代入
  vvec2 &operator = (const vvec2 &v) {
    x=v.x;
    y=v.y;
    return *this;
  } // 何故かセミコロンがないと次からのインデントが狂う
  vvec2 &operator += (const vvec2 &v) {
    x+=v.x;
    y+=v.y;
    return *this;
  }
  vvec2 &operator -= (const vvec2 &v) {
    x-=v.x;
    y-=v.y;
    return *this;
  }
  vvec2 &operator *= (const vvec2 &v) {
    x*=v.x;
    y*=v.y;
    return *this;
  }
  vvec2 &operator /= (const vvec2 &v) {
    x/=v.x;
    y/=v.y;
    return *this;
  }

  // 比較
  bool operator == (const vvec2 &v) const {
    return (x==v.x && y==v.y);
  }
};

//----------------------------------------------------------------------
// Square MATrics: 正方行列
//----------------------------------------------------------------------
struct smat4
{
  float m[4][4];

  // コンストラクタ
  smat4() {
    for(int i = 0; i < 4; ++i)
      for(int j = 0; j < 4; ++j)
        m[i][j] = 0.0f;
  }

  smat4(std::initializer_list<std::initializer_list<float>> l) {
    if (l.size() != 4) {
//        throw std::invalid_argument("Initializer list size does not match matrix dimensions.");
    }
    auto it = l.begin();
    for (int i = 0; i < 4; ++i) {
      if (it->size() != 4) {
//        throw std::invalid_argument("Initializer list size does not match matrix dimensions.");
      }
      auto it2 = it->begin();
      for (int j = 0; j < 4; ++j) {
        m[i][j] = *it2++;
      }
      ++it;
    }
  }

  void clear() {
    for(int i = 0; i < 4; i++)
      for(int j = 0; j < 4; j++)
        m[i][j] = 0.0f;
  }


  // 単位行列を作成する静的メソッド
  static smat4 identity() {
    smat4 id;
    for(int i = 0; i < 4; ++i)
      id.m[i][i] = 1.0f;
    return id;
  }

  void identity_self() {
    clear();
/*
    for(int i = 0; i < 4; i++)
      for(int j = 0; j < 4; j++)
        m[i][j] = 0.0f;
*/
    for(int i = 0; i < 4; ++i)
      m[i][i] = 1.0f;
  }

  smat4 transpose() const {
    smat4 result;
    for(int i = 0; i < 4; i++)
      for(int j = 0; j < 4; j++)
        result.m[j][i] = m[i][j];
    return result;
  }

  void transpose_self() {
    float temp[4][4] = {0};
    for(int i = 0; i < 4; i++)
      for(int j = 0; j < 4; j++)
        temp[j][i] = m[i][j];

    for(int i = 0; i < 4; i++)
      for(int j = 0; j < 4; j++)
        m[i][j] = temp[i][j];
    }

  smat4 operator + (const smat4 &rhs) const {
    smat4 result;
    for(int i = 0; i < 4; ++i)
      for(int j = 0; j < 4; ++j)
        result.m[i][j] = m[i][j] + rhs.m[i][j];
    return result;
  }

  smat4 operator - (const smat4 &rhs) const {
    smat4 result;
    for(int i = 0; i < 4; ++i)
      for(int j = 0; j < 4; ++j)
        result.m[i][j] = m[i][j] - rhs.m[i][j];
    return result;
  }

  smat4 operator * (const smat4 &rhs) const {
    smat4 result;
    for(int i = 0; i < 4; ++i)
      for(int j = 0; j < 4; ++j)
        for(int k = 0; k < 4; ++k)
          result.m[i][j] += m[i][k] * rhs.m[k][j];
    return result;
  }

  vvec4 operator * (const vvec4 &v) const {
    vvec4 result;
    result.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
    result.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
    result.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
    result.w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;
    return result;
  }


  smat4& operator = (const smat4 &rhs) {
    for(int i = 0; i < 4; ++i)
      for(int j = 0; j < 4; ++j)
        m[i][j] = rhs.m[i][j];
    return *this;
  }

    // 加算代入演算子
  smat4 &operator += (const smat4 &mat) {
    for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 4; j++) {
        this->m[i][j] += mat.m[i][j];
      }
    }
    return *this;
  }

  // 減算代入演算子
  smat4 &operator -= (const smat4 &mat) {
    for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 4; j++) {
        this->m[i][j] -= mat.m[i][j];
      }
    }
    return *this;
  }

  // 乗算代入演算子
  smat4 &operator *= (const smat4 &mat) {
    float temp[4][4] = {0};

    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        for (int k = 0; k < 4; ++k) {
          temp[i][j] += m[i][k] * mat.m[k][j];
        }
      }
    }

    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        m[i][j] = temp[i][j];
      }
    }

    return *this;
  }

  // 等価演算子
    bool operator == (const smat4& other) const {
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                if(m[i][j] != other.m[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

  // 非等価演算子
    bool operator != (const smat4& other) const {
        return !(*this == other);
    }

  //----------------------------------------------------------------------
  // scalar
  //----------------------------------------------------------------------
  smat4 operator * (float scalar) const {
    smat4 result;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            result.m[i][j] = this->m[i][j] * scalar;
        }
    }
    return result;
  }

  // 乗算代入演算子も定義
  smat4 &operator *= (float scalar) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            this->m[i][j] *= scalar;
        }
    }
    return *this;
  }

/*
  smat4 operator + (float scalar) const {
    smat4 result;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            result.m[i][j] = this->m[i][j] + scalar;
        }
    }
    return result;
  }

  // 加算代入演算子も定義
  smat4 &operator += (float scalar) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            this->m[i][j] += scalar;
        }
    }
    return *this;
  }

  smat4 operator - (float scalar) const {
    smat4 result;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            result.m[i][j] = this->m[i][j] - scalar;
        }
    }
    return result;
  }

  // 減算代入演算子も定義
  smat4 &operator -= (float scalar) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            this->m[i][j] -= scalar;
        }
    }
    return *this;
  }
*/
};

struct smat3 {
  float m[3][3];

  // Default constructor
  smat3() {
    for(int i = 0; i < 3; i++)
      for(int j = 0; j < 3; j++)
        m[i][j] = 0.0f;
  }

  // Constructor using initializer list
  smat3(std::initializer_list<std::initializer_list<float>> l) {
    if (l.size() != 3) {
//      throw std::invalid_argument("Initializer list size does not match matrix dimensions.");
    }
    int i = 0;
    for(const auto& row : l) {
      if (row.size() != 3) {
//        throw std::invalid_argument("Initializer list size does not match matrix dimensions.");
      }
      int j = 0;
      for(const auto& val : row) {
        m[i][j] = val;
        ++j;
      }
      ++i;
    }
  }

  // Function to generate identity matrix
    static smat3 identity() {
        smat3 I;
        for(int i = 0; i < 3; i++)
            I.m[i][i] = 1.0f;
        return I;
    }

    // Function for matrix transpose
    smat3 transpose() const {
        smat3 result;
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++)
                result.m[j][i] = m[i][j];
        return result;
    }

  void transpose_self() {
    float temp[3][3] = {0};
    for(int i = 0; i < 3; i++)
      for(int j = 0; j < 3; j++)
        temp[j][i] = m[i][j];

    for(int i = 0; i < 3; i++)
      for(int j = 0; j < 3; j++)
        m[i][j] = temp[i][j];
    }

    // Define operator functions based on the new m[3][3] representation...
    smat3 operator + (const smat3 &mat) const {
        smat3 result;
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                result.m[i][j] = this->m[i][j] + mat.m[i][j];
            }
        }
        return result;
    }

    smat3 operator - (const smat3 &mat) const {
        smat3 result;
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                result.m[i][j] = this->m[i][j] - mat.m[i][j];
            }
        }
        return result;
    }

    smat3 operator * (const smat3 &mat) const {
        smat3 result;
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                result.m[i][j] = 0;
                for(int k = 0; k < 3; k++) {
                    result.m[i][j] += this->m[i][k] * mat.m[k][j];
                }
            }
        }
        return result;
    }

    vvec3 operator * (const vvec3 &v) const {
        vvec3 result;
        result.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
        result.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
        result.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;
        return result;
    }
   // 代入演算子
    smat3 &operator = (const smat3 &mat) {
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                this->m[i][j] = mat.m[i][j];
            }
        }
        return *this;
    }

    // 加算代入演算子
    smat3 &operator += (const smat3 &mat) {
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                this->m[i][j] += mat.m[i][j];
            }
        }
        return *this;
    }

    // 減算代入演算子
    smat3 &operator -= (const smat3 &mat) {
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                this->m[i][j] -= mat.m[i][j];
            }
        }
        return *this;
    }

    // 乗算代入演算子
  smat3 &operator *= (const smat3 &mat) {
    float temp[3][3] = {0};

    for(int i = 0; i < 3; i++) {
      for(int j = 0; j < 3; j++) {
        for(int k = 0; k < 3; k++) {
          temp[i][j] += m[i][k] * mat.m[k][j];
        }
      }
    }

    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        m[i][j] = temp[i][j];
      }
    }

    return *this;
  }

  // 等価演算子
    bool operator == (const smat3& other) const {
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                if(m[i][j] != other.m[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

  // 非等価演算子
    bool operator != (const smat3& other) const {
        return !(*this == other);
    }

  //----------------------------------------------------------------------
  // scalar
  //----------------------------------------------------------------------
  smat3 operator * (float scalar) const {
    smat3 result;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            result.m[i][j] = this->m[i][j] * scalar;
        }
    }
    return result;
  }

  // 乗算代入演算子も定義
  smat3 &operator *= (float scalar) {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            this->m[i][j] *= scalar;
        }
    }
    return *this;
  }
/*
  smat3 operator + (float scalar) const {
    smat3 result;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            result.m[i][j] = this->m[i][j] + scalar;
        }
    }
    return result;
  }

  // 加算代入演算子も定義
  smat3 &operator += (float scalar) {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            this->m[i][j] += scalar;
        }
    }
    return *this;
  }

  smat3 operator - (float scalar) const {
    smat3 result;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            result.m[i][j] = this->m[i][j] - scalar;
        }
    }
    return result;
  }

  // 減算代入演算子も定義
  smat3 &operator -= (float scalar) {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            this->m[i][j] -= scalar;
        }
    }
    return *this;
  }
*/

};

// smat2
struct smat2
{
  float m[2][2];

  // Default constructor
  smat2() {
    for(int i = 0; i < 2; i++)
      for(int j = 0; j < 2; j++)
        m[i][j] = 0.0f;
  }

  // Constructor using initializer list
  smat2(std::initializer_list<std::initializer_list<float>> l) {
    if (l.size() != 2) {
//      throw std::invalid_argument("Initializer list size must be 2x2.");
    }        int i = 0;
    for(const auto& row : l) {
      if (row.size() != 2) {
//        throw std::invalid_argument("Initializer list size does not match matrix dimensions.");
      }
      int j = 0;
      for(const auto& val : row) {
        m[i][j] = val;
        ++j;
      }
      ++i;
    }
  }

  // identity matrix
  static smat2 identity() {
    smat2 mat;
    mat.m[0][0] = 1.0f; mat.m[0][1] = 0.0f;
    mat.m[1][0] = 0.0f; mat.m[1][1] = 1.0f;
    return mat;
  }

  // transpose
  smat2 transpose() const {
    smat2 result;
    for(int i = 0; i < 2; i++)
      for(int j = 0; j < 2; j++)
        result.m[j][i] = this->m[i][j];
    return result;
  }

  // transpose self
  smat2 &transpose_self() {
    smat2 temp = *this;
    for(int i = 0; i < 2; i++)
      for(int j = 0; j < 2; j++)
        this->m[j][i] = temp.m[i][j];
    return *this;
  }

  smat2 operator + (const smat2 &mat) const {
    smat2 result;
    for(int i = 0; i < 2; i++)
      for(int j = 0; j < 2; j++)
        result.m[i][j] = this->m[i][j] + mat.m[i][j];
    return result;
  }
  smat2 operator - (const smat2 &mat) const {
    smat2 result;
    for(int i = 0; i < 2; i++)
      for(int j = 0; j < 2; j++)
        result.m[i][j] = this->m[i][j] - mat.m[i][j];
    return result;
  }
  smat2 operator * (const smat2 &mat) const {
    smat2 result;
    for(int i = 0; i < 2; i++)
      for(int j = 0; j < 2; j++)
        for(int k = 0; k < 2; k++)
          result.m[i][j] += this->m[i][k] * mat.m[k][j];
    return result;
  }

  vvec2 operator * (const vvec2 &v) const {
    vvec2 result;
    result.x = m[0][0] * v.x + m[0][1] * v.y;
    result.y = m[1][0] * v.x + m[1][1] * v.y;
    return result;
  }

  // 代入
  smat2 &operator = (const smat2 &mat) {
    for(int i = 0; i < 2; i++)
      for(int j = 0; j < 2; j++)
        this->m[i][j] = mat.m[i][j];
    return *this;
  }
  smat2 &operator += (const smat2 &mat) {
    for(int i = 0; i < 2; i++)
      for(int j = 0; j < 2; j++)
        this->m[i][j] += mat.m[i][j];
    return *this;
  }
  smat2 &operator -= (const smat2 &mat) {
    for(int i = 0; i < 2; i++)
      for(int j = 0; j < 2; j++)
        this->m[i][j] -= mat.m[i][j];
    return *this;
  }
  smat2 &operator *= (const smat2 &mat) {
    smat2 temp;
    for(int i = 0; i < 2; i++)
      for(int j = 0; j < 2; j++) {
        temp.m[i][j] = 0;
        for(int k = 0; k < 2; k++)
          temp.m[i][j] += this->m[i][k] * mat.m[k][j];
      }
    *this = temp;
    return *this;
  }

  // 等価演算子
    bool operator == (const smat2& other) const {
        for(int i = 0; i < 2; i++) {
            for(int j = 0; j < 2; j++) {
                if(m[i][j] != other.m[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

  // 非等価演算子
    bool operator != (const smat2& other) const {
        return !(*this == other);
    }

  //----------------------------------------------------------------------
  // scalar
  //----------------------------------------------------------------------
  smat2 operator * (float scalar) const {
    smat2 result;
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
            result.m[i][j] = this->m[i][j] * scalar;
        }
    }
    return result;
  }

  // 乗算代入演算子も定義
  smat2 &operator *= (float scalar) {
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
            this->m[i][j] *= scalar;
        }
    }
    return *this;
  }
};

void printMatrix(const smat4& matrix);
void printMatrix3(const smat3& matrix);
void printMatrix2(const smat2& matrix);

void printMatrix2d(const float (&m)[4][4]);

//----------------------------------------------------------------------
// Matrix functions (Default Transpose ON)
//----------------------------------------------------------------------
// matget_ series for model
//----------------------------------------------------------------------
void matget_move(const float x, const float y, const float z, smat4& mat, const bool transpose=true);
void matget_zoom(const float x, const float y, const float z, smat4& mat, const bool transpose=true);
void matget_rotate(const float theta, const bool is_axis_x, const bool is_axis_y, const bool is_axis_z, smat4& mat, const bool transpose=true);

void matget_lookat(const vvec3 &cameraPos, const vvec3 &lookAtPos, const vvec3 &up, smat4 &mat, const bool transpose=true);
void matget_ortho(float left, float right,
                  float bottom, float top,
                  float z_near, float z_far, smat4& mat, const bool transpose = true);
void matget_frustum(float left, float right,
                    float bottom, float top,
                    float z_near, float z_far, smat4 &mat, bool transpose=true);
void matget_perspective(float fovY, float aspect, float z_near, float z_far, smat4 &m, bool transpose=true);


void matget_move(const float x, const float y, const float z, float (&m)[4][4], const bool transpose=true);
void matget_zoom(const float x, const float y, const float z, float (&m)[4][4], const bool transpose=true);
void matget_rotate(const float theta, const bool is_axis_x, const bool is_axis_y, const bool is_axis_z, float (&m)[4][4], const bool transpose=true);

void matget_identity(float (&m)[4][4]);

//----------------------------------------------------------------------
// matget_ series for view(camera)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// matget_ series for projection
//----------------------------------------------------------------------

void mat_copy(float src[4][4], float dest[4][4]);
void mat_mul(float a[4][4], float b[4][4], float result[4][4]);

smat4 transpose(smat4 mat);
smat3 transpose3(smat3 mat);
smat2 transpose2(smat2 mat);

float dot_product(vvec3 v1, vvec3 v2);
vvec3 cross_product(vvec3 v1, vvec3 v2);
