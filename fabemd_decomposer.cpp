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
    decompose_y();

}

void fabemd_decomposer::setInputImages(QVector<QImage *> *images)
{
    inputImages = images;
}

void fabemd_decomposer::set_resx(int value)
{
    ROWS = value;
}

void fabemd_decomposer::set_resy(int value)
{
    COLUMNS = value;
}

QImage *test_image;

QImage *fabemd_decomposer::getTestImage()
{
    return test_image;
}

void fabemd_decomposer::rgb_to_ycbcr()
{
    //parse input images, convert them to YCbCr and save each channel to the respective vector
    for(int k = 0; k < inputImages->length(); ++k){
        const int rows = static_cast<int>(inputImages->at(k)->width());
        const int cols = static_cast<int>(inputImages->at(k)->height());

        matrix<float> *y_channel  = new matrix<float>(static_cast<uint>(rows), static_cast<uint>(cols));
        matrix<float> *cb_channel = new matrix<float>(static_cast<uint>(rows), static_cast<uint>(cols));
        matrix<float> *cr_channel = new matrix<float>(static_cast<uint>(rows), static_cast<uint>(cols));

        //convert to ycbcr format
        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < cols; ++j){
                QColor rgb = inputImages->at(k)->pixel(i, j);
                float red   = static_cast<float>(rgb.red());
                float green = static_cast<float>(rgb.green());
                float blue  = static_cast<float>(rgb.blue());

                y_channel-> set_cell_value(static_cast<uint>(i), static_cast<uint>(j), 16.f +  65.738f*red/256.f + 129.057f*green/256.f +  25.064f*blue/256.f);
                cb_channel->set_cell_value(static_cast<uint>(i), static_cast<uint>(j), 128.f -  37.945f*red/256.f -  74.494f*green/256.f + 112.439f*blue/256.f);
                cr_channel->set_cell_value(static_cast<uint>(i), static_cast<uint>(j), 128.f + 112.439f*red/256.f -  94.154f*green/256.f -  18.285f*blue/256.f);
            }
        }

        //push channels to lists (vectors)
        y_channels.push_back(y_channel);
        cb_channels.push_back(cb_channel);
        cr_channels.push_back(cr_channel);
    }
}

void fabemd_decomposer::decompose_y()
{
    for(int k = 0; k < y_channels.length(); ++k){
        matrix<float> *local_maxima  = new matrix<float>(static_cast<uint>(ROWS), static_cast<uint>(COLUMNS));
        matrix<float> *local_minima  = new matrix<float>(static_cast<uint>(ROWS), static_cast<uint>(COLUMNS));
        matrix<float> *cur_y = y_channels[k];
        QVector<float> maxima_distances;
        //TODO function detect_local_maxima()
        for(int i = 1; i < ROWS - 1; ++i){
            for(int j = 1; j < COLUMNS - 1; ++j){
                if((cur_y->valueAt(i, j) > cur_y->valueAt(i - 1, j - 1)) &&
                   (cur_y->valueAt(i, j) > cur_y->valueAt(i - 1, j))     &&
                   (cur_y->valueAt(i, j) > cur_y->valueAt(i - 1, j + 1)) &&
                   (cur_y->valueAt(i, j) > cur_y->valueAt(i, j - 1))     &&
                   (cur_y->valueAt(i, j) > cur_y->valueAt(i, j + 1))     &&
                   (cur_y->valueAt(i, j) > cur_y->valueAt(i + 1, j - 1)) &&
                   (cur_y->valueAt(i, j) > cur_y->valueAt(i + 1, j))     &&
                   (cur_y->valueAt(i, j) > cur_y->valueAt(i + 1, j +1))){
                        local_maxima->set_cell_value( static_cast<uint>(i), static_cast<uint>(j),  cur_y->valueAt(i, j));
                        maxima_distances.push_back(0.f);
                 }

                //TODO if bla bla local_minima->set_cell_value
             }
        }

        test_image = local_maxima->matrix_to_image();
//        calculate_extrema_distances(local_maxima, &maxima_distances);
//        qDebug()<<"maxima found" << maxima_distances.length();
    }
}

void fabemd_decomposer::calculate_extrema_distances(matrix<float> *extrema, QVector<float> *extrema_distances)
{

    qDebug()<<"extrema count" << extrema_distances->length();

    for(int i = 1; i < ROWS - 1; ++i){
        for(int j = 1; j < COLUMNS - 1; ++j){

            if(extrema->valueAt(i, j) > 0){
                int win_half_size   = 1;
                float distance      = 0.f;

                while(distance == 0.f){
                    for(int k = -win_half_size; k < win_half_size; ++k){
                        for(int l = -win_half_size; l < win_half_size; ++l){
                            if(((i+l) >0) && ((j+k)>0) && ((i+l)<= ROWS) && ((j+l)<=COLUMNS)){
                                //TODO: calculate distances
                            }
                        }
                    }
                }
            }

        }
    }
}

