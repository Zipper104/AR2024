#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <math.h>

struct Matrix {
    size_t rows;
    size_t cols;
    double *data;
};
typedef struct Matrix Matrix;

enum MatrixExceptionLevel {ERROR, WARNING, INFO, DEBUG};

void matrix_exception(const enum MatrixExceptionLevel level, const char *msg)
{
    if (level == ERROR) {
        printf("ERROR: %s", msg);
    }
    if (level == WARNING) {
        printf("WARNING: %s", msg);
    }
}

Matrix matrix_allocate(const size_t rows, const size_t cols)
{
    Matrix A = {0, 0, NULL};
    if (rows == 0 || cols == 0) {
        return (Matrix){rows, cols, NULL};
    }

    A.data = malloc(rows * cols * sizeof(double));
    if (A.data == NULL) {
        matrix_exception(ERROR, "Memory allocation failed.");
        return A;
    }

    A.rows = rows;
    A.cols = cols;
    return A;
}

void matrix_free(Matrix *A)
{
    if (A->data != NULL) {
        free(A->data);
        A->data = NULL;  // Prevent dangling pointer
    }
    *A = (Matrix){0, 0, NULL};
}

void matrix_set(Matrix *A, const double *values)
{
    if (A->data != NULL) {
        memcpy(A->data, values, A->rows * A->cols * sizeof(double));
    }
}

void matrix_print(const Matrix A)
{
    for (size_t row = 0; row < A.rows; ++row) {
        for (size_t col = 0; col < A.cols; ++col) {
            printf("%2.3f ", A.data[row * A.cols + col]);
        }
        printf("\n");
    }
    printf("\n");
}

int factorial(int i)
{
    if (i == 0) return 1;
    else return i * factorial(i - 1);
}

// A += B
Matrix matrix_add(const Matrix A, const Matrix B)
{
    if (!((A.cols == B.cols) && (A.rows == B.rows))) {
        matrix_exception(ERROR, "matrix_add: incompatible sizes.");
        return (Matrix){0, 0, NULL};
    }

    Matrix result = matrix_allocate(A.rows, A.cols);
    if (result.data == NULL) return result; // Handle allocation failure

    for (size_t idx = 0; idx < result.rows * result.cols; ++idx) {
        result.data[idx] = A.data[idx] + B.data[idx];
    }
    return result;
}

// A -= B
Matrix matrix_sub(const Matrix A, const Matrix B)
{
    if (!((A.cols == B.cols) && (A.rows == B.rows))) {
        matrix_exception(ERROR, "matrix_sub: incompatible sizes.");
        return (Matrix){0, 0, NULL};
    }

    Matrix result = matrix_allocate(A.rows, A.cols);
    if (result.data == NULL) return result; // Handle allocation failure

    for (size_t idx = 0; idx < result.rows * result.cols; ++idx) {
        result.data[idx] = A.data[idx] - B.data[idx];
    }
    return result;
}

// A *= B (element-wise multiplication)
Matrix matrix_mult(const Matrix A, const Matrix B)
{
    if (!((A.cols == B.cols) && (A.rows == B.rows))) {
        matrix_exception(ERROR, "matrix_mult: incompatible sizes.");
        return (Matrix){0, 0, NULL};
    }

    Matrix result = matrix_allocate(A.rows, A.cols);
    if (result.data == NULL) return result; // Handle allocation failure

    for (size_t idx = 0; idx < result.rows * result.cols; ++idx) {
        result.data[idx] = A.data[idx] * B.data[idx];
    }
    return result;
}

// A *= number
Matrix matrix_mult_numb(const Matrix A, double number)
{
    Matrix result = matrix_allocate(A.rows, A.cols);
    if (result.data == NULL) return result; // Handle allocation failure

    if (A.data != NULL) {
        for (size_t idx = 0; idx < result.rows * result.cols; ++idx) {
            result.data[idx] = A.data[idx] * number;
        }
    }
    return result;
}

// A /= number
Matrix matrix_del_numb(const Matrix A, double number)
{
    Matrix result = matrix_allocate(A.rows, A.cols);
    if (result.data == NULL) return result; // Handle allocation failure

    if (A.data != NULL) {
        for (size_t idx = 0; idx < result.rows * result.cols; ++idx) {
            result.data[idx] = A.data[idx] / number;
        }
    }
    return result;
}

// A*B
Matrix matrix_multiply(const Matrix A, const Matrix B) {
    if (A.cols != B.rows) {
        matrix_exception(ERROR, "matrix_multiply: incompatible sizes.");
        return (Matrix){0, 0, NULL};
    }

    Matrix result = matrix_allocate(A.rows, B.cols);
    if (result.data == NULL) return result; // Handle allocation failure

    for (size_t i = 0; i < A.rows; i++) {
        for (size_t j = 0; j < B.cols; j++) {
            result.data[i * B.cols + j] = 0;
            for (size_t k = 0; k < A.cols; k++) {
                result.data[i * B.cols + j] += A.data[i * A.cols + k] * B.data[k * B.cols + j];
            }
        }
    }
    return result;
}

