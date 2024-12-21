#include <cmath>
#include <Windows.h>
#include "MatrixAlgebra.h"

#define DEG_TO_RAD 0.01745329251994329576923690768489f

void GetIdentityMatrix( float* o_Output ) {
    o_Output[0] = 1.0f;
    o_Output[1] = 0.0f;
    o_Output[2] = 0.0f;
    o_Output[3] = 0.0f;

    o_Output[4] = 0.0f;
    o_Output[5] = 1.0f;
    o_Output[6] = 0.0f;
    o_Output[7] = 0.0f;

    o_Output[8]  = 0.0f;
    o_Output[9]  = 0.0f;
    o_Output[10] = 1.0f;
    o_Output[11] = 0.0f;

    o_Output[12] = 0.0f;
    o_Output[13] = 0.0f;
    o_Output[14] = 0.0f;
    o_Output[15] = 1.0f;
}

void GetPerspectiveProjectionMatrix( const float i_Left,
                                     const float i_Right,
                                     const float i_Bottom,
                                     const float i_Top,
                                     const float i_Near,
                                     const float i_Far,
                                     float*      o_Output ) {
    o_Output[0] = (2.0f * i_Near) / (i_Right - i_Left);
    o_Output[1] = 0.0f;
    o_Output[2] = 0.0f;
    o_Output[3] = 0.0f;

    o_Output[4] = 0.0f;
    o_Output[5] = (2.0f * i_Near) / (i_Top - i_Bottom);
    o_Output[6] = 0.0f;
    o_Output[7] = 0.0f;

    o_Output[8]  = (i_Right + i_Left) / (i_Right - i_Left);
    o_Output[9]  = (i_Top + i_Bottom) / (i_Top - i_Bottom);
    o_Output[10] = -(i_Far + i_Near) / (i_Far - i_Near);
    o_Output[11] = -1.0f;

    o_Output[12] = 0.0f;
    o_Output[13] = 0.0f;
    o_Output[14] = (-2.0f * i_Far * i_Near) / (i_Far - i_Near);
    o_Output[15] = 0.0f;
}

void GetPerspectiveProjectionMatrix( const float i_FOV,
                                     const float i_Near,
                                     const float i_Far,
                                     const float GAspectRatio,
                                     float*      o_Output ) {

    float f = 1.0f / tan( i_FOV * DEG_TO_RAD * 0.5f);

    o_Output[0] = f / GAspectRatio;
    o_Output[1] = 0.0f;
    o_Output[2] = 0.0f;
    o_Output[3] = 0.0f;

    o_Output[4] = 0.0f;
    o_Output[5] = f;
    o_Output[6] = 0.0f;
    o_Output[7] = 0.0f;

    o_Output[8]  = 0.0f;
    o_Output[9]  = 0.0f;
    o_Output[10] = (i_Near + i_Far) / (i_Near - i_Far);
    o_Output[11] = -1.0f;

    o_Output[12] = 0.0f;
    o_Output[13] = 0.0f;
    o_Output[14] = (2.0f * i_Near * i_Far) / (i_Near - i_Far);
    o_Output[15] = 0.0f;
}

void GetTranslationMatrix( const float i_XAxis,
                           const float i_YAxis,
                           const float i_ZAxis,
                           float*      o_Output ) {
    o_Output[0] = 1.0f;
    o_Output[1] = 0.0f;
    o_Output[2] = 0.0f;
    o_Output[3] = 0.0f;

    o_Output[4] = 0.0f;
    o_Output[5] = 1.0f;
    o_Output[6] = 0.0f;
    o_Output[7] = 0.0f;

    o_Output[8]  = 0.0f;
    o_Output[9]  = 0.0f;
    o_Output[10] = 1.0f;
    o_Output[11] = 0.0f;

    o_Output[12] = i_XAxis;
    o_Output[13] = i_YAxis;
    o_Output[14] = i_ZAxis;
    o_Output[15] = 1.0f;
}

void GetXRotationMatrix( const float i_Angle,
                         float*      o_Output ) {
    float cos_angle = cos( i_Angle * DEG_TO_RAD );
    float sin_angle = sin( i_Angle * DEG_TO_RAD );

    o_Output[0] = 1.0f;
    o_Output[1] = 0.0f;
    o_Output[2] = 0.0f;
    o_Output[3] = 0.0f;

    o_Output[4] = 0.0f;
    o_Output[5] = cos_angle;
    o_Output[6] = sin_angle;
    o_Output[7] = 0.0f;

    o_Output[8]  = 0.0f;
    o_Output[9]  = -sin_angle;
    o_Output[10] = cos_angle;
    o_Output[11] = 0.0f;

    o_Output[12] = 0.0f;
    o_Output[13] = 0.0f;
    o_Output[14] = 0.0f;
    o_Output[15] = 1.0f;
}

