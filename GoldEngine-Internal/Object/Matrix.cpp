#include "../SDK.h"
#include "Matrix.h"

using namespace Engine::Components;
using namespace Engine::Components::Abstract;

generic <class M>
M Engine::Components::Matrix<M>::New()
{
	return M();
}

generic <class M>
inline M Engine::Components::Matrix<M>::Identity()
{
	RAYLIB::Matrix matrix = RAYMATH::MatrixIdentity();
	return M();
}

generic <class M>
M Engine::Components::Matrix<M>::Add(M left, M right)
{
    if (left->size != right->size)
        throw gcnew System::ArithmeticException("Attempted to add two matrices of different size");

    array<float>^ leftVals = left->GetValues();
    array<float>^ rightVals = right->GetValues();
    array<float>^ resultVals = gcnew array<float>(leftVals->Length);

    for (int i = 0; i < leftVals->Length; i++)
    {
        resultVals[i] = leftVals[i] + rightVals[i];
    }

    M result = gcnew M();
    result->SetValues(resultVals);
    return result;
}

generic <class M>
M Engine::Components::Matrix<M>::Subtract(M left, M right)
{
    if (left->size != right->size)
        throw gcnew System::ArithmeticException("Attempted to subtract two matrices of different size");

    array<float>^ leftVals = left->GetValues();
    array<float>^ rightVals = right->GetValues();
    array<float>^ resultVals = gcnew array<float>(leftVals->Length);

    for (int i = 0; i < leftVals->Length; i++)
    {
        resultVals[i] = leftVals[i] - rightVals[i];
    }

    M result = gcnew M();
    result->SetValues(resultVals);
    return result;
}

generic <class M>
M Engine::Components::Matrix<M>::Multiply(M left, M right)
{
    if (left->Columns != right->Rows)
        throw gcnew System::ArithmeticException("Invalid matrix dimensions for multiplication.");

    if (left->Rows != right->Rows || left->Columns != right->Columns)
        throw gcnew System::ArithmeticException("Only square matrices of the same dimensions are supported.");

    int rows = left->Rows;
    int cols = right->Columns;
    int inner = left->Columns;

    array<float>^ result = gcnew array<float>(rows * cols);

    array<float>^ a = left->GetValues();
    array<float>^ b = right->GetValues();

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            float sum = 0;
            for (int k = 0; k < inner; k++)
            {
                sum += a[i * inner + k] * b[k * cols + j];
            }
            result[i * cols + j] = sum;
        }
    }

    M res = M();
    res->SetValues(result);
    return res;
}

generic<class M>
M Engine::Components::Matrix<M>::Translate(Engine::Components::Vector3 position)
{
    RAYMATH::Matrix matrix = RAYMATH::MatrixTranslate(position.x, position.y, position.z);
    M _matrix = M();
    
    array<float>^ values = { 
        matrix.m0, matrix.m1, matrix.m2, matrix.m3,
        matrix.m4, matrix.m5, matrix.m6, matrix.m7,
        matrix.m8, matrix.m9, matrix.m10, matrix.m11,
        matrix.m12, matrix.m13,matrix.m14, matrix.m15
    };

    _matrix->SetValues(values);

    return _matrix;
}

generic <class M>
M Engine::Components::Matrix<M>::Scale(Engine::Components::Vector3 scale)
{
    RAYMATH::Matrix matrix = RAYMATH::MatrixScale(scale.x, scale.y, scale.z);
    M _matrix = M();

    array<float>^ values = {
        matrix.m0, matrix.m1, matrix.m2, matrix.m3,
        matrix.m4, matrix.m5, matrix.m6, matrix.m7,
        matrix.m8, matrix.m9, matrix.m10, matrix.m11,
        matrix.m12, matrix.m13,matrix.m14, matrix.m15
    };

    _matrix->SetValues(values);
    return _matrix;
}

generic <class M>
M Engine::Components::Matrix<M>::RotateXYZ(Engine::Components::Vector3 rotation)
{
    RAYMATH::Matrix matrix = RAYMATH::MatrixRotateXYZ(rotation.toNative());
    M _matrix = M();

    array<float>^ values = {
        matrix.m0, matrix.m1, matrix.m2, matrix.m3,
        matrix.m4, matrix.m5, matrix.m6, matrix.m7,
        matrix.m8, matrix.m9, matrix.m10, matrix.m11,
        matrix.m12, matrix.m13,matrix.m14, matrix.m15
    };

    _matrix->SetValues(values);
    return _matrix;
}

