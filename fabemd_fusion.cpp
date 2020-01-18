#include "fabemd_fusion.h"
#include <QDebug>
#include <qmath.h>
#include <algorithm>
#include <chrono>
#include "matrix_2d.h"

FabemdFusion::FabemdFusion()
{

}

void FabemdFusion::FuseImages(int rows_, int columns_)
{
    ROWS    = rows_;
    COLUMNS = columns_;

    y_channels.clear();
    cb_channels.clear();
    cr_channels.clear();
    imfs.clear();

    RGBToYCbCr();
    DecomposeY();
    FuseIMFs(3);
    //TODO fuse Cb, Cr
}

void FabemdFusion::SetInputImages(QVector<QImage *> *images)
{
    inputImages = images;
}

QImage *FabemdFusion::GetFusedImage()
{
    return fused_image;
}

void FabemdFusion::RGBToYCbCr()
{
    //parse input images, convert them to YCbCr and save each channel to the respective vector
    for(int k = 0; k < inputImages->length(); ++k){
        Matrix2D<float> *y_channel  = new Matrix2D<float>(static_cast<uint>(ROWS), static_cast<uint>(COLUMNS));
        Matrix2D<float> *cb_channel = new Matrix2D<float>(static_cast<uint>(ROWS), static_cast<uint>(COLUMNS));
        Matrix2D<float> *cr_channel = new Matrix2D<float>(static_cast<uint>(ROWS), static_cast<uint>(COLUMNS));

        //convert to ycbcr format
        for(int i = 0; i < ROWS; ++i){
            for(int j = 0; j < COLUMNS; ++j){
                QColor rgb  = inputImages->at(k)->pixel(i, j);
                float red   = static_cast<float>(rgb.red());
                float green = static_cast<float>(rgb.green());
                float blue  = static_cast<float>(rgb.blue());

                y_channel-> SetCellValue(static_cast<uint>(i), static_cast<uint>(j), 16.f  +  65.738f*red/256.f + 129.057f*green/256.f +  25.064f*blue/256.f);
                cb_channel->SetCellValue(static_cast<uint>(i), static_cast<uint>(j), 128.f -  37.945f*red/256.f -  74.494f*green/256.f + 112.439f*blue/256.f);
                cr_channel->SetCellValue(static_cast<uint>(i), static_cast<uint>(j), 128.f + 112.439f*red/256.f -  94.154f*green/256.f -  18.285f*blue/256.f);
            }
        }

        //push channels to lists (vectors)
        y_channels.push_back(y_channel);
        cb_channels.push_back(cb_channel);
        cr_channels.push_back(cr_channel);
    }
}

