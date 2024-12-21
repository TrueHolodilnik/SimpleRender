#ifndef _MATRIX_ALGEBRA_HEADER_
#define _MATRIX_ALGEBRA_HEADER_

void GetIdentityMatrix( float* o_Output );

void GetPerspectiveProjectionMatrix( const float i_Left,
                                     const float i_Right,
                                     const float i_Bottom,
                                     const float i_Top,
                                     const float i_Near,
                                     const float i_Far,
                                     float*      o_Output );

void GetPerspectiveProjectionMatrix( const float i_FOV,
                                     const float i_Near,
                                     const float i_Far,
                                     const float AspectRatio,
                                     float*      o_Output );

void GetTranslationMatrix( const float i_XAxis,
                           const float i_YAxis,
                           const float i_ZAxis,
                           float*      o_Output );

void GetXRotationMatrix( const float i_Angle,
                         float*      o_Output );

void GetYRotationMatrix( const float i_Angle,
                         float*      o_Output );

void GetZRotationMatrix( const float i_Angle,
                         float*      o_Output );

void GetScalingMatrix( const float i_XAxis,
                       const float i_YAxis,
                       const float i_ZAxis,
                       float*      o_Output );

void Multiply( const float* i_Matrix1,
               const float* i_Matrix2,
               float*       o_Output );

void Translate( const float i_XAxis,
                const float i_YAxis,
                const float i_ZAxis,
                float*      io_Matrix );

void XRotate( const float i_Angle,
              float*      io_Matrix );

void YRotate( const float i_Angle,
              float*      io_Matrix );

void ZRotate( const float i_Angle,
              float*      io_Matrix );

void Scale( const float i_XAxis,
            const float i_YAxis,
            const float i_ZAxis,
            float*      io_Matrix );

#endif // _MATRIX_ALGEBRA_HEADER_