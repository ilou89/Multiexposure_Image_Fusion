#include "matrix_2d.h"
#include "QFile"
#include "QDebug"
#include <math.h>
#include <iostream>
#include <chrono>

template<typename T>
Matrix2D<T>::Matrix2D(const uint width, const uint height)
{
    mat.resize(width);
      for (int i=0; i<mat.size(); i++) {
        mat[i].resize(height);
      }
      columns = width;
      rows    = height;

      //Initialize all values to 0.f
      Fill(0.f);
}

template<typename T>
Matrix2D<T>::Matrix2D(const Matrix2D &p2)
{
    mat.resize(p2.columns);
    for (int i=0; i<mat.size(); i++) {
      mat[i].resize(p2.rows);
    }

    columns    = p2.columns;
    rows = p2.rows;

    for(int i = 0; i < columns; ++i){
        for(int j = 0; j < rows; ++j){
            mat[i][j] = p2.mat[i][j];
        }
    }
}

template<typename T>
Matrix2D<T> Matrix2D<T>::operator+(const Matrix2D &m)
{
    //  if matrices do not have the same size, return original matrix
    if (columns != m.columns || rows != m.rows){
            return (*this);
    }
    Matrix2D<T> new_mat(columns, rows);
    for (int i = 0; i < columns; ++i){
        for (int j = 0; j < rows; ++j){
            new_mat.mat[i][j] = this->mat[i][j] + m.mat[i][j];
        }
    }
    return new_mat;
}

template<typename T>
Matrix2D<T> Matrix2D<T>::operator-(const Matrix2D &m)
{
    //  if matrices do not have the same size, return original matrix
    if (columns != m.columns || rows != m.rows){
            return (*this);
    }
    Matrix2D<T> new_mat(columns, rows);
    for (int i = 0; i < columns; ++i){
        for (int j = 0; j < rows; ++j){
            new_mat.mat[i][j] = this->mat[i][j] - m.mat[i][j];
        }
    }
    return new_mat;
}

template<typename T>
Matrix2D<T> Matrix2D<T>::operator*(const T value)
{
    for (int i = 0; i < columns; ++i){
        for (int j = 0; j < rows; ++j){
            this->mat[i][j] = value*this->mat[i][j];
        }
    }

    return *this;
}

template<typename T>
bool Matrix2D<T>::Compare(const Matrix2D &m2)
{
    for (int i = 0; i < columns; ++i){
        for (int j = 0; j < rows; ++j){
            if ( this->mat[i][j] - m2.mat[i][j] > 1.f) {
                return false;
            }
        }
    }
    return true;
}

template<typename T>
Matrix2D<T>::~Matrix2D()
{
    // We do not use dynamic memory allocation within this class,
    // thus destruction will do nothing here. Make it virtual
    // so that future subclasses can perform their own object destruction
}

template<typename T>
uint Matrix2D<T>::GetRows()
{
    return columns;
}

template<typename T>
uint Matrix2D<T>::GetColumns()
{
    return rows;
}

template<typename T>
T Matrix2D<T>::ValueAt(const int i, const int j)
{
    return mat[i][j];
}

template<typename T>
void Matrix2D<T>::SetCellValue(const int i, const int j, const T value)
{
    mat[i][j] = value;
}

template<typename T>
void Matrix2D<T>::Fill(const T value)
{
    for (int i=0; i<mat.size(); i++) {
      std::fill(mat[i].begin(), mat[i].end(), value);
    }
}

template<typename T>
QImage* Matrix2D<T>::ConvertToQImage()
{
    QImage *image = new QImage(this->columns, this->rows, QImage::Format_RGB32);

    for(int i = 0; i < image->width(); ++i){
        for(int j = 0; j < image->height(); ++j){

            QRgb color = 0xff000000;
            if(this->ValueAt(i, j) > 255.f){
                color = 0xffffffff;
            }else if(this->ValueAt(i, j) < 0.f){
                color = 0xff000000;
            }else{
                color = qRgb(static_cast<int>(this->ValueAt(i, j)), static_cast<int>(this->ValueAt(i, j)), static_cast<int>(this->ValueAt(i, j)));
            }

            image->setPixel(i, j, color);
        }
    }

    return image;
}

template<typename T>
void Matrix2D<T>::FilterMax(const int filter_size)
{
    int half_size = filter_size/2;
    Matrix2D<T> temp_matrix(columns, rows);

    for ( int i = 0; i < columns; ++i ) {
        for ( int j = 0; j < rows; ++j ) {
            T max_value = -1.f;
            for(int k = -half_size; k <= half_size; ++k){
                for(int l = -half_size; l <= half_size; ++l){
                    int row    = i + k;
                    int column = j + l;

                    if(row < 0 || row >= columns){
                        break;
                    }

                    if(column < 0 || column >= rows){
                        continue;
                    }

                    if(mat[row][column] > max_value){
                        max_value = mat[row][column];
                    }
                }
            }

            temp_matrix.mat[i][j] = max_value;
        }
    }

    for(int i = 0; i < columns; ++i){
        for(int j = 0; j < rows; ++j){
            mat[i][j] = temp_matrix.mat[i][j];
        }
    }
}