void FabemdFusion::DecomposeY()
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

    //Control the decomposition granularity. Trade-off between performance for image quality
    int imf_count = 0;

    while ( (extrema_count > extrema_threshold) && (imf_count < 6) ) {

        int win_size_prev = win_size;

        //Step 1
        for ( int k = 0; k < y_channels.length(); ++k ) {

            maxima_distances.clear();
            minima_distances.clear();

            local_maxima.Fill(-1.f);
            local_minima.Fill(-1.f);
            Matrix2D<float> *cur_y = y_channels[k];

            //TODO create function detect_local_maxima()
            for ( int i = 1; i < ROWS - 1; ++i ) {
                for ( int j = 1; j < COLUMNS - 1; ++j ) {

                    if ( ( cur_y->ValueAt(i, j) > cur_y->ValueAt(i - 1, j - 1) ) &&
                         ( cur_y->ValueAt(i, j) > cur_y->ValueAt(i - 1, j)     ) &&
                         ( cur_y->ValueAt(i, j) > cur_y->ValueAt(i - 1, j + 1) ) &&
                         ( cur_y->ValueAt(i, j) > cur_y->ValueAt(i, j - 1)     ) &&
                         ( cur_y->ValueAt(i, j) > cur_y->ValueAt(i, j + 1)     ) &&
                         ( cur_y->ValueAt(i, j) > cur_y->ValueAt(i + 1, j - 1) ) &&
                         ( cur_y->ValueAt(i, j) > cur_y->ValueAt(i + 1, j)     ) &&
                         ( cur_y->ValueAt(i, j) > cur_y->ValueAt(i + 1, j +1)  )){
                            local_maxima.SetCellValue( static_cast<uint>(i), static_cast<uint>(j),  cur_y->ValueAt(i, j));
                            //Fill the maxima distances with the biggest possible distance
                            maxima_distances.push_back(static_cast<float>(qSqrt(ROWS*ROWS + COLUMNS*COLUMNS)));
                     }

                    if((cur_y->ValueAt(i, j) < cur_y->ValueAt(i - 1, j - 1)) &&
                       (cur_y->ValueAt(i, j) < cur_y->ValueAt(i - 1, j))     &&
                       (cur_y->ValueAt(i, j) < cur_y->ValueAt(i - 1, j + 1)) &&
                       (cur_y->ValueAt(i, j) < cur_y->ValueAt(i, j - 1))     &&
                       (cur_y->ValueAt(i, j) < cur_y->ValueAt(i, j + 1))     &&
                       (cur_y->ValueAt(i, j) < cur_y->ValueAt(i + 1, j - 1)) &&
                       (cur_y->ValueAt(i, j) < cur_y->ValueAt(i + 1, j))     &&
                       (cur_y->ValueAt(i, j) < cur_y->ValueAt(i + 1, j +1))){
                            local_minima.SetCellValue( static_cast<uint>(i), static_cast<uint>(j),  cur_y->ValueAt(i, j));
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

        for ( int k = 0; k < y_channels.count(); ++k ) {
            QString filename;
            Matrix2D<float> *upper_envelope = new Matrix2D<float>(*y_channels[k]);
            auto start = std::chrono::steady_clock::now();

            upper_envelope->FilterMax(win_size);
            auto end = std::chrono::steady_clock::now();
            std::cout << "MAX Filter: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<< " ms ||" << " Filter size "<< win_size<<"   "<<std::endl<< std::flush;
            start = std::chrono::steady_clock::now();
            upper_envelope->FilterMean(win_size);
            end = std::chrono::steady_clock::now();
            std::cout << "MEAN Filter: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<< " ms ||" << " Filter size "<< win_size<<"   "<<std::endl<< std::flush;


            Matrix2D<float> *lower_envelope = new Matrix2D<float>(*y_channels[k]);
            start = std::chrono::steady_clock::now();
            lower_envelope->FilterMin(win_size);
            end = std::chrono::steady_clock::now();
            std::cout << "MIN Filter: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<< " ms ||" << " Filter size "<< win_size<<"   "<<std::endl<< std::flush;
            start = std::chrono::steady_clock::now();
            lower_envelope->FilterMean(win_size);
            end = std::chrono::steady_clock::now();
            std::cout << "MEAN Filter: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<< " ms ||" << " Filter size "<< win_size<<"   "<<std::endl<< std::flush;


            Matrix2D<float> *imf = new Matrix2D<float>(*y_channels[k]);
            *imf = *y_channels[k] - (*upper_envelope + *lower_envelope)*0.5;
            imfs[k].push_back(imf);

            fused_image = imf->ConvertToQImage();
            filename = "Images/results/imf_" + QString::number(imfs.at(0).length()) + "_" + QString::number(k) + ".png";
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

int FabemdFusion::GetExtremaDistance(Matrix2D<float> *extrema, QVector<float> *extrema_distances)
{
    float const max_distance = extrema_distances->at(0);
    int counter = -1;
    for(int i = 1; i < ROWS - 1; ++i){
        for(int j = 1; j < COLUMNS - 1; ++j){

            if(extrema->ValueAt(i, j) >= 0){
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
                        if ( (((cur_row) > 0) && ((j+l) > 0) && ((cur_row) < ROWS) && ((j+l) < COLUMNS)) && (extrema->ValueAt(cur_row, j+l) >= 0) ) {
                            if(static_cast<float>(qSqrt(win_half_size*win_half_size + l*l)) < distance_temp){
                                distance_temp = static_cast<float>(qSqrt(win_half_size*win_half_size + l*l));
                            }
                        }
                    }
                    //Left
                    int cur_col = j - win_half_size;
                    for(int k = -win_half_size; k < win_half_size; ++k){
                        //If curr index is valid and the is a local extrema
                        if((((i+k) > 0) && ((cur_col) > 0) && ((i+k) < ROWS) && ((cur_col) < COLUMNS)) && (extrema->ValueAt(i + k, cur_col) >= 0)){
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
                        if((((cur_row) > 0) && ((j+l) > 0) && ((cur_row) < ROWS) && ((j+l) < COLUMNS)) && (extrema->ValueAt(cur_row, j + l) >= 0)){
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
                        if((((i+k) > 0) && ((cur_col) > 0) && ((i+k) < ROWS) && ((cur_col) < COLUMNS)) && (extrema->ValueAt(i + k, cur_col) >= 0)){
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

void FabemdFusion::FuseIMFs(int win_size)
{
    int half_win = static_cast<int>(0.5*win_size);

    qDebug()<<"Fuse IMFs";
    const int images_count = imfs.length();
    const int imfs_depth   = imfs.at(0).length();

    QVector<float> weights;

    Matrix2D<float> *fused_y_channel = new Matrix2D<float>(static_cast<uint>(ROWS), static_cast<uint>(COLUMNS));

    auto start = std::chrono::steady_clock::now();
    for ( int i = 0; i < imfs_depth; ++i ) {
        Matrix2D<float> *fused_imf = new Matrix2D<float>(static_cast<uint>(ROWS), static_cast<uint>(COLUMNS));

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

                            local_energy += imfs.at(j).at(i)->ValueAt(row, column) * imfs.at(j).at(i)->ValueAt(row, column);
                        }
                    }
                    nom   += local_energy*imfs.at(j).at(i)->ValueAt(x, y);
                    denom += local_energy;

                }//images_count

                // Avoid divisions with very small values, set pixel to 0
                if ( denom < 0.001f){
                    nom   = 0.f;
                    denom = 1.f;
                }
                fused_imf->SetCellValue(static_cast<uint>(x), static_cast<uint>(y), nom/denom);
            } //columns
        } //rows

        fused_image = fused_imf->ConvertToQImage();
        QString filename = "Images/results/imf_fused_" + QString::number(i) + ".png";
        fused_image->save(filename);
        filename = "Images/results/imf_fused_" + QString::number(i) + ".txt";
        fused_imf->SaveToFile(filename);

        *fused_y_channel = *fused_y_channel + *fused_imf;

    } //imfs_depth

    auto end = std::chrono::steady_clock::now();
    std::cout << "Fuse IMFs runtime: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<< " ms"<<std::endl<< std::flush;


    fused_y_channel->ScaleToInterval(16, 236);
    fused_image = fused_y_channel->ConvertToQImage();
    fused_image->save("Images/results/output.png");
}
