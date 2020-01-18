#ifndef MATRIX_H
#define MATRIX_H
#include <QVector>
#include <QImage>

template <typename T> class Matrix2D
{
public:
    Matrix2D(uint rows_, uint columns_);
    Matrix2D(const Matrix2D &p2);

    //Implement operators as friends
    Matrix2D operator+(const Matrix2D&);
    Matrix2D operator-(const Matrix2D&);
    Matrix2D operator*(const T value);

    virtual ~Matrix2D();

    QImage* ConvertToQImage();
    uint    GetRows();
    uint    GetColumns();
    T       GetMinValue();
    T       GetMaxValue();
    T       ValueAt(const int i, const int j);
    void    SaveToFile(const QString filename);
    void    SetCellValue(const int i, const int j, const T value);
    void    Fill(const T value);
    void    ScaleToInterval(const T start, const T end);
    bool    Compare(const Matrix2D&m2);

    //To be reimplemented in derived classes usisng parallel programming techniques
    virtual void FilterMax (const int filter_size);
    virtual void FilterMin (const int filter_size);
    virtual void FilterMean(const int filter_size);

protected:
    int rows, columns;
    QVector<QVector<T> > mat;
};

//Include source file due to the template feature
#include "matrix_2d.cpp"

#endif // MATRIX_H
