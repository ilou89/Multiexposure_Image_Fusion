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

template<typename T>
matrix<T>::matrix(const matrix &p2)
{
    mat.resize(p2.rows);
    for (int i=0; i<mat.size(); i++) {
      mat[i].resize(p2.columns);
    }

    rows    = p2.rows;
    columns = p2.columns;

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < columns; ++j){
            mat[i][j] = p2.mat[i][j];
        }
    }
}

template<typename T>
matrix<T> matrix<T>::operator-(const matrix &m)
{
    //  if matrices do not have the same size, return original matrix
    if (rows != m.rows || columns != m.columns){
        qDebug()<<"pou pas re karamitro";
//            cerr << "Matrix sizes do not match.";
            return (*this);
    }
    matrix<T> new_mat(rows, columns);
    for (int i = 0; i < rows; ++i){
        for (int j = 0; j < columns; ++j){
//            qDebug()<<i<<j<<this->mat[i][j] << m.mat[i][j];
            new_mat.mat[i][j] = this->mat[i][j] - m.mat[i][j];
        }
    }
    return new_mat;
}

template<typename T>
matrix<T> matrix<T>::operator*(const T value)
{
    qDebug()<<"mulitplying scalar with matrix";
    for (int i = 0; i < rows; ++i){
        for (int j = 0; j < columns; ++j){
            this->mat[i][j] = value*this->mat[i][j];
        }
    }

    return *this;
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

            QRgb color = 0xff000000;
            if(this->valueAt(i, j) > 255.f){
                color = 0xffffffff;
            }else if(this->valueAt(i, j) < 0.f){
                color = 0xff000000;
            }else{
                color = qRgb(static_cast<int>(this->valueAt(i, j)), static_cast<int>(this->valueAt(i, j)), static_cast<int>(this->valueAt(i, j)));
            }

            image->setPixel(i, j, color);
        }
    }

    return image;
}

//Naive implementation of max filter: TODO replace with Van Herk algorithm
template<typename T>
void matrix<T>::filterMax(int filter_size)
{
    int half_size = filter_size/2;
    matrix<float> temp_matrix(rows, columns);

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < columns; ++j){

            float max_value = 0.f;
            for(int k = -half_size; k <= half_size; ++k){
                for(int l = -half_size; l <= half_size; ++l){

                    //Mirror pixels at the border of the image
                    int row    = i + k;
                    int column = j + l;

                    if(row < 0){
                        continue;
//                        row = -row;
                    }

                    if(column < 0){
                        continue;
//                        column = -column;
                    }

                    if(row >= rows){
                        continue;
//                        row = row - rows;
                    }

                    if(column >= columns){
                        continue;
//                        column = column - columns;
                    }

                    if(mat[row][column] > max_value){
                        max_value = mat[row][column];
                    }
                }
            }

            for(int k = -half_size; k <= half_size; ++k){
                for(int l = -half_size; l <= half_size; ++l){

                    //Mirror pixels at the border of the image
                    int row    = i + k;
                    int column = j + l;

                    if(row < 0){
                        continue;
//                        row = -row;
                    }

                    if(column < 0){
                        continue;
//                        column = -column;
                    }

                    if(row >= rows){
                        continue;
//                        row -= rows;
                    }

                    if(column >= columns){
                        continue;
//                        column -= columns;
                    }

                    temp_matrix.mat[row][column] = max_value;
                }
            }
        }
    }

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < columns; ++j){
            mat[i][j] = temp_matrix.mat[i][j];
        }
    }
}

template<typename T>
void matrix<T>::filterMin(int filter_size)
{
    int half_size = filter_size/2;
    qDebug()<<"half size"<<half_size;
    matrix<float> temp_matrix(rows, columns);

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < columns; ++j){

            float min_value = 255.f;

            //Find minimum value inside the filter window
            for(int k = -half_size; k <= half_size; ++k){
                for(int l = -half_size; l <= half_size; ++l){

                    //Mirror pixels at the border of the image
                    int row    = i + k;
                    int column = j + l;

                    if(row < 0){
//                        row = -row;
                        continue;
                    }

                    if(column < 0){
//                        column = -column;
                        continue;
                    }

                    if(row >= rows){
//                        row = row - rows;
                        continue;
                    }

                    if(column >= columns){
//                        column = column - columns;
                        continue;
                    }

                    if(mat[row][column] < min_value){
                        min_value = mat[row][column];
                    }
                }
            }

            //replace values inside window with the minimum value found earlier
            for(int k = -half_size; k <= half_size; ++k){
                for(int l = -half_size; l <= half_size; ++l){

                    //Mirror pixels at the border of the image
                    int row    = i + k;
                    int column = j + l;

                    if(row < 0){
                        continue;
//                        row = -row;
                    }

                    if(column < 0){
                        continue;
//                        column = -column;
                    }

                    if(row >= rows){
                        continue;
                        row -= rows;
                    }

                    if(column >= columns){
                        continue;
//                        column -= columns;
                    }

                    temp_matrix.mat[row][column] = min_value;
                }
            }
        }
    }

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < columns; ++j){
            mat[i][j] = temp_matrix.mat[i][j];
        }
    }
}

template<typename T>
void matrix<T>::filterMean(int filter_size)
{
    int half_size = filter_size/2;
    matrix<float> temp_matrix(rows, columns);

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < columns; ++j){

                float new_value = 0.f;
                int pixels_count = 0;
                for(int k = -half_size; k <= half_size; ++k){
                    for(int l = -half_size; l <= half_size; ++l){

                        //Mirror pixels at the border of the image
                        int row    = i + k;
                        int column = j + l;

                        if(row < 0){
                            continue;
//                            row = -row;
                        }

                        if(column < 0){
                            continue;
//                            column = -column;
                        }

                        if(row >= rows){
                            continue;
//                            row -= rows;
                        }

                        if(column >= columns){
                            continue;
//                            column -= columns;
                        }
                        pixels_count++;
                        new_value += mat[row][column];
                    }
                }
                temp_matrix.mat[i][j] = new_value/pixels_count;
        }
    }

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < columns; ++j){
            mat[i][j] = temp_matrix.mat[i][j];
        }
    }
}
