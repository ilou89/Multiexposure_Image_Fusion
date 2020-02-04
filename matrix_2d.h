#ifndef MATRIX_H
#define MATRIX_H
#include <vector>
#include <QImage>
#include <memory>

template <typename T> class Matrix2D
{
public:
    Matrix2D(const uint width_, const uint height_);
    //Rule of three (destructor - copy costructor - copy assignment operator)
    virtual ~Matrix2D();
    Matrix2D(const Matrix2D &p2)          = default;
    Matrix2D& operator=(const Matrix2D &) = default;

    Matrix2D operator+(const Matrix2D&);
    Matrix2D operator-(const Matrix2D&);
    Matrix2D operator*(const T value);

    std::unique_ptr<QImage> ConvertToQImage();
    uint    GetWidth();
    uint    GetHeight();
    T       GetMinValue();
    T       GetMaxValue();
    T       ValueAt(const int x, const int y);
    void    SaveToFile(const QString filename);
    void    SetCellValue(const int x, const int y, const T value);
    void    Fill(const T value);
    void    ScaleToInterval(const T start, const T end);
    bool    Compare(const Matrix2D&m2);

    //To be reimplemented in derived classes usisng parallel programming techniques
    virtual void FilterMax (const int filter_size);
    virtual void FilterMin (const int filter_size);
    virtual void FilterMean(const int filter_size);

protected:
    int width, height;
    std::vector< std::vector<T> > mat;
};

//Include source file due to the template feature
#include "matrix_2d.cpp"

#endif // MATRIX_H
