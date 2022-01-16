#ifndef __GAME_OF_LIFE_MATRIX_H__
#define __GAME_OF_LIFE_MATRIX_H__

void orthographicProjection(float left, float right, float bottom, float top, float near, float far, float out_matrix[16]);
float dot(const float a[3], const float b[3]);
void cross(const float a[3], const float b[3], float out[3]);
void normalise(float a[3]);
float norm(const float a[3]);
float mult(const float a[16], const float b[16], float out_matrix[16]);
void zeroMatrix(float a[16]);
void identityMatrix(float a[16]);
void lookDir(const float eye[3], const float target[3], const float up[3], float out_matrix[16]);
void transpose(float a[16]);


#endif // __GAME_OF_LIFE_MATRIX_H__
