#ifndef MATRIX_H
#define MATRIX_H
#include <QVector>
#include <QtGlobal>

template <typename T> class matrix
{
public:
    //Constructor
    matrix(uint rows_, uint columns_);
    //Assignment operator '='
//    matrix(const matrix<T>& rhs);
    //Destructor
    virtual ~matrix();

    uint get_rows();
    uint get_columns();
    T    valueAt(int i, int j);
    void set_cell_value(uint i, uint j, T value);
    void fill(T value);
private:
    uint rows, columns;
    QVector<QVector<T> > mat;
};

//Include source file due to the template feature
#include "matrix.cpp"

#endif // MATRIX_H
