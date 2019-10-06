#include "fabemd_decomposer.h"
#include <QDebug>

fabemd_decomposer::fabemd_decomposer()
{

}

void fabemd_decomposer::getIMFs()
{
    //Step 1: convert to YCbCr
    rgb_to_ycbcr();
    //Step2: TODO
}

void fabemd_decomposer::setInputImages(QVector<QImage *> *images)
{
    inputImages = images;
}

void fabemd_decomposer::rgb_to_ycbcr()
{
    //Parse input images, convert them to YCbCr and save each channel to the respective vector
    for(int k = 0; k < inputImages->length(); ++k){
        QVector<QVector<float>> *y_channel = new QVector<QVector<float> >(inputImages->at(k)->width(), QVector<float>(inputImages->at(k)->height()));
        qDebug()<<y_channel->length()<<y_channel->at(0).length()<<sizeof (y_channel);

        for(int i = 0; i < y_channel->length(); ++i){
            for(int j = 0; j < y_channel->at(i).length(); ++j){
//                *y_channel[i][j] = bla bla;
            }
        }

    }
}

