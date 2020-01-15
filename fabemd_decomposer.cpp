#include "fabemd_decomposer.h"
#include <QDebug>
#include <qmath.h>
#include <algorithm>
#include "matrix_2d.h"

fabemd_decomposer::fabemd_decomposer()
{

}

void fabemd_decomposer::FuseImages()
{
    //TODO free memory from previous execution

    RGBToYCbCr();
    DecomposeY();
    FuseIMFs(3);
    //TODO fuse Cb, Cr
}

void fabemd_decomposer::SetInputImages(QVector<QImage *> *images)
{
    inputImages = images;
}

void fabemd_decomposer::SetResX(int value)
{
    ROWS = value;
}

void fabemd_decomposer::SetResY(int value)
{
    COLUMNS = value;
}

QImage *fabemd_decomposer::GetFusedImage()
{
    return fused_image;
}

void fabemd_decomposer::RGBToYCbCr()
{
    //parse input images, convert them to YCbCr and save each channel to the respective vector
    for(int k = 0; k < inputImages->length(); ++k){
        const int rows = static_cast<int>(inputImages->at(k)->width());
        const int cols = static_cast<int>(inputImages->at(k)->height());

        Matrix2D<float> *y_channel  = new Matrix2D<float>(static_cast<uint>(rows), static_cast<uint>(cols));
        Matrix2D<float> *cb_channel = new Matrix2D<float>(static_cast<uint>(rows), static_cast<uint>(cols));
        Matrix2D<float> *cr_channel = new Matrix2D<float>(static_cast<uint>(rows), static_cast<uint>(cols));

        //convert to ycbcr format
        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < cols; ++j){
                QColor rgb = inputImages->at(k)->pixel(i, j);
                float red   = static_cast<float>(rgb.red());
                float green = static_cast<float>(rgb.green());
                float blue  = static_cast<float>(rgb.blue());

                y_channel-> set_cell_value(static_cast<uint>(i), static_cast<uint>(j), 16.f  +  65.738f*red/256.f + 129.057f*green/256.f +  25.064f*blue/256.f);
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

void fabemd_decomposer::DecomposeY()
{
    //window minimum size
    int upper_env_win_size = 2;
    int lower_env_win_size = 2;
    int win_size           = 2;

    QVector<float> maxima_distances;
    QVector<float> minima_distances;
    Matrix2D<float> local_maxima(static_cast<uint>(ROWS), static_cast<uint>(COLUMNS));
    Matrix2D<float> local_minima(static_cast<uint>(ROWS), static_cast<uint>(COLUMNS));

    //Init extrema count for the first iteration
    int extrema_count = 5000;
    imfs.resize(y_channels.length());
    const int extrema_threshold = 10;
    int imf_count = 0;


    //Consider 7 imfs enough for images decomposition
    while ( (extrema_count > extrema_threshold) && (imf_count < 150) ) {

        qDebug()<<"imfs"<<imfs.at(imfs.length() -1).length();
        int win_size_prev = win_size;

        //Step 1
        for ( int k = 0; k < y_channels.length(); ++k ) {

            maxima_distances.clear();
            minima_distances.clear();

            local_maxima.fill(-1.f);
            local_minima.fill(-1.f);
            Matrix2D<float> *cur_y = y_channels[k];

            //TODO create function detect_local_maxima()
            for ( int i = 1; i < ROWS - 1; ++i ) {
                for ( int j = 1; j < COLUMNS - 1; ++j ) {

                    if ( ( cur_y->valueAt(i, j) > cur_y->valueAt(i - 1, j - 1) ) &&
                         ( cur_y->valueAt(i, j) > cur_y->valueAt(i - 1, j)     ) &&
                         ( cur_y->valueAt(i, j) > cur_y->valueAt(i - 1, j + 1) ) &&
                         ( cur_y->valueAt(i, j) > cur_y->valueAt(i, j - 1)     ) &&
                         ( cur_y->valueAt(i, j) > cur_y->valueAt(i, j + 1)     ) &&
                         ( cur_y->valueAt(i, j) > cur_y->valueAt(i + 1, j - 1) ) &&
                         ( cur_y->valueAt(i, j) > cur_y->valueAt(i + 1, j)     ) &&
                         ( cur_y->valueAt(i, j) > cur_y->valueAt(i + 1, j +1)  )){
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

            extrema_count = std::min(maxima_distances.length(), minima_distances.length());

            int cur_win_size = GetExtremaDistance(&local_maxima, &maxima_distances);

            if(cur_win_size > upper_env_win_size){
                upper_env_win_size = cur_win_size;
            } else {
                upper_env_win_size = static_cast<int>(1.5*win_size_prev);
            }

            if(upper_env_win_size%2 == 0){
                upper_env_win_size++;
            }

            cur_win_size = GetExtremaDistance(&local_minima, &minima_distances);

            if(cur_win_size > lower_env_win_size){
                lower_env_win_size = cur_win_size;
            } else {
                lower_env_win_size = static_cast<int>(1.5*win_size_prev);
            }

            //window size is always odd number
            if(lower_env_win_size%2 == 0){
                lower_env_win_size++;
            }

        }//for k < image_count

        win_size = std::max(upper_env_win_size, lower_env_win_size);
        qDebug()<<"Extrema"<<extrema_count;

        for ( int k = 0; k < y_channels.count(); ++k ) {
            QString filename;
            Matrix2D<float> *upper_envelope = new Matrix2D<float>(*y_channels[k]);
            upper_envelope->filterMax(win_size);
            upper_envelope->filterMean(win_size);

            Matrix2D<float> *lower_envelope = new Matrix2D<float>(*y_channels[k]);
            lower_envelope->filterMin(win_size);
            lower_envelope->filterMean(win_size);

            Matrix2D<float> *imf = new Matrix2D<float>(*y_channels[k]);
            *imf = *y_channels[k] - (*upper_envelope + *lower_envelope)*0.5;
            imfs[k].push_back(imf);

            fused_image = imf->matrix_to_image();
            filename = "Images/results/imf_" + QString::number(imfs.at(0).length()) + "_" + QString::number(k) + ".png";
            qDebug()<<"saving imf"<<filename;
            fused_image->save(filename);

            //Update y-channel for the next iteration
            *y_channels[k] = *y_channels[k] - *imf;

            if ( k == y_channels.count() -1 ) {
                imf_count++;
            }
        }
    }//while(extrema_count > thresshold)

    //Add the residue to the imfs in order to process the imfs vector afterwards
    for ( int i = 0; i <  y_channels.count(); ++i ) {
        imfs[i].push_back(y_channels[i]);
    }
}

int fabemd_decomposer::GetExtremaDistance(Matrix2D<float> *extrema, QVector<float> *extrema_distances)
{
    float const max_distance = extrema_distances->at(0);
    int counter = -1;
    for(int i = 1; i < ROWS - 1; ++i){
        for(int j = 1; j < COLUMNS - 1; ++j){

            if(extrema->valueAt(i, j) >= 0){
                bool distance_found   = false;
                int win_half_size     = 2;
                float distance        = max_distance;
                counter++;

                while(distance_found == false){
                    //search on the edge of the reactangle
                    float distance_temp = max_distance;
                    //Top
                    int cur_row = i - win_half_size;
                    for ( int l = -win_half_size; l < win_half_size; ++l ) {
                        if ( (((cur_row) > 0) && ((j+l) > 0) && ((cur_row) < ROWS) && ((j+l) < COLUMNS)) && (extrema->valueAt(cur_row, j+l) >= 0) ) {
                            if(static_cast<float>(qSqrt(win_half_size*win_half_size + l*l)) < distance_temp){
                                distance_temp = static_cast<float>(qSqrt(win_half_size*win_half_size + l*l));
                            }
                        }
                    }
                    //Left
                    int cur_col = j - win_half_size;
                    for(int k = -win_half_size; k < win_half_size; ++k){
                        //If curr index is valid and the is a local extrema
                        if((((i+k) > 0) && ((cur_col) > 0) && ((i+k) < ROWS) && ((cur_col) < COLUMNS)) && (extrema->valueAt(i + k, cur_col) >= 0)){
                            //Update distance_temp if the new distance is shorter
                            if(static_cast<float>(qSqrt(k*k + win_half_size*win_half_size)) < distance_temp){
                                distance_temp = static_cast<float>(qSqrt(k*k + win_half_size*win_half_size));
                            }
                        }
                    }

                    //Bottom
                    cur_row = i + win_half_size;
                    for(int l = -win_half_size; l < win_half_size; ++l){
                        //If curr index is valid and the is a local extrema
                        if((((cur_row) > 0) && ((j+l) > 0) && ((cur_row) < ROWS) && ((j+l) < COLUMNS)) && (extrema->valueAt(cur_row, j + l) >= 0)){
                            //Update distance_temp if the new distance is shorter
                            if(static_cast<float>(qSqrt(win_half_size*win_half_size + l*l)) < distance_temp){
                                distance_temp = static_cast<float>(qSqrt(win_half_size*win_half_size + l*l));
                            }
                        }
                    }
                    //Right
                    cur_col = j + win_half_size;
                    for(int k = -win_half_size; k < win_half_size; ++k){
                        //If curr index is valid and the is a local extrema
                        if((((i+k) > 0) && ((cur_col) > 0) && ((i+k) < ROWS) && ((cur_col) < COLUMNS)) && (extrema->valueAt(i + k, cur_col) >= 0)){
                            //Update distance_temp if the new distance is shorter
                            if(static_cast<float>(qSqrt(k*k + win_half_size*win_half_size)) < distance_temp){
                                distance_temp = static_cast<float>(qSqrt(k*k + win_half_size*win_half_size));
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
    int min_val = static_cast<int>(*std::min_element(extrema_distances->constBegin(), extrema_distances->constEnd()));
    return min_val;
}

void fabemd_decomposer::FuseIMFs(int win_size)
{
    int half_win = static_cast<int>(0.5*win_size);

    qDebug()<<"Fuse imfs half win"<<half_win;
    const int images_count = imfs.length();
    const int imfs_depth   = imfs.at(0).length();

    QVector<float> weights;

    Matrix2D<float> *fused_y_channel = new Matrix2D<float>(static_cast<uint>(ROWS), static_cast<uint>(COLUMNS));

    for ( int i = 0; i < imfs_depth; ++i ) {
        Matrix2D<float> *fused_imf = new Matrix2D<float>(static_cast<uint>(ROWS), static_cast<uint>(COLUMNS));

        qDebug()<<"creating imf"<< i;

        for ( int x = 0; x < ROWS; ++x ) {
            for ( int y = 0; y < COLUMNS; ++y ) {
                float denom = 0.f;
                float nom   = 0.f;

                for ( int j = 0; j < images_count; ++j ) {
                    float local_energy = 0.f;
                    for (int k = -half_win; k < half_win; ++k ) {
                         for (int l = -half_win; l < half_win; ++l ) {
                            int row    = x + l;
                            int column = y + k;

                            if ( row < 0 ) {
                                row = -row;
                            } else if ( row >= ROWS ) {
                                row -= ROWS;
                            }

                            if ( column < 0 ) {
                                column = -column;
                            } else if ( column >= COLUMNS ) {
                                column -= COLUMNS;
                            }

                            local_energy += imfs.at(j).at(i)->valueAt(row, column) * imfs.at(j).at(i)->valueAt(row, column);
                        }
                    }
                    nom   += local_energy*imfs.at(j).at(i)->valueAt(x, y);
                    denom += local_energy;

                }//images_count

                // Avoid divisions with very small values, set pixel to 0
                if ( denom < 0.001f){
                    nom   = 0.f;
                    denom = 1.f;
                }
                fused_imf->set_cell_value(static_cast<uint>(x), static_cast<uint>(y), nom/denom);
            } //columns
        } //rows

        fused_image = fused_imf->matrix_to_image();
        QString filename = "Images/results/imf_fused_" + QString::number(i) + ".png";
        fused_image->save(filename);
        filename = "Images/results/imf_fused_" + QString::number(i) + ".txt";
        fused_imf->SaveToFile(filename);

        *fused_y_channel = *fused_y_channel + *fused_imf;

    } //imfs_depth

//    fused_y_channel->ScaleToInterval(16, 236);
    fused_image = fused_y_channel->matrix_to_image();
    fused_image->save("Images/results/output.png");
}
