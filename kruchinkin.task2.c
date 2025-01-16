#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <math.h>

// Структура для представления матрицы.  Содержит количество строк и столбцов, а также указатель на данные.
typedef struct {
    size_t columns;  // Количество столбцов в матрице
    size_t rows;    // Количество строк в матрице
    double* elements; // Указатель на динамически выделенный массив, хранящий элементы матрицы
} Matrix;

// Перечисление для кодов ошибок матрицы.
typedef enum { MATRIX_ERROR } MatrixError;


// Функция обработки ошибок матрицы. Выводит сообщение об ошибке в stderr.
void matrix_error_handler(MatrixError code, const char* message) {
    if (code == MATRIX_ERROR) {
        fprintf(stderr, "Ошибка: %s\n", message);
    }
}


// Функция создания матрицы заданного размера.  Обрабатывает возможные ошибки выделения памяти.
Matrix create_matrix(size_t cols, size_t rows) {
    Matrix m = {cols, rows, NULL};
    if (cols == 0 || rows == 0) {
        matrix_error_handler(MATRIX_ERROR, "Размеры матрицы должны быть положительными.");
        return (Matrix){0, 0, NULL};
    }
    if (cols > SIZE_MAX / rows || (cols * rows) > SIZE_MAX / sizeof(double)) {
        matrix_error_handler(MATRIX_ERROR, "Матрица слишком большая для доступной памяти.");
        return (Matrix){0, 0, NULL};
    }

    m.elements = (double*)malloc(cols * rows * sizeof(double));
    if (m.elements == NULL) {
        matrix_error_handler(MATRIX_ERROR, "Ошибка выделения памяти.");
        return (Matrix){0, 0, NULL};
    }
    return m;
}


// Функция освобождения памяти, занятой матрицей.  Обрабатывает случай NULL-указателя.
void free_matrix(Matrix* m) {
    if (m == NULL || m->elements == NULL) return;
    free(m->elements);
    m->columns = 0;
    m->rows = 0;
    m->elements = NULL;
}


// Функция получения элемента матрицы по индексу строки и столбца.
double get_element(const Matrix m, size_t row, size_t col) {
    return m.elements[m.columns * row + col];
}


// Функция установки элементов матрицы с помощью массива значений.
void set_elements(Matrix m, const double* values) {
    memcpy(m.elements, values, m.rows * m.columns * sizeof(double));
}


// Функция сложения двух матриц. Проверяет совместимость размеров перед сложением.
Matrix matrix_sum(const Matrix a, const Matrix b) {
    if (a.rows != b.rows || a.columns != b.columns) {
        matrix_error_handler(MATRIX_ERROR, "Несовместимые размеры матриц для сложения.");
        return (Matrix){0, 0, NULL};
    }
    Matrix result = create_matrix(a.columns, a.rows);
    for (size_t i = 0; i < result.rows * result.columns; ++i) {
        result.elements[i] = a.elements[i] + b.elements[i];
    }
    return result;
}


// Функция вычитания двух матриц. Проверяет совместимость размеров перед вычитанием.
Matrix matrix_difference(const Matrix a, const Matrix b) {
    if (a.rows != b.rows || a.columns != b.columns) {
        matrix_error_handler(MATRIX_ERROR, "Несовместимые размеры матриц для вычитания.");
        return (Matrix){0, 0, NULL};
    }
    Matrix result = create_matrix(a.columns, a.rows);
    for (size_t i = 0; i < result.rows * result.columns; ++i) {
        result.elements[i] = a.elements[i] - b.elements[i];
    }
    return result;
}


// Функция умножения матрицы на скаляр.
Matrix scalar_multiply(const Matrix a, double scalar) {
    Matrix result = create_matrix(a.columns, a.rows);
    for (size_t i = 0; i < result.rows * result.columns; ++i) {
        result.elements[i] = a.elements[i] * scalar;
    }
    return result;
}


// Функция перемножения двух матриц. Проверяет совместимость размеров перед умножением.
Matrix matrix_product(const Matrix a, const Matrix b) {
    if (a.columns != b.rows) {
        matrix_error_handler(MATRIX_ERROR, "Несовместимые размеры матриц для умножения.");
        return (Matrix){0, 0, NULL};
    }
    Matrix result = create_matrix(b.columns, a.rows);
    for (size_t i = 0; i < result.rows; ++i) {
        for (size_t j = 0; j < result.columns; ++j) {
            result.elements[i * result.columns + j] = 0.0;
            for (size_t k = 0; k < a.columns; ++k) {
                result.elements[i * result.columns + j] += a.elements[i * a.columns + k] * b.elements[k * b.columns + j];
            }
        }
    }
    return result;
}


// Рекурсивная функция вычисления факториала числа.
size_t factorial_recursive(size_t n) {
    if (n == 0) return 1;
    return n * factorial_recursive(n - 1);
}


