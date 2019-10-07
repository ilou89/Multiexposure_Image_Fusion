#include "fabemd_decomposer.h"
#include <QDebug>

fabemd_decomposer::fabemd_decomposer()
{

}

void fabemd_decomposer::getIMFs()
{
    //Step 1: Clear memory from previous execution
    //TODO
    //Step 2: convert to YCbCr
    rgb_to_ycbcr();
    //Step2: TODO
}

void fabemd_decomposer::setInputImages(QVector<QImage *> *images)
{
    inputImages = images;
}

void fabemd_decomposer::rgb_to_ycbcr()
{
    //parse input images, convert them to YCbCr and save each channel to the respective vector
    for(int k = 0; k < inputImages->length(); ++k){
        const int RESX = static_cast<int>(inputImages->at(k)->width());
        const int RESY = static_cast<int>(inputImages->at(k)->height());
        //allocate memory
        //TODO create separate function that performs memory allocation
        float **y_channel;
        float **cb_channel;
        float **cr_channel;
        y_channel  = new float*[static_cast<uint>(RESX)];
        cb_channel = new float*[static_cast<uint>(RESX)];
        cr_channel = new float*[static_cast<uint>(RESX)];
        for(int i = 0; i < RESX; ++i){
            y_channel[i]  = new float[static_cast<uint>(RESY)];
            cb_channel[i] = new float[static_cast<uint>(RESY)];
            cr_channel[i] = new float[static_cast<uint>(RESY)];
        }

        //convert to ycbcr format
        for(int i = 0; i < RESX; ++i){
            for(int j = 0; j < RESY; ++j){
                QColor rgb = inputImages->at(k)->pixel(i, j);
                float red   = static_cast<float>(rgb.red());
                float green = static_cast<float>(rgb.green());
                float blue  = static_cast<float>(rgb.blue());

                y_channel[i][j]  =  16.f +  65.738f*red/256.f + 129.057f*green/256.f +  25.064f*blue/256.f;
                cb_channel[i][j] = 128.f -  37.945f*red/256.f -  74.494f*green/256.f + 112.439f*blue/256.f;
                cr_channel[i][j] = 128.f + 112.439f*red/256.f -  94.154f*green/256.f -  18.285f*blue/256.f;
            }
        }

        //push channels to lists (vectors)
        y_channels.push_back(&y_channel);
        cb_channels.push_back(&cb_channel);
        cr_channels.push_back(&cr_channel);
    }
}

