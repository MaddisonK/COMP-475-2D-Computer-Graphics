#include "GMatrix.h"
#include <iostream>
#include <math.h>

GMatrix::GMatrix() : GMatrix(1, 0, 0, 0, 1, 0) {}

GMatrix GMatrix::Translate(float tx, float ty) {
    return GMatrix(1, 0, tx, 0, 1, ty);
}

GMatrix GMatrix::Scale(float sx, float sy) {
    return GMatrix(sx, 0, 0, 0, sy, 0);
}

GMatrix GMatrix::Rotate(float radians) {
    return GMatrix(cos(radians), -sin(radians), 0, sin(radians), cos(radians), 0);
}

GMatrix GMatrix::Concat(GMatrix const& A, GMatrix const& B) {
    GMatrix m = GMatrix();
    m[0] = A[0] * B[0] + A[1] * B[3];
    m[1] = A[0] * B[1] + A[1] * B[4];
    m[2] = A[0] * B[2] + A[1] * B[5] + A[2];
    m[3] = A[3] * B[0] + A[4] * B[3];
    m[4] = A[3] * B[1] + A[4] * B[4];
    m[5] = A[3] * B[2] + A[4] * B[5] + A[5];
    return m;
}

bool GMatrix::invert(GMatrix* inverse) const {
    float det = (*this)[0] * (*this)[4] - (*this)[1] * (*this)[3];
    if (det == 0.0f) {return false;}
    float invdet = 1 / det;
    GMatrix m;
    m[0] = (*this)[4];
    m[1] = -1*(*this)[1];
    m[2] = (*this)[1]*(*this)[5]-(*this)[2]*(*this)[4];
    m[3] = -1*(*this)[3];
    m[4] = (*this)[0];
    m[5] = -1*((*this)[0]*(*this)[5]-(*this)[2]*(*this)[3]);
    (*inverse)[0] = m[0]*invdet;
    (*inverse)[1] = m[1]*invdet;
    (*inverse)[2] = m[2]*invdet;
    (*inverse)[3] = m[3]*invdet;
    (*inverse)[4] = m[4]*invdet;
    (*inverse)[5] = m[5]*invdet;
    return true;
}

void GMatrix::mapPoints(GPoint dst[], const GPoint src[], int count) const {
    for (int i = 0; i < count; i++) {
        float fX = src[i].x() * (*this)[0] + src[i].y() * (*this)[1] + (*this)[2];
        dst[i].fY = src[i].x() * (*this)[3] + src[i].y() * (*this)[4] + (*this)[5];
        dst[i].fX = fX;
    }
}

void printMatrix(GMatrix const& A) {
    std::cout << "[";
    for (int i = 0; i<3; i++) {
        std::cout << roundf(A[i]) << " ";
    }
    std::cout << "\n";
    for (int i = 3; i < 6; i++) {
        std::cout << " " << roundf(A[i]);
    }
    std::cout << "]\n";
}