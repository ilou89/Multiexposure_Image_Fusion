#include "matrix_2d.h"
#include "QFile"
#include <math.h>

template<typename T>
Matrix2D<T>::Matrix2D(uint rows_, uint columns_)
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
Matrix2D<T>::Matrix2D(const Matrix2D &p2)
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
Matrix2D<T> Matrix2D<T>::operator+(const Matrix2D &m)
{
    //  if matrices do not have the same size, return original matrix
    if (rows != m.rows || columns != m.columns){
            return (*this);
    }
    Matrix2D<T> new_mat(rows, columns);
    for (int i = 0; i < rows; ++i){
        for (int j = 0; j < columns; ++j){
            new_mat.mat[i][j] = this->mat[i][j] + m.mat[i][j];
        }
    }
    return new_mat;
}

template<typename T>
Matrix2D<T> Matrix2D<T>::operator-(const Matrix2D &m)
{
    //  if matrices do not have the same size, return original matrix
    if (rows != m.rows || columns != m.columns){
            return (*this);
    }
    Matrix2D<T> new_mat(rows, columns);
    for (int i = 0; i < rows; ++i){
        for (int j = 0; j < columns; ++j){
            new_mat.mat[i][j] = this->mat[i][j] - m.mat[i][j];
        }
    }
    return new_mat;
}

template<typename T>
Matrix2D<T> Matrix2D<T>::operator*(const T value)
{
    for (int i = 0; i < rows; ++i){
        for (int j = 0; j < columns; ++j){
            this->mat[i][j] = value*this->mat[i][j];
        }
    }

    return *this;
}

template<typename T>
bool Matrix2D<T>::compare(const Matrix2D &m2)
{
    qDebug()<<"compare matrices";
    for (int i = 0; i < rows; ++i){
        for (int j = 0; j < columns; ++j){
            if ( this->mat[i][j] - m2.mat[i][j] > 1.f) {
                qDebug()<< this->mat[i][j] << " vs " << m2.mat[i][j];
//                return false;
            }
        }
    }
    return true;
}

//template<typename T>
//bool Matrix2D<T>::operator==(const Matrix2D&m)
//{
//    qDebug()<<"compare matrices";
//    for (int i = 0; i < rows; ++i){
//        for (int j = 0; j < columns; ++j){
//            if ( this->mat[i][j] != m.mat[i][j] ) {
//                qDebug()<< this->mat[i][j] << " vs " << m.mat[i][j];
//                return false;
//            }
//        }
//    }
//    return true;
//}

template<typename T>
Matrix2D<T>::~Matrix2D()
{
    // We do not use dynamic memory allocation (new),
    // thus destruction will do nothing here. Make it virtual
    // so that future subclasses can perform their own destruction
}

template<typename T>
uint Matrix2D<T>::get_rows()
{
    return rows;
}

template<typename T>
uint Matrix2D<T>::get_columns()
{
    return columns;
}

template<typename T>
T Matrix2D<T>::valueAt(int i, int j)
{
    return mat[i][j];
}

template<typename T>
void Matrix2D<T>::set_cell_value(uint i, uint j, T value)
{
    mat[i][j] = value;
}

template<typename T>
void Matrix2D<T>::fill(T value)
{
    for (int i=0; i<mat.size(); i++) {
      std::fill(mat[i].begin(), mat[i].end(), value);
    }
}

template<typename T>
QImage* Matrix2D<T>::matrix_to_image()
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
void Matrix2D<T>::filterMax(int filter_size)
{
    int half_size = filter_size/2;
    Matrix2D<float> temp_matrix(rows, columns);

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
void Matrix2D<T>::filterMin(int filter_size)
{
    int half_size = filter_size/2;
//    qDebug()<<"half size"<<half_size;
    Matrix2D<float> temp_matrix(rows, columns);

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
void Matrix2D<T>::filterMean(int filter_size)
{
    //Mean Filter is separable, thus the convolution will be performed in two passes: row and column-wise
    int half_size = static_cast<int>(floor(filter_size/2));

    Matrix2D<float> temp_matrix(rows, columns);

#if 1
    //Row-wise convolution
    for(int i = 0; i < rows; ++i ) {
        for(int j = 0; j < columns; ++j){
            float new_value  = 0.f;
            int pixels_count = 0;
            for(int k = -half_size; k <= half_size; ++k){
                int row = i + k;

                //symmetric padding
                if (row < 0 ) {
                    row = -row;
                }
                if ( row >= rows) {
                    row -= rows;
                }

                if ( row >= 0 && row < rows ) {
                    pixels_count++;
                    new_value += mat[row][j];
                } else {
                    qDebug()<<"EEEEEEErroooor";
                }
            }
            temp_matrix.mat[i][j] = new_value/pixels_count;
        }
    }

    Matrix2D<float> temp_matrix2(rows, columns);
    //Column-wise convolution
    for(int i = 0; i < rows; ++i ) {
        for(int j = 0; j < columns; ++j){
            float new_value  = 0.f;
            int pixels_count = 0;
            for(int k = -half_size; k <= half_size; ++k){
                int column = j + k;

                //symmetric padding
                if (column < 0 ) {
                    column = -column;
                }
                if ( column >= columns) {
                    column -= column;
                }

                if ( column >= 0 && column < columns ) {
                    pixels_count++;
                    new_value += temp_matrix.mat[i][column];
                } else {
                    qDebug()<<"EEEEEEErroooor";
                }
            }
            temp_matrix2.mat[i][j] = new_value/pixels_count;
        }
    }

#else

//Naive convolution
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
    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < columns; ++j){
            mat[i][j] = temp_matrix.mat[i][j];
        }
    }
}

template<typename T>
void Matrix2D<T>::ScaleToInterval(T start, T end)
{
    const T min = GetMinValue();
    const T max = GetMaxValue();

    for ( int i = 0; i < rows; ++i ) {
        for ( int j = 0; j < columns; ++j ) {
            mat[i][j] = start + (end - start) * ( mat[i][j] - min ) / ( max - min );
        }
    }
}

template<typename T>
T Matrix2D<T>::GetMinValue()
{
    T min = mat[0][0];
    for ( int i = 0; i < rows; ++i ) {
        for ( int j = 0; j < columns; ++j ) {
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
    for ( int i = 0; i < rows; ++i ) {
        for ( int j = 0; j < columns; ++j ) {
            if (mat[i][j] > max) {
                max = mat[i][j];
            }
        }
    }

    return max;
}

template<typename T>
void Matrix2D<T>::SaveToFile(QString filename)
{
    qDebug()<<"saving to"<<filename;

    QFile file( filename );
    if ( file.open(QIODevice::ReadWrite) )
    {
        QTextStream stream( &file );

        for( int i = 0; i < rows; ++i ) {
            stream << "[ ";
            for( int j = 0; j < columns; ++j ) {
                stream << QString::number(mat[i][j], 'f', 2) <<", ";
            }
            stream << "]\n";
        }
    }
    file.close();
}
