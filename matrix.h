#ifndef MATRIX_H
#define MATRIX_H
#include <QVector>
#include <QImage>
#include <QtGlobal>
#include <qdebug.h>
template <typename T> class matrix
{
public:
    //Constructor
    matrix(uint rows_, uint columns_);
    //Copy constructor
    matrix(const matrix &p2);
    //TODO copy assignment
    //operator overloads
//    matrix operator=(const matrix&);
    matrix operator-(const matrix&);
    matrix operator*(const T value);
//    matrix operator*(const matrix&);

    //Destructor
    virtual ~matrix();
    uint get_rows();
    uint get_columns();
    T    valueAt(int i, int j);
    void set_cell_value(uint i, uint j, T value);
    void fill(T value);
    QImage* matrix_to_image();
    void filterMax(int filter_size);
    void filterMin(int filter_size);
    void filterMean(int filter_size);

private:
    uint rows, columns;
    QVector<QVector<T> > mat;
};

//Include source file due to the template feature
#include "matrix.cpp"

#endif // MATRIX_H
