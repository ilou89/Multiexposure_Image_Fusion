#ifndef FABEMD_DECOMPOSER_H
#define FABEMD_DECOMPOSER_H

#include <QImage>

class fabemd_decomposer
{
private:
    QVector<QImage*> *inputImages;
    //2D arrays of size (width x height)
    QVector<QVector<QVector<float>>> y_channels;
    QVector<QVector<QVector<float>>> cb_channels;
    QVector<QVector<QVector<float>>> cr_channels;


    void rgb_to_ycbcr();
public:
    fabemd_decomposer();

    void getIMFs();
    void setInputImages(QVector<QImage *> *images);
};

#endif // FABEMD_DECOMPOSER_H
