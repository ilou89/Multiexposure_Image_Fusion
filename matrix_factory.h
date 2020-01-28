#ifndef MATRIX_FACTORY_H
#define MATRIX_FACTORY_H
#include <memory>
#include "matrix_2d.h"

enum MatrixImplementation{
        SW_NAIVE,
        SW_OPTIMIZED,
        OPEN_MP
};

class MatrixFactory
{
public:
    MatrixFactory();
    virtual ~MatrixFactory();

    static std::unique_ptr<Matrix2D<float>> CreateMatrix2D(MatrixImplementation type, uint width, uint height);
};

#endif // MATRIX_FACTORY_H