Engine::Components::Matrix16::Matrix16(RAYLIB::Matrix matrix) :
    Engine::Components::Matrix16()
{
    this->m0 = matrix.m0;
    this->m1 = matrix.m1;
    this->m2 = matrix.m2;
    this->m3 = matrix.m3;
    this->m4 = matrix.m4;
    this->m5 = matrix.m5;
    this->m6 = matrix.m6;
    this->m7 = matrix.m7;
    this->m8 = matrix.m8;
    this->m9 = matrix.m9;
    this->m10 = matrix.m10;
    this->m11 = matrix.m11;
    this->m12 = matrix.m12;
    this->m13 = matrix.m13;
    this->m14 = matrix.m14;
    this->m15 = matrix.m15;
}

Engine::Components::Matrix16::Matrix16(System::Numerics::Matrix4x4 matrix) :
    Engine::Components::Matrix16()
{
    this->m0 = matrix.M11;
    this->m1 = matrix.M12;
    this->m2 = matrix.M13;
    this->m3 = matrix.M14;
    this->m4 = matrix.M21;
    this->m5 = matrix.M22;
    this->m6 = matrix.M23;
    this->m7 = matrix.M24;
    this->m8 = matrix.M31;
    this->m9 = matrix.M32;
    this->m10 = matrix.M33;
    this->m11 = matrix.M34;
    this->m12 = matrix.M41;
    this->m13 = matrix.M42;
    this->m14 = matrix.M43;
    this->m15 = matrix.M44;
}

array<float>^ Engine::Components::Matrix16::GetValues()
{
    return gcnew array<float>{ m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15 };
}

void Engine::Components::Matrix16::SetValues(array<float>^ values)
{
    if (values->Length != 16) throw gcnew System::ArgumentException("Invalid matrix size for input");
    m0 = values[0];     m1 = values[1];     m2 = values[2];     m3 = values[3];
    m4 = values[4];     m5 = values[5];     m6 = values[6];     m7 = values[7];
    m8 = values[8];     m9 = values[9];     m10 = values[10];    m11 = values[11];
    m12 = values[12];    m13 = values[13];   m14 = values[14];  m15 = values[15];
}

void Engine::Components::Matrix16::Decompose(Engine::Components::Vector3% position, Engine::Components::Quaternion% rotation, Engine::Components::Vector3% scale)
{
    // Extract translation
    position = Engine::Components::Vector3(m12, m13, m14);

    // Extract scale (length of column vectors)
    scale.x = Math::Sqrt(m0 * m0 + m1 * m1 + m2 * m2);
    scale.y = Math::Sqrt(m4 * m4 + m5 * m5 + m6 * m6);
    scale.z = Math::Sqrt(m8 * m8 + m9 * m9 + m10 * m10);

    float r00 = m0 / scale.x, r01 = m1 / scale.x, r02 = m2 / scale.x;
    float r10 = m4 / scale.y, r11 = m5 / scale.y, r12 = m6 / scale.y;
    float r20 = m8 / scale.z, r21 = m9 / scale.z, r22 = m10 / scale.z;

    float trace = r00 + r11 + r22;
    if (trace > 0.0f)
    {
        float s = Math::Sqrt(trace + 1.0f) * 2;
        rotation.w = 0.25f * s;
        rotation.x = (r21 - r12) / s;
        rotation.y = (r02 - r20) / s;
        rotation.z = (r10 - r01) / s;
    }
    else if ((r00 > r11) && (r00 > r22))
    {
        float s = Math::Sqrt(1.0f + r00 - r11 - r22) * 2;
        rotation.w = (r21 - r12) / s;
        rotation.x = 0.25f * s;
        rotation.y = (r01 + r10) / s;
        rotation.z = (r02 + r20) / s;
    }
    else if (r11 > r22)
    {
        float s = Math::Sqrt(1.0f + r11 - r00 - r22) * 2;
        rotation.w = (r02 - r20) / s;
        rotation.x = (r01 + r10) / s;
        rotation.y = 0.25f * s;
        rotation.z = (r12 + r21) / s;
    }
    else
    {
        float s = Math::Sqrt(1.0f + r22 - r00 - r11) * 2;
        rotation.w = (r10 - r01) / s;
        rotation.x = (r02 + r20) / s;
        rotation.y = (r12 + r21) / s;
        rotation.z = 0.25f * s;
    }
}

