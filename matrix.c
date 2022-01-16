#include "matrix.h"

#include <math.h>

// Sort of works, but this seems to be producing a weird zoom level...
void orthographicProjection(float left, float right, float bottom, float top, float near, float far, float out_matrix[16]) {
    identityMatrix(out_matrix);

    out_matrix[0] = 2.0f / (right - left);
    out_matrix[5] = 2.0f / (top - bottom);
    out_matrix[10] = -2.0f / (far - near);
    out_matrix[12] = -(right + left) / (right - left);
    out_matrix[13] = - (top + bottom) / (top - bottom);
    out_matrix[14] = - (far + near) / (far - near);
}

/// Used to set the view matrix when you know the looking point.
void lookDir(const float eye[3], const float target[3], const float up[3], float out_matrix[16]) {

    float xaxis[3];
    float yaxis[3];
    float zaxis[3] = {eye[0] - target[0], eye[1] - target[1], eye[2] - target[1]};
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
    transpose(out_matrix); // why do i need this??
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

float mult(const float a[16], const float b[16], float out_matrix[16]) {
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
