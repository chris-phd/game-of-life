#include "matrix.h"

#include <math.h>
#include <stdio.h>

void translation(float x, float y, float z, float out_matrix[16]) {
    identityMatrix(out_matrix);
    out_matrix[3] = x;
    out_matrix[7] = y;
    out_matrix[11] = z;
}

void scale(float scale, float out_matrix[16]) {
    identityMatrix(out_matrix);
    out_matrix[0] = out_matrix[5] = out_matrix[10] = scale;
}

// Sort of works, but this seems to be producing a weird zoom level...
void orthographicProjection(float left, float right, float bottom, float top, float near, float far, float out_matrix[16]) {
    identityMatrix(out_matrix);

    out_matrix[0] = 2.0f / (right - left);
    out_matrix[5] = 2.0f / (top - bottom);
    out_matrix[10] = -2.0f / (far - near);
    out_matrix[12] = -(right + left) / (right - left);
    out_matrix[13] = - (top + bottom) / (top - bottom);
    out_matrix[14] = - (far + near) / (far - near);

    transpose(out_matrix); // needed? 
}

/// Used to set the view matrix when you know the looking point.
void lookDir(const float eye[3], const float target[3], const float up[3], float out_matrix[16]) {

    float xaxis[3];
    float yaxis[3];
    float zaxis[3] = {eye[0] - target[0], eye[1] - target[1], eye[2] - target[2]};
    normalise(zaxis);

    cross(up, zaxis, xaxis);
    normalise(xaxis);

    cross(zaxis, xaxis, yaxis);
    normalise(yaxis);


    float translation[16];
    identityMatrix(translation);
    translation[3] = -eye[0];
    translation[7] = -eye[1];
    translation[11] = -eye[2];

    float rotation[16];
    identityMatrix(rotation);
    rotation[0] = xaxis[0];
    rotation[1] = xaxis[1];
    rotation[2] = xaxis[2];
    rotation[4] = yaxis[0];
    rotation[5] = yaxis[1];
    rotation[6] = yaxis[2];
    rotation[8] = zaxis[0];
    rotation[9] = zaxis[1];
    rotation[10] = zaxis[2];
    
    mult(rotation, translation, out_matrix);
    transpose(out_matrix);
}

float dot(const float a[3], const float b[3]) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

void cross(const float a[3], const float b[3], float out[3]) {
    out[0] = a[1]*b[2] - a[2]*b[1];
    out[1] = a[2]*b[0] - a[0]*b[2];
    out[2] = a[0]*b[1] - a[1]*b[0];
}

void normalise(float a[3]) {
    float inv_mag = 1/norm(a);
    a[0] = a[0] * inv_mag;
    a[1] = a[1] * inv_mag;
    a[2] = a[2] * inv_mag;
}

/// Returns the l2 norm of a vec3
float norm(const float a[3]) {
    return sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
}

void mult(const float a[16], const float b[16], float out_matrix[16]) {
    // zero all elements
    out_matrix[0] = out_matrix[1] = out_matrix[2] = out_matrix[3] = 
    out_matrix[4] = out_matrix[5] = out_matrix[6] = out_matrix[7] = 
    out_matrix[8] = out_matrix[9] = out_matrix[10] = out_matrix[11] = 
    out_matrix[12] = out_matrix[13] = out_matrix[14] = out_matrix[15] = 0.0f;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                out_matrix[j + i*4] += a[k + i*4] * b[j + k*4];
            }
        }
    }
}

void multCol(const float a[16], const float b[4], float out[4]) {

    out[0] = out[1] = out[2] = out[3] = 0.0f;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                out[i] += a[k + i*4] * b[k];
            }
        }
    }
}

void zeroMatrix(float out_matrix[16]) {
    // zero all elements
    out_matrix[0] = out_matrix[1] = out_matrix[2] = out_matrix[3] = 
    out_matrix[4] = out_matrix[5] = out_matrix[6] = out_matrix[7] = 
    out_matrix[8] = out_matrix[9] = out_matrix[10] = out_matrix[11] = 
    out_matrix[12] = out_matrix[13] = out_matrix[14] = out_matrix[15] = 0.0f;
    
}

void identityMatrix(float out_matrix[16]) {
    zeroMatrix(out_matrix);
    out_matrix[0] = out_matrix[5] = out_matrix[10] = out_matrix[15] = 1.0f;
}

void transpose(float a[16]) {

    float b[16];
    for (int i = 0; i < 16; ++i) {
        b[i] = a[i];
    }

    a[0] = b[0];
    a[1] = b[4];
    a[2] = b[8];
    a[3] = b[12];
    a[4] = b[1];
    a[5] = b[5];
    a[6] = b[9];
    a[7] = b[13];
    a[8] = b[2];
    a[9] = b[6];
    a[10] = b[10];
    a[11] = b[14];
    a[12] = b[3];
    a[13] = b[7];
    a[14] = b[11];
    a[15] = b[15];
}

void printMat4x4(float a[16]) {

    fprintf(stderr, "\t%.3f, %.3f, %.3f, %.3f\n", a[0], a[1], a[2], a[3]);
    fprintf(stderr, "\t%.3f, %.3f, %.3f, %.3f\n", a[4], a[5], a[6], a[7]);
    fprintf(stderr, "\t%.3f, %.3f, %.3f, %.3f\n", a[8], a[9], a[10], a[11]);
    fprintf(stderr, "\t%.3f, %.3f, %.3f, %.3f\n", a[12], a[13], a[14], a[15]);

}

// Invert a 4x4 matrix. Returns 1 if the matrix can be inverted,
// returns 0 if it cannot. 
// https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
int invert(const float m[16], float invOut[16])
{
    double inv[16], det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return 0;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return 1;
}