Matrix16^ Engine::Components::Matrix16::MatrixMultiply(Matrix16^ left, Matrix16^ right)
{
    Matrix16^ result = gcnew Matrix16();

    result->m0 = left->m0 * right->m0 + left->m1 * right->m4 + left->m2 * right->m8 + left->m3 * right->m12;
    result->m1 = left->m0 * right->m1 + left->m1 * right->m5 + left->m2 * right->m9 + left->m3 * right->m13;
    result->m2 = left->m0 * right->m2 + left->m1 * right->m6 + left->m2 * right->m10 + left->m3 * right->m14;
    result->m3 = left->m0 * right->m3 + left->m1 * right->m7 + left->m2 * right->m11 + left->m3 * right->m15;
    result->m4 = left->m4 * right->m0 + left->m5 * right->m4 + left->m6 * right->m8 + left->m7 * right->m12;
    result->m5 = left->m4 * right->m1 + left->m5 * right->m5 + left->m6 * right->m9 + left->m7 * right->m13;
    result->m6 = left->m4 * right->m2 + left->m5 * right->m6 + left->m6 * right->m10 + left->m7 * right->m14;
    result->m7 = left->m4 * right->m3 + left->m5 * right->m7 + left->m6 * right->m11 + left->m7 * right->m15;
    result->m8 = left->m8 * right->m0 + left->m9 * right->m4 + left->m10 * right->m8 + left->m11 * right->m12;
    result->m9 = left->m8 * right->m1 + left->m9 * right->m5 + left->m10 * right->m9 + left->m11 * right->m13;
    result->m10 = left->m8 * right->m2 + left->m9 * right->m6 + left->m10 * right->m10 + left->m11 * right->m14;
    result->m11 = left->m8 * right->m3 + left->m9 * right->m7 + left->m10 * right->m11 + left->m11 * right->m15;
    result->m12 = left->m12 * right->m0 + left->m13 * right->m4 + left->m14 * right->m8 + left->m15 * right->m12;
    result->m13 = left->m12 * right->m1 + left->m13 * right->m5 + left->m14 * right->m9 + left->m15 * right->m13;
    result->m14 = left->m12 * right->m2 + left->m13 * right->m6 + left->m14 * right->m10 + left->m15 * right->m14;
    result->m15 = left->m12 * right->m3 + left->m13 * right->m7 + left->m14 * right->m11 + left->m15 * right->m15;

    return result;
}

Matrix16^ Engine::Components::Matrix16::FromTRS(Engine::Components::Vector3 position, Engine::Components::Quaternion rotation, Engine::Components::Vector3 scale)
{
    Matrix16^ S = gcnew Matrix16();
    S->m0 = scale.x; S->m1 = 0;       S->m2 = 0;       S->m3 = 0;
    S->m4 = 0;       S->m5 = scale.y; S->m6 = 0;       S->m7 = 0;
    S->m8 = 0;       S->m9 = 0;       S->m10 = scale.z; S->m11 = 0;
    S->m12 = 0;      S->m13 = 0;      S->m14 = 0;      S->m15 = 1;


    float x = rotation.x, y = rotation.y, z = rotation.z, w = rotation.w;
    float xx = x * x, yy = y * y, zz = z * z;
    float xy = x * y, xz = x * z, yz = y * z;
    float wx = w * x, wy = w * y, wz = w * z;

    Matrix16^ R = gcnew Matrix16();
    R->m0 = 1 - 2 * (yy + zz); R->m1 = 2 * (xy - wz);     R->m2 = 2 * (xz + wy);     R->m3 = 0;
    R->m4 = 2 * (xy + wz);     R->m5 = 1 - 2 * (xx + zz); R->m6 = 2 * (yz - wx);     R->m7 = 0;
    R->m8 = 2 * (xz - wy);     R->m9 = 2 * (yz + wx);     R->m10 = 1 - 2 * (xx + yy); R->m11 = 0;
    R->m12 = 0;                  R->m13 = 0;                 R->m14 = 0;                 R->m15 = 1;

    Matrix16^ T = gcnew Matrix16();
    T->m0 = 1; T->m1 = 0; T->m2 = 0; T->m3 = 0;
    T->m4 = 0; T->m5 = 1; T->m6 = 0; T->m7 = 0;
    T->m8 = 0; T->m9 = 0; T->m10 = 1; T->m11 = 0;
    T->m12 = position.x; T->m13 = position.y; T->m14 = position.z; T->m15 = 1;

    Matrix16^ TR = Matrix16::MatrixMultiply(T, R);
    return Matrix16::MatrixMultiply(TR, S);
}

