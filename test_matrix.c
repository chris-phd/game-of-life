#include <stdio.h>
#include <math.h>

#include "matrix.h"

int isEqual(const float a[16], const float b[16]);
int isEqualCol(const float a[4], const float b[4]);

int main(void) {

    float a[16] = {1.0f, 1.0f, 1.0f, -1.0f,
                   1.0f, 1.0f, -1.0f, 1.0f,
                   1.0f, -1.0f, 1.0f, 1.0f,
                   -1.0f, 1.0f, 1.0f, 1.0f};
    float a_inv_correct[16] = {0.25f, 0.25f, 0.25f, -0.25f,
                               0.25f, 0.25f, -0.25f, 0.25f,
                               0.25f, -0.25f, 0.25f, 0.25f,
                               -0.25f, 0.25f, 0.25f, 0.25f};
    
    float a_inv_calc[16];
    if (!invert(a, a_inv_calc)) {
        fprintf(stderr, "test_matrix: FAILED TEST 1");
        return 1;
    }

    if (!isEqual(a_inv_calc, a_inv_correct)) {
        fprintf(stderr, "test_matrix: FAILED TEST 2");
        fprintf(stderr, "     Expected\n");
        printMat4x4(a_inv_correct);
        fprintf(stderr, "     Calculated\n");
        printMat4x4(a_inv_calc);
        return 2;
    }

    // Matrix is not invertable, since it does not have full rank. 
    float b[16] = {1.0f, 1.0f, 1.0f, -1.0f,
                   1.0f, 1.0f, 1.0f, -1.0f,
                   1.0f, -1.0f, 1.0f, 1.0f,
                   -1.0f, 1.0f, 1.0f, 1.0f};
    
    float b_inv_calc[16];
    if (invert(b, b_inv_calc)) {
        fprintf(stderr, "test_matrix: FAILED TEST 3");
        return 3;
    }

    // Test matrix multiplication
    float c[16] = {0.0f, -1.0f, 0.0f, 0.0f,
                   1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 1.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f};
    float d[16] = {0.0f, 0.0f, -1.0f, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                   1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f};
    float cd_correct[16] = {0.0f, -1.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, -1.0f, 0.0f,
                            1.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f};
    float cd_calc[16];
    mult(c, d, cd_calc);
    if (!isEqual(cd_calc, cd_correct)) {
        fprintf(stderr, "test_matrix: FAILED TEST 4\n");
        return 4;
    }

    // Matrix x Col multiplication
    float e[16] = {0.0f, -1.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, -1.0f, 0.0f,
                   1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f};
    float f[16] = {1.0f, 0.0f, 0.0f, 1.0f};
    float ef_correct[4] = {0.0f, 0.0f, 1.0f, 1.0f};
    float ef_calc[4];
    multCol(e, f, ef_calc);
    if (!isEqualCol(cd_calc, cd_correct)) {
        fprintf(stderr, "test_matrix: FAILED TEST 5\n");
        return 5;
    }


    fprintf(stderr, "test_matrix: ALL TESTS PASSED\n");
    return 0;
}


int isEqual(const float a[16], const float b[16]) {
    float tol = 1e-9;
    for (int i = 0; i < 16; ++i) {
        if (fabs(a[i] - b[i]) > tol)
            return 0;
    }
    return 1;
}

int isEqualCol(const float a[4], const float b[4]) {
    float tol = 1e-9;
    for (int i = 0; i < 4; ++i) {
        if (fabs(a[i] - b[i]) > tol)
            return 0;
    }
    return 1;
}
