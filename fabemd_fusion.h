#ifndef FABEMD_DECOMPOSER_H
#define FABEMD_DECOMPOSER_H

#include <QImage>
#include "matrix_2d.h"

class FabemdFusion
{
public:
    FabemdFusion();

    void FuseImages(const int width_, const int height_);
    void SetInputImages(QVector<QImage *> *images);
    std::unique_ptr<QImage> GetFusedImage();

private:
    QVector<QImage*> *inputImages;

    QVector<Matrix2D<float> *> y_channels;
    QVector<Matrix2D<float> *> cb_channels;
    QVector<Matrix2D<float> *> cr_channels;
    QVector<QVector<Matrix2D<float> *>> imfs;

    void RGBToYCbCr();
    void YCbCrToRGB();
    void DecomposeY();
    int GetExtremaDistance(Matrix2D<float> *extrema, QVector<float> *extrema_distance);
    int WIDTH, HEIGHT;
    bool scale_y;
    void FuseIMFs(const int win_size);
    void FuseCbCr();
    void RestoreYChannels();
    std::unique_ptr<QImage> fused_image;

    Matrix2D<float> *fused_y;
    Matrix2D<float> *fused_cb;
    Matrix2D<float> *fused_cr;

};

#endif // FABEMD_DECOMPOSER_H