Matrix16^ Engine::Components::Matrix16::Invert(Matrix16^ m)
{
    Matrix16^ inv = gcnew Matrix16();

    float mat[16] = { m->m0, m->m1, m->m2, m->m3,
                      m->m4, m->m5, m->m6, m->m7,
                      m->m8, m->m9, m->m10, m->m11,
                      m->m12, m->m13, m->m14, m->m15 };

    float invOut[16];

    float det;
    int i;

    invOut[0] = mat[5] * mat[10] * mat[15] -
        mat[5] * mat[11] * mat[14] -
        mat[9] * mat[6] * mat[15] +
        mat[9] * mat[7] * mat[14] +
        mat[13] * mat[6] * mat[11] -
        mat[13] * mat[7] * mat[10];

    invOut[4] = -mat[4] * mat[10] * mat[15] +
        mat[4] * mat[11] * mat[14] +
        mat[8] * mat[6] * mat[15] -
        mat[8] * mat[7] * mat[14] -
        mat[12] * mat[6] * mat[11] +
        mat[12] * mat[7] * mat[10];

    invOut[8] = mat[4] * mat[9] * mat[15] -
        mat[4] * mat[11] * mat[13] -
        mat[8] * mat[5] * mat[15] +
        mat[8] * mat[7] * mat[13] +
        mat[12] * mat[5] * mat[11] -
        mat[12] * mat[7] * mat[9];

    invOut[12] = -mat[4] * mat[9] * mat[14] +
        mat[4] * mat[10] * mat[13] +
        mat[8] * mat[5] * mat[14] -
        mat[8] * mat[6] * mat[13] -
        mat[12] * mat[5] * mat[10] +
        mat[12] * mat[6] * mat[9];

    invOut[1] = -mat[1] * mat[10] * mat[15] +
        mat[1] * mat[11] * mat[14] +
        mat[9] * mat[2] * mat[15] -
        mat[9] * mat[3] * mat[14] -
        mat[13] * mat[2] * mat[11] +
        mat[13] * mat[3] * mat[10];

    invOut[5] = mat[0] * mat[10] * mat[15] -
        mat[0] * mat[11] * mat[14] -
        mat[8] * mat[2] * mat[15] +
        mat[8] * mat[3] * mat[14] +
        mat[12] * mat[2] * mat[11] -
        mat[12] * mat[3] * mat[10];

    invOut[9] = -mat[0] * mat[9] * mat[15] +
        mat[0] * mat[11] * mat[13] +
        mat[8] * mat[1] * mat[15] -
        mat[8] * mat[3] * mat[13] -
        mat[12] * mat[1] * mat[11] +
        mat[12] * mat[3] * mat[9];

    invOut[13] = mat[0] * mat[9] * mat[14] -
        mat[0] * mat[10] * mat[13] -
        mat[8] * mat[1] * mat[14] +
        mat[8] * mat[2] * mat[13] +
        mat[12] * mat[1] * mat[10] -
        mat[12] * mat[2] * mat[9];

    invOut[2] = mat[1] * mat[6] * mat[15] -
        mat[1] * mat[7] * mat[14] -
        mat[5] * mat[2] * mat[15] +
        mat[5] * mat[3] * mat[14] +
        mat[13] * mat[2] * mat[7] -
        mat[13] * mat[3] * mat[6];

    invOut[6] = -mat[0] * mat[6] * mat[15] +
        mat[0] * mat[7] * mat[14] +
        mat[4] * mat[2] * mat[15] -
        mat[4] * mat[3] * mat[14] -
        mat[12] * mat[2] * mat[7] +
        mat[12] * mat[3] * mat[6];

    invOut[10] = mat[0] * mat[5] * mat[15] -
        mat[0] * mat[7] * mat[13] -
        mat[4] * mat[1] * mat[15] +
        mat[4] * mat[3] * mat[13] +
        mat[12] * mat[1] * mat[7] -
        mat[12] * mat[3] * mat[5];

    invOut[14] = -mat[0] * mat[5] * mat[14] +
        mat[0] * mat[6] * mat[13] +
        mat[4] * mat[1] * mat[14] -
        mat[4] * mat[2] * mat[13] -
        mat[12] * mat[1] * mat[6] +
        mat[12] * mat[2] * mat[5];

    invOut[3] = -mat[1] * mat[6] * mat[11] +
        mat[1] * mat[7] * mat[10] +
        mat[5] * mat[2] * mat[11] -
        mat[5] * mat[3] * mat[10] -
        mat[9] * mat[2] * mat[7] +
        mat[9] * mat[3] * mat[6];

    invOut[7] = mat[0] * mat[6] * mat[11] -
        mat[0] * mat[7] * mat[10] -
        mat[4] * mat[2] * mat[11] +
        mat[4] * mat[3] * mat[10] +
        mat[8] * mat[2] * mat[7] -
        mat[8] * mat[3] * mat[6];

    invOut[11] = -mat[0] * mat[5] * mat[11] +
        mat[0] * mat[7] * mat[9] +
        mat[4] * mat[1] * mat[11] -
        mat[4] * mat[3] * mat[9] -
        mat[8] * mat[1] * mat[7] +
        mat[8] * mat[3] * mat[5];

    invOut[15] = mat[0] * mat[5] * mat[10] -
        mat[0] * mat[6] * mat[9] -
        mat[4] * mat[1] * mat[10] +
        mat[4] * mat[2] * mat[9] +
        mat[8] * mat[1] * mat[6] -
        mat[8] * mat[2] * mat[5];

    det = mat[0] * invOut[0] + mat[1] * invOut[4] + mat[2] * invOut[8] + mat[3] * invOut[12];

    if (det == 0)
        throw gcnew System::Exception("Matrix is non-invertible");

    det = 1.0f / det;

    for (i = 0; i < 16; i++)
        invOut[i] *= det;

    inv->m0 = invOut[0];   inv->m1 = invOut[1];   inv->m2 = invOut[2];   inv->m3 = invOut[3];
    inv->m4 = invOut[4];   inv->m5 = invOut[5];   inv->m6 = invOut[6];   inv->m7 = invOut[7];
    inv->m8 = invOut[8];   inv->m9 = invOut[9];   inv->m10 = invOut[10]; inv->m11 = invOut[11];
    inv->m12 = invOut[12]; inv->m13 = invOut[13]; inv->m14 = invOut[14]; inv->m15 = invOut[15];

    return inv;
}

