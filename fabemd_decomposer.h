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

    QVector<QVector<matrix<float> *>> imfs;

    void rgb_to_ycbcr();
    void decompose_y();
    int calculate_extrema_distances(matrix<float> *extrema, QVector<float> *extrema_distance);
    int ROWS, COLUMNS;

    void fuse_imfs(int win_size);

public:
    fabemd_decomposer();

    void getIMFs();
    void setInputImages(QVector<QImage *> *images);
    void SetResX(int value);
    void SetResY(int value);

    QImage *GetTestImage();
};

#endif // FABEMD_DECOMPOSER_H
