#include "fabemd_decomposer.h"
#include <QDebug>
#include <qmath.h>
#include <algorithm>

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
    //window minimum size
    int upper_env_win_size = 3;
    int lower_env_win_size = 3;

    QVector<float> maxima_distances;
    QVector<float> minima_distances;
    matrix<float> local_maxima(static_cast<uint>(ROWS), static_cast<uint>(COLUMNS));
    matrix<float> local_minima(static_cast<uint>(ROWS), static_cast<uint>(COLUMNS));

    for(int k = 0; k < y_channels.length(); ++k){
        maxima_distances.clear();
        minima_distances.clear();

        local_maxima.fill(0.f);
        local_minima.fill(0.f);
        matrix<float> *cur_y = y_channels[k];

        //TODO create function detect_local_maxima()
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
                        local_maxima.set_cell_value( static_cast<uint>(i), static_cast<uint>(j),  cur_y->valueAt(i, j));
                        //Fill the maxima distances with the biggest possible distance
                        maxima_distances.push_back(static_cast<float>(qSqrt(ROWS*ROWS + COLUMNS*COLUMNS)));
                 }

                if((cur_y->valueAt(i, j) < cur_y->valueAt(i - 1, j - 1)) &&
                   (cur_y->valueAt(i, j) < cur_y->valueAt(i - 1, j))     &&
                   (cur_y->valueAt(i, j) < cur_y->valueAt(i - 1, j + 1)) &&
                   (cur_y->valueAt(i, j) < cur_y->valueAt(i, j - 1))     &&
                   (cur_y->valueAt(i, j) < cur_y->valueAt(i, j + 1))     &&
                   (cur_y->valueAt(i, j) < cur_y->valueAt(i + 1, j - 1)) &&
                   (cur_y->valueAt(i, j) < cur_y->valueAt(i + 1, j))     &&
                   (cur_y->valueAt(i, j) < cur_y->valueAt(i + 1, j +1))){
                        local_minima.set_cell_value( static_cast<uint>(i), static_cast<uint>(j),  cur_y->valueAt(i, j));
                        //Fill the maxima distances with the biggest possible distance
                        minima_distances.push_back(static_cast<float>(qSqrt(ROWS*ROWS + COLUMNS*COLUMNS)));
                 }
             }
        }

        int cur_win_size = calculate_extrema_distances(&local_maxima, &maxima_distances);

        if(cur_win_size > upper_env_win_size){
            upper_env_win_size = cur_win_size;

            //window size is always odd number
            if(upper_env_win_size%2 == 0){
                upper_env_win_size--;
            }
        }

        cur_win_size = calculate_extrema_distances(&local_minima, &minima_distances);

        if(cur_win_size > lower_env_win_size){
            lower_env_win_size = cur_win_size;

            //window size is always odd number
            if(lower_env_win_size%2 == 0){
                lower_env_win_size--;
            }
        }
    }//for k < image_count

    qDebug()<<"step 2" << lower_env_win_size;
    for(int k = 0; k < y_channels.count(); ++k){
        if(k == 0){
            matrix<float> *upper_envelope = new matrix<float>(*y_channels[k]);
            upper_envelope->filterMax(upper_env_win_size);  //too slow
            upper_envelope->filterMean(upper_env_win_size); //too slow
            qDebug()<<"upper envelope calculated";

            matrix<float> *lower_envelope = new matrix<float>(*y_channels[k]);
            lower_envelope->filterMin(lower_env_win_size);
            lower_envelope->filterMean(lower_env_win_size);
            qDebug()<<"lower envelope calculated";

//            test_image = lower_envelope->matrix_to_image();
            matrix<float> *imf = new matrix<float>(*y_channels[k]);
            *imf = *y_channels[k] - (*upper_envelope - *lower_envelope)*0.5;
            test_image = imf->matrix_to_image();


            for(int x=0;x<ROWS;++x){
                for(int y=0;y<COLUMNS;++y){
                    qDebug()<<x<<y<<imf->valueAt(x,y)<<y_channels[k]->valueAt(x,y)<<upper_envelope->valueAt(x,y)<<lower_envelope->valueAt(x,y);
                }
            }

        }
    }
}

