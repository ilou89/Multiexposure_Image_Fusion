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

    QVector<matrix<float> *> imfs;
//    QVector<matrix<float> *> maxima;
//    QVector<matrix<float> *> minima;

//    QVector<QVector<float>> maxima_distances;

    void rgb_to_ycbcr();
    void decompose_y();
    int calculate_extrema_distances(matrix<float> *extrema, QVector<float> *extrema_distance);
    int ROWS, COLUMNS;

public:
    fabemd_decomposer();

    void getIMFs();
    void setInputImages(QVector<QImage *> *images);
    void set_resx(int value);
    void set_resy(int value);

    QImage *getTestImage();
};

#endif // FABEMD_DECOMPOSER_H