void GetYRotationMatrix( const float i_Angle,
                         float*      o_Output ) {
    float cos_angle = cos( i_Angle * DEG_TO_RAD );
    float sin_angle = sin( i_Angle * DEG_TO_RAD );

    o_Output[0] = cos_angle;
    o_Output[1] = 0.0f;
    o_Output[2] = -sin_angle;
    o_Output[3] = 0.0f;

    o_Output[4] = 0.0f;
    o_Output[5] = 1.0f;
    o_Output[6] = 0.0f;
    o_Output[7] = 0.0f;

    o_Output[8]  = sin_angle;
    o_Output[9]  = 0.0f;
    o_Output[10] = cos_angle;
    o_Output[11] = 0.0f;

    o_Output[12] = 0.0f;
    o_Output[13] = 0.0f;
    o_Output[14] = 0.0f;
    o_Output[15] = 1.0f;
}

void GetZRotationMatrix( const float i_Angle,
                         float*      o_Output ) {
    float cos_angle = cos( i_Angle * DEG_TO_RAD );
    float sin_angle = sin( i_Angle * DEG_TO_RAD );

    o_Output[0] = cos_angle;
    o_Output[1] = sin_angle;
    o_Output[2] = 0.0f;
    o_Output[3] = 0.0f;

    o_Output[4] = -sin_angle;
    o_Output[5] = cos_angle;
    o_Output[6] = 0.0f;
    o_Output[7] = 0.0f;

    o_Output[8]  = 0.0f;
    o_Output[9]  = 0.0f;
    o_Output[10] = 1.0f;
    o_Output[11] = 0.0f;

    o_Output[12] = 0.0f;
    o_Output[13] = 0.0f;
    o_Output[14] = 0.0f;
    o_Output[15] = 1.0f;
}

void GetScalingMatrix( const float i_XAxis,
                       const float i_YAxis,
                       const float i_ZAxis,
                       float*      o_Output ) {
    o_Output[0] = i_XAxis;
    o_Output[1] = 0.0f;
    o_Output[2] = 0.0f;
    o_Output[3] = 0.0f;

    o_Output[4] = 0.0f;
    o_Output[5] = i_YAxis;
    o_Output[6] = 0.0f;
    o_Output[7] = 0.0f;

    o_Output[8]  = 0.0f;
    o_Output[9]  = 0.0f;
    o_Output[10] = i_ZAxis;
    o_Output[11] = 0.0f;

    o_Output[12] = 0.0f;
    o_Output[13] = 0.0f;
    o_Output[14] = 0.0f;
    o_Output[15] = 1.0f;
}

void Multiply( const float* i_Matrix1,
               const float* i_Matrix2,
               float*       o_Output ) {
    float result[16];

    result[0] = i_Matrix1[0] * i_Matrix2[0] + i_Matrix1[4] * i_Matrix2[1] + i_Matrix1[8] * i_Matrix2[2] + i_Matrix1[12] * i_Matrix2[3];
    result[1] = i_Matrix1[1] * i_Matrix2[0] + i_Matrix1[5] * i_Matrix2[1] + i_Matrix1[9] * i_Matrix2[2] + i_Matrix1[13] * i_Matrix2[3];
    result[2] = i_Matrix1[2] * i_Matrix2[0] + i_Matrix1[6] * i_Matrix2[1] + i_Matrix1[10] * i_Matrix2[2] + i_Matrix1[14] * i_Matrix2[3];
    result[3] = i_Matrix1[3] * i_Matrix2[0] + i_Matrix1[7] * i_Matrix2[1] + i_Matrix1[11] * i_Matrix2[2] + i_Matrix1[15] * i_Matrix2[3];

    result[4] = i_Matrix1[0] * i_Matrix2[4] + i_Matrix1[4] * i_Matrix2[5] + i_Matrix1[8] * i_Matrix2[6] + i_Matrix1[12] * i_Matrix2[7];
    result[5] = i_Matrix1[1] * i_Matrix2[4] + i_Matrix1[5] * i_Matrix2[5] + i_Matrix1[9] * i_Matrix2[6] + i_Matrix1[13] * i_Matrix2[7];
    result[6] = i_Matrix1[2] * i_Matrix2[4] + i_Matrix1[6] * i_Matrix2[5] + i_Matrix1[10] * i_Matrix2[6] + i_Matrix1[14] * i_Matrix2[7];
    result[7] = i_Matrix1[3] * i_Matrix2[4] + i_Matrix1[7] * i_Matrix2[5] + i_Matrix1[11] * i_Matrix2[6] + i_Matrix1[15] * i_Matrix2[7];

    result[8] = i_Matrix1[0] * i_Matrix2[8] + i_Matrix1[4] * i_Matrix2[9] + i_Matrix1[8] * i_Matrix2[10] + i_Matrix1[12] * i_Matrix2[11];
    result[9] = i_Matrix1[1] * i_Matrix2[8] + i_Matrix1[5] * i_Matrix2[9] + i_Matrix1[9] * i_Matrix2[10] + i_Matrix1[13] * i_Matrix2[11];
    result[10] = i_Matrix1[2] * i_Matrix2[8] + i_Matrix1[6] * i_Matrix2[9] + i_Matrix1[10] * i_Matrix2[10] + i_Matrix1[14] * i_Matrix2[11];
    result[11] = i_Matrix1[3] * i_Matrix2[8] + i_Matrix1[7] * i_Matrix2[9] + i_Matrix1[11] * i_Matrix2[10] + i_Matrix1[15] * i_Matrix2[11];

    result[12] = i_Matrix1[0] * i_Matrix2[12] + i_Matrix1[4] * i_Matrix2[13] + i_Matrix1[8] * i_Matrix2[14] + i_Matrix1[12] * i_Matrix2[15];
    result[13] = i_Matrix1[1] * i_Matrix2[12] + i_Matrix1[5] * i_Matrix2[13] + i_Matrix1[9] * i_Matrix2[14] + i_Matrix1[13] * i_Matrix2[15];
    result[14] = i_Matrix1[2] * i_Matrix2[12] + i_Matrix1[6] * i_Matrix2[13] + i_Matrix1[10] * i_Matrix2[14] + i_Matrix1[14] * i_Matrix2[15];
    result[15] = i_Matrix1[3] * i_Matrix2[12] + i_Matrix1[7] * i_Matrix2[13] + i_Matrix1[11] * i_Matrix2[14] + i_Matrix1[15] * i_Matrix2[15];

    memcpy_s( o_Output, 16*sizeof(float), result, 16*sizeof(float) );
}

