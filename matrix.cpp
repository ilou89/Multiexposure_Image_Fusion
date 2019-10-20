#include "matrix.h"

template<typename T>
matrix<T>::matrix(uint rows_, uint columns_)
{
    mat.resize(rows_);
      for (int i=0; i<mat.size(); i++) {
        mat[i].resize(columns_);
      }
      rows    = rows_;
      columns = columns_;

      //Initialize all values to 0.f
      fill(0.f);
}

//template<typename T>
//matrix<T>::matrix(const matrix<T> &rhs)
//{
//    if (&rhs == this)
//        return *this;

//      unsigned new_rows = rhs.get_rows();
//      unsigned new_cols = rhs.get_columns();

//      mat.resize(new_rows);
//      for (unsigned i=0; i<mat.size(); i++) {
//        mat[i].resize(new_cols);
//      }

//      for (unsigned i=0; i<new_rows; i++) {
//        for (unsigned j=0; j<new_cols; j++) {
//          mat[i][j] = rhs(i, j);
//        }
//      }
//      rows    = new_rows;
//      columns = new_cols;

//      return *this;
//}

template<typename T>
matrix<T>::~matrix()
{
    // We do not use dynamic memory allocation (new),
    // thus destruction will do nothing here. Destruction
    // will be handled by the compiler
}

template<typename T>
uint matrix<T>::get_rows()
{
    return rows;
}

template<typename T>
uint matrix<T>::get_columns()
{
    return columns;
}

template<typename T>
T matrix<T>::valueAt(int i, int j)
{
    return mat[i][j];
}

template<typename T>
void matrix<T>::set_cell_value(uint i, uint j, T value)
{
    mat[i][j] = value;
}

template<typename T>
void matrix<T>::fill(T value)
{
    for (int i=0; i<mat.size(); i++) {
      std::fill(mat[i].begin(), mat[i].end(), value);
    }
}

template<typename T>
QImage* matrix<T>::matrix_to_image()
{
    QImage *image = new QImage(this->rows, this->columns, QImage::Format_RGB32);

    for(int i = 0; i < image->width(); ++i){
        for(int j = 0; j < image->height(); ++j){
            QRgb color = qRgb(static_cast<int>(this->valueAt(i, j)), static_cast<int>(this->valueAt(i, j)), static_cast<int>(this->valueAt(i, j)));
            image->setPixel(i, j, color);
        }
    }

    return image;
}
