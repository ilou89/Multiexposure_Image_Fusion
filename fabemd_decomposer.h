#ifndef FABEMD_DECOMPOSER_H
#define FABEMD_DECOMPOSER_H

#include <QImage>
#include "matrix.h"

class fabemd_decomposer
{
private:
    QVector<QImage*> *inputImages;

    QVector<matrix<float> *> y_channels;
    QVector<matrix<float> *> cb_channels;
    QVector<matrix<float> *> cr_channels;

    void rgb_to_ycbcr();
    void decompose_y();
    int ROWS, COLUMNS;

public:
    fabemd_decomposer();

    void getIMFs();
    void setInputImages(QVector<QImage *> *images);
    void set_resx(int value);
    void set_resy(int value);
};

#endif // FABEMD_DECOMPOSER_H