// Функция создания единичной матрицы заданного размера.
Matrix identity_matrix(size_t size) {
    Matrix I = create_matrix(size, size);
    for (size_t i = 0; i < size * size; i += size + 1) {
        I.elements[i] = 1.0;
    }
    return I;
}


// Функция вычисления экспоненты матрицы (e^A) с помощью приближения ряда Тейлора.
Matrix matrix_exponential(const Matrix a) {
    if (a.columns != a.rows) {
        matrix_error_handler(MATRIX_ERROR, "Матрица должна быть квадратной для вычисления экспоненты.");
        return (Matrix){0, 0, NULL};
    }
    Matrix result = identity_matrix(a.columns);
    Matrix term = identity_matrix(a.columns);
    for (size_t n = 1; n <= 20; ++n) {
        Matrix prev_term = term;
        term = matrix_product(term, a);
        free_matrix(&prev_term);
        Matrix scaled_term = scalar_multiply(term, 1.0 / factorial_recursive(n));
        Matrix prev_result = result;
        result = matrix_sum(result, scaled_term);
        free_matrix(&prev_result);
        free_matrix(&scaled_term);
    }
    free_matrix(&term);
    return result;
}


// Функция получения подматрицы путем удаления строки и столбца.
Matrix submatrix(const Matrix m, size_t row, size_t col) {
    Matrix sub = create_matrix(m.columns - 1, m.rows - 1);
    size_t idx = 0;
    for (size_t i = 0; i < m.rows; ++i) {
        if (i == row) continue;
        for (size_t j = 0; j < m.columns; ++j) {
            if (j == col) continue;
            sub.elements[idx++] = m.elements[i * m.columns + j];
        }
    }
    return sub;
}


// Рекурсивная функция вычисления определителя матрицы.
double determinant(const Matrix m) {
    if (m.columns != m.rows) {
        matrix_error_handler(MATRIX_ERROR, "Матрица должна быть квадратной для вычисления определителя.");
        return NAN;
    }
    if (m.rows == 2) {
        return m.elements[0] * m.elements[3] - m.elements[1] * m.elements[2];
    }
    double det = 0.0;
    for (size_t i = 0; i < m.columns; ++i) {
        Matrix sub = submatrix(m, 0, i);
        double sign = (i % 2 == 0) ? 1.0 : -1.0;
        det += sign * m.elements[i] * determinant(sub);
        free_matrix(&sub);
    }
    return det;
}

// Функция вычисления транспонированной матрицы
Matrix transpose(const Matrix m) {
  Matrix t = create_matrix(m.rows, m.columns);
  for(size_t i = 0; i < m.rows; i++) {
    for(size_t j = 0; j < m.columns; j++) {
      t.elements[j * m.rows + i] = m.elements[i * m.columns + j];
    }
  }
  return t;
}


// Функция вычисления обратной матрицы.  Проверяет на вырожденность.
Matrix matrix_inverse(const Matrix a) {
    double det = determinant(a);
    if (fabs(det) < 1e-6) { // проверка на близкий к нулю определитель
        matrix_error_handler(MATRIX_ERROR, "Матрица вырожденная (или почти вырожденная) и не может быть инвертирована.");
        return (Matrix){0, 0, NULL};
    }
    Matrix t = transpose(a);
    Matrix inverse = scalar_multiply(t, 1.0 / det);
    free_matrix(&t);
    return inverse;
}


// Функция вывода матрицы на консоль.
void print_matrix(const Matrix m) {
    for (size_t i = 0; i < m.rows; ++i) {
        for (size_t j = 0; j < m.columns; ++j) {
            printf("%5.2f ", get_element(m, i, j));
        }
        printf("\n");
    }
}


int main() {
    Matrix a = create_matrix(2, 2);
    Matrix b = create_matrix(2, 2);

    set_elements(a, (double[]){1.0, 2.0, 3.0, 4.0});
    set_elements(b, (double[]){5.0, 6.0, 7.0, 8.0});

    printf("Matrix A:\n");
    print_matrix(a);
    printf("Matrix B:\n");
    print_matrix(b);

    Matrix c = matrix_sum(a, b);
    printf("A + B:\n");
    print_matrix(c);

    Matrix d = matrix_difference(a, b);
    printf("A - B:\n");
    print_matrix(d);

    Matrix e = scalar_multiply(a, 2.0);
    printf("2 * A:\n");
    print_matrix(e);

    Matrix f = matrix_product(a, b);
    printf("A * B:\n");
    print_matrix(f);

    Matrix exp_a = matrix_exponential(a);
    printf("e^A:\n");
    print_matrix(exp_a);

    double det_a = determinant(a);
    printf("Determinant of A: %f\n", det_a);

    Matrix inv_a = matrix_inverse(a);
    printf("Inverse of A:\n");
    print_matrix(inv_a);


    free_matrix(&a);
    free_matrix(&b);
    free_matrix(&c);
    free_matrix(&d);
    free_matrix(&e);
    free_matrix(&f);
    free_matrix(&exp_a);
    free_matrix(&inv_a);

    return 0;
}