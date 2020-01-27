#include "matrix_2d.h"
#include "QFile"
#include "QDebug"
#include <math.h>
#include <iostream>
#include <chrono>

template<typename T>
Matrix2D<T>::Matrix2D(const uint width_, const uint height_)
{
    mat.resize(width_);
      for (int i=0; i<mat.size(); i++) {
        mat[i].resize(height_);
      }
      width  = width_;
      height = height_;

      //Initialize all values to 0.f
      Fill(0.f);
}

template<typename T>
Matrix2D<T>::Matrix2D(const Matrix2D &p2)
{
    mat.resize(p2.width);
    for (int i=0; i<mat.size(); i++) {
      mat[i].resize(p2.height);
    }

    width  = p2.width;
    height = p2.height;

    for(int i = 0; i < width; ++i){
        for(int j = 0; j < height; ++j){
            mat[i][j] = p2.mat[i][j];
        }
    }
}

template<typename T>
Matrix2D<T> Matrix2D<T>::operator+(const Matrix2D &m)
{
    //  if matrices do not have the same size, return original matrix
    if (width != m.width || height != m.height){
            return (*this);
    }
    Matrix2D<T> new_mat(width, height);
    for (int x = 0; x < width; ++x){
        for (int y = 0; y < height; ++y){
            new_mat.mat[x][y] = this->mat[x][y] + m.mat[x][y];
        }
    }
    return new_mat;
}

template<typename T>
Matrix2D<T> Matrix2D<T>::operator-(const Matrix2D &m)
{
    //  if matrices do not have the same size, return original matrix
    if (width != m.width || height != m.height){
            return (*this);
    }
    Matrix2D<T> new_mat(width, height);
    for (int x = 0; x < width; ++x){
        for (int y = 0; y < height; ++y){
            new_mat.mat[x][y] = this->mat[x][y] - m.mat[x][y];
        }
    }
    return new_mat;
}

template<typename T>
Matrix2D<T> Matrix2D<T>::operator*(const T value)
{
    for (int x = 0; x < width; ++x){
        for (int y = 0; y < height; ++y){
            this->mat[x][y] = value*this->mat[x][y];
        }
    }

    return *this;
}

