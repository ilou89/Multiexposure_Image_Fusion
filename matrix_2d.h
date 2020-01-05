#ifndef MATRIX_H
#define MATRIX_H
#include <QVector>
#include <QImage>
#include <QtGlobal>
#include <qdebug.h>
template <typename T> class Matrix2D
{
public:
    Matrix2D(uint rows_, uint columns_);
    Matrix2D(const Matrix2D &p2);

    //TODO make them friends in order to avoid operand strict order
    Matrix2D operator+(const Matrix2D&);
    Matrix2D operator-(const Matrix2D&);
    Matrix2D operator*(const T value);

    virtual ~Matrix2D();
    uint get_rows();
    uint get_columns();
    T    valueAt(int i, int j);
    void set_cell_value(uint i, uint j, T value);
    void fill(T value);
    QImage* matrix_to_image();
    void filterMax(int filter_size);
    void filterMin(int filter_size);
    void filterMean(int filter_size);
    void ScaleToInterval(T start, T end);

    T GetMinValue();
    T GetMaxValue();
    void SaveToFile(QString filename);

private:
    uint rows, columns;
    QVector<QVector<T> > mat;
};

//Include source file due to the template restrictions
#include "matrix_2d.cpp"

#endif // MATRIX_H