RAYLIB::Matrix Engine::Components::Matrix16::toNative()
{
    return {
        m0, m1, m2, m3,
        m4, m5, m6, m7,
        m8, m9, m10, m11,
        m12, m13, m14, m15
    };
}

Engine::Components::Matrix6::Matrix6(System::Numerics::Matrix3x2 matrix) :
    Engine::Components::Matrix6()
{
    this->m0 = matrix.M11;
    this->m1 = matrix.M12;
    this->m2 = matrix.M21;
    this->m3 = matrix.M22;
    this->m4 = matrix.M31;
    this->m5 = matrix.M32;
}

array<float>^ Engine::Components::Matrix6::GetValues()
{
    return gcnew array<float>{ m0, m1, m2, m3, m4, m5 };
}

void Engine::Components::Matrix6::SetValues(array<float>^ values)
{
    if (values->Length != 6) throw gcnew System::ArgumentException("Invalid matrix size for input");
    m0 = values[0]; m1 = values[1]; m2 = values[2]; 
    m3 = values[3]; m4 = values[4]; m5 = values[5];
}

array<float>^ Engine::Components::Matrix4::GetValues()
{
    return gcnew array<float>{ m0, m1, m2, m3 };
}

void Engine::Components::Matrix4::SetValues(array<float>^ values)
{
    if (values->Length != 4) throw gcnew System::ArgumentException("Invalid matrix size for input");
    m0 = values[0]; m1 = values[1];
    m2 = values[2]; m3 = values[3];
}

Engine::Components::Matrix16::Matrix16() :
    Engine::Components::Abstract::Mat(16)
{
    this->Rows = 4;
    this->Columns = 4;
}

Engine::Components::Matrix6::Matrix6() :
    Engine::Components::Abstract::Mat(6)
{
    this->Rows = 3;
    this->Columns = 2;
}

Engine::Components::Matrix4::Matrix4() :
    Engine::Components::Abstract::Mat(4)
{
    this->Rows = 2;
    this->Columns = 2;
}