// e^A
Matrix matrix_exp(const Matrix A) {
    Matrix result = matrix_allocate(A.rows, A.cols);
    Matrix A_power = matrix_allocate(A.rows, A.cols);
    if (result.data == NULL || A_power.data == NULL) {
        matrix_free(&result);
        matrix_free(&A_power);
        return (Matrix){0, 0, NULL}; // Return NULL matrix on failure
    }
    
    memcpy(A_power.data, A.data, A.rows * A.cols * sizeof(double)); // A^1 = A

    // e^A (first 20 terms)
    for (int n = 1; n < 20; n++) {
        // A^n / n!
        double factorial_n = factorial(n);
        Matrix term = matrix_mult_numb(A_power, 1.0 / factorial_n);
        if (term.data == NULL) {
            matrix_free(&result);
            matrix_free(&A_power);
            return (Matrix){0, 0, NULL}; // Return NULL matrix on failure
        }

        // +result  
        Matrix temp = matrix_add(result, term);
        matrix_free(&result);
        result = temp;

        // A_power = A^(n-1) * A
        Matrix new_power = matrix_multiply(A_power, A);
        matrix_free(&A_power);
        A_power = new_power;

        matrix_free(&term);
    }

    matrix_free(&A_power);
    return result;
}

Matrix matrix_minor(const Matrix A, size_t row, size_t col) 
{
    Matrix minor = matrix_allocate(A.rows - 1, A.cols - 1);
    if (minor.data == NULL) return minor; 
    
    size_t idx = 0;
    for (size_t rows = 0; rows < A.rows; ++rows) {
        if (rows == row) continue;
        for (size_t cols = 0; cols < A.cols; ++cols) {
            if (cols == col) continue;
            minor.data[idx++] = A.data[rows * A.cols + cols];
        }
    }
    
    return minor;
}

double matrix_determinant(const Matrix A) {
    if (A.rows != A.cols) {
        matrix_exception(ERROR, "not square matrix");
        return 0.0; 
    }

    if (A.rows == 1) {
        return A.data[0]; // 1x1 matrix
    }
    
    if (A.rows == 2) {
        return A.data[0] * A.data[3] - A.data[1] * A.data[2]; // 2x2 matrix
    }

    double det = 0.0;
    for (size_t col = 0; col < A.cols; col++) {
        Matrix minor = matrix_minor(A, 0, col);
        if (minor.data != NULL) {
            det += (col % 2 == 0 ? 1 : -1) * A.data[col] * matrix_determinant(minor);
            matrix_free(&minor); // Free allocated minor
        }
    }
    return det;
}

Matrix matrix_adjugate(const Matrix A) {
    Matrix adj = matrix_allocate(A.rows, A.cols);
    if (adj.data == NULL) return adj; 

    for (size_t r = 0; r < A.rows; ++r) {
        for (size_t c = 0; c < A.cols; ++c) {
            Matrix minor = matrix_minor(A, r, c);
            double sign = (r + c) % 2 == 0 ? 1 : -1;
            adj.data[c * A.cols + r] = sign * matrix_determinant(minor);
            matrix_free(&minor);
        }
    }
    
    return adj;
}


Matrix matrix_inverse(const Matrix A) {
    double det = matrix_determinant(A);
    if (det == 0) {
        matrix_exception(ERROR, "Matrix is singular and cannot be inverted.");
        return (Matrix){0, 0, NULL}; 
    }

    Matrix adj = matrix_adjugate(A);
    Matrix inv = matrix_allocate(A.rows, A.cols);
    if (inv.data == NULL) {
        matrix_free(&adj);
        return (Matrix){0, 0, NULL}; 
    }

    for (size_t i = 0; i < A.rows * A.cols; ++i) {
        inv.data[i] = adj.data[i] / det;
    }

    matrix_free(&adj);
    return inv;
}

int main()
{
    Matrix A, B;
    A = matrix_allocate(2, 2);
    B = matrix_allocate(2, 2);

    double valuesA[] = {1., 2., 3., 4.};
    double valuesB[] = {1., 2., 5., 1.};

    matrix_set(&A, valuesA);
    matrix_set(&B, valuesB);
    matrix_print(A);
    matrix_print(B);

    Matrix C = matrix_add(A, B);
    matrix_print(C);

    Matrix D = matrix_sub(A, B);
    matrix_print(D);

    Matrix E = matrix_mult(A, B);
    matrix_print(E);

    Matrix F = matrix_mult_numb(A, 4);
    matrix_print(F);

    Matrix expA = matrix_exp(A);
    matrix_print(expA);

    double detA = matrix_determinant(A);
    printf("Determinant of A: %f", detA);    
    
    Matrix G = matrix_inverse(A);
    matrix_print(G);

    // Free allocated matrices
    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&C);
    matrix_free(&D);
    matrix_free(&E);
    matrix_free(&F);
    matrix_free(&expA);
    matrix_free(&G);

    return 0;
}