int fabemd_decomposer::calculate_extrema_distances(matrix<float> *extrema, QVector<float> *extrema_distances)
{
    float const max_distance = extrema_distances->at(0);
    int counter = -1;
    for(int i = 1; i < ROWS - 1; ++i){
        for(int j = 1; j < COLUMNS - 1; ++j){

            if(extrema->valueAt(i, j) > 0){
                bool distance_found   = false;
                int win_half_size     = 2;
                float distance        = max_distance;
                counter++;

                while(distance_found == false){
                    //search on the edge of the reactangle
                    float distance_temp = max_distance;
                    //Top
                    int cur_row = i - win_half_size;
                    for(int l = -win_half_size; l < win_half_size; ++l){
                        //If curr index is valid and the is a local extrema
                        if((((cur_row) > 0) && ((j+l) > 0) && ((cur_row) < ROWS) && ((j+l) < COLUMNS)) && (extrema->valueAt(cur_row, j + l) > 0)){
                            //Update distance_temp if the new distance is shorter
                            if(static_cast<float>(qSqrt(cur_row*cur_row + l*l)) < distance_temp){
                                distance_temp = static_cast<float>(qSqrt((i - cur_row)*(i - cur_row) + l*l));
                            }
                        }
                    }
                    //Right
                    int cur_col = j - win_half_size;
                    for(int k = -win_half_size; k < win_half_size; ++k){
                        //If curr index is valid and the is a local extrema
                        if((((i+k) > 0) && ((cur_col) > 0) && ((i+k) < ROWS) && ((cur_col) < COLUMNS)) && (extrema->valueAt(i + k, cur_col) > 0)){
                            //Update distance_temp if the new distance is shorter
                            if(static_cast<float>(qSqrt(k*k + cur_col*cur_col)) < distance_temp){
                                distance_temp = static_cast<float>(qSqrt(k*k + (j - cur_col)*(j - cur_col)));
                            }
                        }
                    }

                    //Bottom
                    cur_row = i + win_half_size;
                    for(int l = -win_half_size; l < win_half_size; ++l){
                        //If curr index is valid and the is a local extrema
                        if((((cur_row) > 0) && ((j+l) > 0) && ((cur_row) < ROWS) && ((j+l) < COLUMNS)) && (extrema->valueAt(cur_row, j + l) > 0)){
                            //Update distance_temp if the new distance is shorter
                            if(static_cast<float>(qSqrt(cur_row*cur_row + l*l)) < distance_temp){
                                distance_temp = static_cast<float>(qSqrt((i - cur_row)*(i - cur_row) + l*l));
                            }
                        }
                    }
                    //Left
                    cur_col = j + win_half_size;
                    for(int k = -win_half_size; k < win_half_size; ++k){
                        //If curr index is valid and the is a local extrema
                        if((((i+k) > 0) && ((cur_col) > 0) && ((i+k) < ROWS) && ((cur_col) < COLUMNS)) && (extrema->valueAt(i + k, cur_col) > 0)){
                            //Update distance_temp if the new distance is shorter
                            if(static_cast<float>(qSqrt(k*k + cur_col*cur_col)) < distance_temp){
                                distance_temp = static_cast<float>(qSqrt(k*k + (j - cur_col)*(j - cur_col)));
                            }
                        }
                    }

                    if(distance_temp < distance){
                        distance = distance_temp;
                        distance_found = true;
                        extrema_distances->replace(counter, distance_temp);
                    }

                    if(distance_found == false){
                        win_half_size +=1;
                    }
                }
            }
        }
    }
    int min_val = static_cast<int>(*std::max_element(extrema_distances->constBegin(), extrema_distances->constEnd()));
    return min_val;
}