template<typename T>
bool Matrix2D<T>::Compare(const Matrix2D &m2)
{
    for (int x = 0; x < width; ++x){
        for (int y = 0; y < height; ++y){
            if ( this->mat[x][y] - m2.mat[x][y] > 1.f) {
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
uint Matrix2D<T>::GetWidth()
{
    return width;
}

template<typename T>
uint Matrix2D<T>::GetHeight()
{
    return height;
}

template<typename T>
T Matrix2D<T>::ValueAt(const int x, const int y)
{
    return mat[x][y];
}

template<typename T>
void Matrix2D<T>::SetCellValue(const int x, const int y, const T value)
{
    mat[x][y] = value;
}

template<typename T>
void Matrix2D<T>::Fill(const T value)
{
    for (int i = 0; i<mat.size(); ++i) {
      std::fill( mat[i].begin(), mat[i].end(), value);
    }
}

template<typename T>
QImage* Matrix2D<T>::ConvertToQImage()
{
    QImage *image = new QImage(this->width, this->height, QImage::Format_RGB32);

    for(int x = 0; x < image->width(); ++x){
        for(int y = 0; y < image->height(); ++y){

            QRgb color = 0xff000000;
            if(this->ValueAt(x, y) > 255.f){
                color = 0xffffffff;
            }else if(this->ValueAt(x, y) < 0.f){
                color = 0xff000000;
            }else{
                color = qRgb(static_cast<int>(this->ValueAt(x, y)), static_cast<int>(this->ValueAt(x, y)), static_cast<int>(this->ValueAt(x, y)));
            }

            image->setPixel(x, y, color);
        }
    }

    return image;
}

template<typename T>
void Matrix2D<T>::FilterMax(const int filter_size)
{
    int half_size = filter_size/2;
    Matrix2D<T> temp_matrix(width, height);

    for ( int x = 0; x < width; ++x ) {
        for ( int y = 0; y < height; ++y ) {
            T max_value = -1.f;
            for(int k = -half_size; k <= half_size; ++k){
                for(int l = -half_size; l <= half_size; ++l){
                    int col = x + k;
                    int row = y + l;

                    if(col < 0 || col >= width){
                        break;
                    }

                    if(row < 0 || row >= height){
                        continue;
                    }

                    if(mat[col][row] > max_value){
                        max_value = mat[col][row];
                    }
                }
            }

            temp_matrix.mat[x][y] = max_value;
        }
    }

    for(int x = 0; x < width; ++x){
        for(int y = 0; y < height; ++y){
            mat[x][y] = temp_matrix.mat[x][y];
        }
    }
}

template<typename T>
void Matrix2D<T>::FilterMin(const int filter_size)
{
    int half_size = filter_size/2;
    Matrix2D<T> temp_matrix(width, height);

    for(int x = 0; x < width; ++x){
        for(int y = 0; y < height; ++y){

            T min_value = 255.f;

            //Find minimum value inside the filter window
            for(int k = -half_size; k <= half_size; ++k){
                for(int l = -half_size; l <= half_size; ++l){

                    //Mirror pixels at the border of the image
                    int col = x + k;
                    int row = y + l;

                    if(col < 0 || col >= width){
                        break;
                    }

                    if(row < 0 || row >= height){
                        continue;
                    }

                    if(mat[col][row] < min_value){
                        min_value = mat[col][row];
                    }
                }
            }
            temp_matrix.mat[x][y] = min_value;
        }
    }

    for(int x = 0; x < width; ++x){
        for(int y = 0; y < height; ++y){
            mat[x][y] = temp_matrix.mat[x][y];
        }
    }
}

template<typename T>
void Matrix2D<T>::FilterMean(const int filter_size)
{
    //Mean Filter is separable, thus the convolution will be performed in two passes: row and column-wise
    int half_size = static_cast<int>(floor(filter_size/2));

    Matrix2D<T> temp_matrix(width, height);

#if 1
    //Row-wise convolution
    for(int x = 0; x < width; ++x ) {
        for(int y = 0; y < height; ++y){
            T new_value      = 0.f;
            int pixels_count = 0;
            for(int k = -half_size; k <= half_size; ++k){
                int col = x + k;

                //symmetric padding
                if (col < 0 ) {
                    col = -col;
                }
                if ( col >= width) {
                    col -= width;
                }

                if ( col >= 0 && col < width ) {
                    pixels_count++;
                    new_value += mat[col][y];
                }
            }
            temp_matrix.mat[x][y] = new_value/pixels_count;
        }
    }

    Matrix2D<T> temp_matrix2(width, height);
    //Column-wise convolution
    for(int x = 0; x < width; ++x ) {
        for(int y = 0; y < height; ++y){
            T new_value      = 0.f;
            int pixels_count = 0;
            for(int k = -half_size; k <= half_size; ++k){
                int row = y + k;

                //symmetric padding
                if (row < 0 ) {
                    row = -row;
                }
                if ( row >= height) {
                    row -= height;
                }

                if ( row >= 0 && row < height ) {
                    pixels_count++;
                    new_value += temp_matrix.mat[x][row];
                }
            }
            temp_matrix2.mat[x][y] = new_value/pixels_count;
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
    for(int x = 0; x < width; ++x){
        for(int y = 0; y < height; ++y){
            mat[x][y] = temp_matrix.mat[x][y];
        }
    }
}

template<typename T>
void Matrix2D<T>::ScaleToInterval(const T start, const T end)
{
    const T min = GetMinValue();
    const T max = GetMaxValue();

    for ( int x = 0; x < width; ++x ) {
        for ( int y = 0; y < height; ++y ) {
            mat[x][y] = start + (end - start) * ( mat[x][y] - min ) / ( max - min );
        }
    }
}

template<typename T>
T Matrix2D<T>::GetMinValue()
{
    T min = mat[0][0];
    for ( int x = 0; x < width; ++x ) {
        for ( int y = 0; y < height; ++y ) {
            if (mat[x][y] < min) {
                min = mat[x][y];
            }
        }
    }

    return min;
}

template<typename T>
T Matrix2D<T>::GetMaxValue()
{
    T max = mat[0][0];
    for ( int x = 0; x < width; ++x ) {
        for ( int y = 0; y < height; ++y ) {
            if (mat[x][y] > max) {
                max = mat[x][y];
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

        for( int x = 0; x < width; ++x ) {
            stream << "[ ";
            for( int y = 0; y < height; ++y ) {
                stream << QString::number(mat[x][y], 'f', 2) <<", ";
            }
            stream << "]\n";
        }
    }
    file.close();
}
