#include "matrix_factory.h"

MatrixFactory::MatrixFactory()
{

}

MatrixFactory::~MatrixFactory()
{

}

std::unique_ptr<Matrix2D<float> > MatrixFactory::CreateMatrix2D(MatrixImplementation type, uint width, uint height)
{
    switch ( type ) {
//        case SW_NAIVE:
//        case SW_OPTIMIZED:
//        case OPEN_MP:
        default:
            return std::make_unique<Matrix2D<float>>(width, height);
    }
}