void Translate( const float i_XAxis,
                const float i_YAxis,
                const float i_ZAxis,
                float*      io_Matrix ) {
    float matrix[16];

    matrix[0] = 1.0f;
    matrix[1] = 0.0f;
    matrix[2] = 0.0f;
    matrix[3] = 0.0f;

    matrix[4] = 0.0f;
    matrix[5] = 1.0f;
    matrix[6] = 0.0f;
    matrix[7] = 0.0f;

    matrix[8]  = 0.0f;
    matrix[9]  = 0.0f;
    matrix[10] = 1.0f;
    matrix[11] = 0.0f;

    matrix[12] = i_XAxis;
    matrix[13] = i_YAxis;
    matrix[14] = i_ZAxis;
    matrix[15] = 1.0f;

    Multiply( matrix, io_Matrix, io_Matrix );
}

void XRotate( const float i_Angle,
              float*      io_Matrix ) {
    float matrix[16];

    float cos_angle = cos( i_Angle * DEG_TO_RAD );
    float sin_angle = sin( i_Angle * DEG_TO_RAD );

    matrix[0] = 1.0f;
    matrix[1] = 0.0f;
    matrix[2] = 0.0f;
    matrix[3] = 0.0f;

    matrix[4] = 0.0f;
    matrix[5] = cos_angle;
    matrix[6] = sin_angle;
    matrix[7] = 0.0f;

    matrix[8]  = 0.0f;
    matrix[9]  = -sin_angle;
    matrix[10] = cos_angle;
    matrix[11] = 0.0f;

    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;

    Multiply( matrix, io_Matrix, io_Matrix );
}

void YRotate( const float i_Angle,
              float*      io_Matrix ) {
    float matrix[16];

    float cos_angle = cos( i_Angle * DEG_TO_RAD );
    float sin_angle = sin( i_Angle * DEG_TO_RAD );

    matrix[0] = cos_angle;
    matrix[1] = 0.0f;
    matrix[2] = -sin_angle;
    matrix[3] = 0.0f;

    matrix[4] = 0.0f;
    matrix[5] = 1.0f;
    matrix[6] = 0.0f;
    matrix[7] = 0.0f;

    matrix[8]  = sin_angle;
    matrix[9]  = 0.0f;
    matrix[10] = cos_angle;
    matrix[11] = 0.0f;

    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;

    Multiply( matrix, io_Matrix, io_Matrix );
}

void ZRotate( const float i_Angle,
              float*      io_Matrix ) {
    float matrix[16];

    float cos_angle = cos( i_Angle * DEG_TO_RAD );
    float sin_angle = sin( i_Angle * DEG_TO_RAD );

    matrix[0] = cos_angle;
    matrix[1] = sin_angle;
    matrix[2] = 0.0f;
    matrix[3] = 0.0f;

    matrix[4] = -sin_angle;
    matrix[5] = cos_angle;
    matrix[6] = 0.0f;
    matrix[7] = 0.0f;

    matrix[8]  = 0.0f;
    matrix[9]  = 0.0f;
    matrix[10] = 1.0f;
    matrix[11] = 0.0f;

    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;

    Multiply( matrix, io_Matrix, io_Matrix );
}

void Scale( const float i_XAxis,
            const float i_YAxis,
            const float i_ZAxis,
            float*      io_Matrix ) {
    float matrix[16];

    matrix[0] = i_XAxis;
    matrix[1] = 0.0f;
    matrix[2] = 0.0f;
    matrix[3] = 0.0f;

    matrix[4] = 0.0f;
    matrix[5] = i_YAxis;
    matrix[6] = 0.0f;
    matrix[7] = 0.0f;

    matrix[8]  = 0.0f;
    matrix[9]  = 0.0f;
    matrix[10] = i_ZAxis;
    matrix[11] = 0.0f;

    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;

    Multiply( matrix, io_Matrix, io_Matrix );
}