template<typename T>
void Matrix2D<T>::FilterMin(const int filter_size)
{
    int half_size = filter_size/2;
    Matrix2D<T> temp_matrix(columns, rows);

    for(int i = 0; i < columns; ++i){
        for(int j = 0; j < rows; ++j){

            T min_value = 255.f;

            //Find minimum value inside the filter window
            for(int k = -half_size; k <= half_size; ++k){
                for(int l = -half_size; l <= half_size; ++l){

                    //Mirror pixels at the border of the image
                    int row    = i + k;
                    int column = j + l;

                    if(row < 0){
                        continue;
                    }

                    if(column < 0){
                        continue;
                    }

                    if(row >= columns){
                        continue;
                    }

                    if(column >= rows){
                        continue;
                    }

                    if(mat[row][column] < min_value){
                        min_value = mat[row][column];
                    }
                }
            }
            temp_matrix.mat[i][j] = min_value;
        }
    }

    for(int i = 0; i < columns; ++i){
        for(int j = 0; j < rows; ++j){
            mat[i][j] = temp_matrix.mat[i][j];
        }
    }
}

template<typename T>
void Matrix2D<T>::FilterMean(const int filter_size)
{
    //Mean Filter is separable, thus the convolution will be performed in two passes: row and column-wise
    int half_size = static_cast<int>(floor(filter_size/2));

    Matrix2D<T> temp_matrix(columns, rows);

#if 1
    //Row-wise convolution
    for(int i = 0; i < columns; ++i ) {
        for(int j = 0; j < rows; ++j){
            T new_value      = 0.f;
            int pixels_count = 0;
            for(int k = -half_size; k <= half_size; ++k){
                int row = i + k;

                //symmetric padding
                if (row < 0 ) {
                    row = -row;
                }
                if ( row >= columns) {
                    row -= columns;
                }

                if ( row >= 0 && row < columns ) {
                    pixels_count++;
                    new_value += mat[row][j];
                }
            }
            temp_matrix.mat[i][j] = new_value/pixels_count;
        }
    }

    Matrix2D<T> temp_matrix2(columns, rows);
    //Column-wise convolution
    for(int i = 0; i < columns; ++i ) {
        for(int j = 0; j < rows; ++j){
            T new_value      = 0.f;
            int pixels_count = 0;
            for(int k = -half_size; k <= half_size; ++k){
                int column = j + k;

                //symmetric padding
                if (column < 0 ) {
                    column = -column;
                }
                if ( column >= rows) {
                    column -= column;
                }

                if ( column >= 0 && column < rows ) {
                    pixels_count++;
                    new_value += temp_matrix.mat[i][column];
                }
            }
            temp_matrix2.mat[i][j] = new_value/pixels_count;
        }
    }

#else

//Naive convolution
    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < columns; ++j){

                T      new_value = 0.f;
                int pixels_count = 0;
                for(int k = -half_size; k <= half_size; ++k){
                    for(int l = -half_size; l <= half_size; ++l){

                        //Mirror pixels at the border of the image
                        int row    = i + k;
                        int column = j + l;

                        if(row < 0){
                            row = -row;
                        }

                        if(column < 0){
                            column = -column;
                        }

                        if(row >= rows){
                            row -= rows;
                        }

                        if(column >= columns){
                            column -= columns;
                        }

                        if ( (row >= 0 && row < rows) && (column >= 0 && column < columns) ){
                            pixels_count++;
                            new_value += mat[row][column];
                        }else{
                            qDebug()<<"out of scope"<<row<<column;
                        }
                    }
                }
                if(pixels_count != filter_size*filter_size){
                    qDebug()<<"wrong elements in convolution";
                }
//                qDebug()<<"pixels count"<<pixels_count<<"filter size"<<filter_size;
                temp_matrix.mat[i][j] = new_value/pixels_count;
        }
    }
#endif
    for(int i = 0; i < columns; ++i){
        for(int j = 0; j < rows; ++j){
            mat[i][j] = temp_matrix.mat[i][j];
        }
    }
}

template<typename T>
void Matrix2D<T>::ScaleToInterval(const T start, const T end)
{
    const T min = GetMinValue();
    const T max = GetMaxValue();

    for ( int i = 0; i < columns; ++i ) {
        for ( int j = 0; j < rows; ++j ) {
            mat[i][j] = start + (end - start) * ( mat[i][j] - min ) / ( max - min );
        }
    }
}

template<typename T>
T Matrix2D<T>::GetMinValue()
{
    T min = mat[0][0];
    for ( int i = 0; i < columns; ++i ) {
        for ( int j = 0; j < rows; ++j ) {
            if (mat[i][j] < min) {
                min = mat[i][j];
            }
        }
    }

    return min;
}

template<typename T>
T Matrix2D<T>::GetMaxValue()
{
    T max = mat[0][0];
    for ( int i = 0; i < columns; ++i ) {
        for ( int j = 0; j < rows; ++j ) {
            if (mat[i][j] > max) {
                max = mat[i][j];
            }
        }
    }

    return max;
}

template<typename T>
void Matrix2D<T>::SaveToFile(const QString filename)
{
    qDebug()<<"saving to"<<filename;

    QFile file( filename );
    if ( file.open(QIODevice::ReadWrite) )
    {
        QTextStream stream( &file );

        for( int i = 0; i < columns; ++i ) {
            stream << "[ ";
            for( int j = 0; j < rows; ++j ) {
                stream << QString::number(mat[i][j], 'f', 2) <<", ";
            }
            stream << "]\n";
        }
    }
    file.close();
}
