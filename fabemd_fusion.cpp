#include <iostream>
#include <iomanip>

#include "fabemd_fusion.h"
#include <QDebug>
#include <qmath.h>
#include <algorithm>
#include <chrono>
#include "matrix_2d.h"
#include "matrix_factory.h"


FabemdFusion::FabemdFusion()
{
    WIDTH       = 0;
    HEIGHT      = 0;
    scale_y     = true;
    fused_image = nullptr;
}

void FabemdFusion::FuseImages(const int width_, const int height_)
{
    WIDTH       = width_;
    HEIGHT      = height_;
    fused_image = nullptr;

    y_channels.clear();
    cb_channels.clear();
    cr_channels.clear();
    imfs.clear();

    auto start = std::chrono::steady_clock::now();
    RGBToYCbCr();
    DecomposeY();
    FuseIMFs(3);
    FuseCbCr();
    YCbCrToRGB();

    auto end = std::chrono::steady_clock::now();
    std::cout << "Total Runtime:  " << std::setw(10) << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<< " ms" << std::endl<< std::flush;
}

void FabemdFusion::SetInputImages(QVector<QImage *> *images)
{
    inputImages = images;
}

std::unique_ptr<QImage> FabemdFusion::GetFusedImage()
{
    return std::move(fused_image);
}

void FabemdFusion::RGBToYCbCr()
{
    //parse input images, convert them to YCbCr and save each channel to the respective vector
    for(int k = 0; k < inputImages->length(); ++k){
        Matrix2D<float> *y_channel  = new Matrix2D<float>(static_cast<uint>(WIDTH), static_cast<uint>(HEIGHT));
        Matrix2D<float> *cb_channel = new Matrix2D<float>(static_cast<uint>(WIDTH), static_cast<uint>(HEIGHT));
        Matrix2D<float> *cr_channel = new Matrix2D<float>(static_cast<uint>(WIDTH), static_cast<uint>(HEIGHT));

        //convert to ycbcr format
        for(int i = 0; i < WIDTH; ++i){
            for(int j = 0; j < HEIGHT; ++j){
                QColor rgb  = inputImages->at(k)->pixel(i, j);
                float red   = static_cast<float>(rgb.red());
                float green = static_cast<float>(rgb.green());
                float blue  = static_cast<float>(rgb.blue());

#if 0
                y_channel-> SetCellValue(i, j, 16.f  +  65.738f*red/256.f + 129.057f*green/256.f +  25.064f*blue/256.f);
                cb_channel->SetCellValue(i, j, 128.f -  37.945f*red/256.f -  74.494f*green/256.f + 112.439f*blue/256.f);
                cr_channel->SetCellValue(i, j, 128.f + 112.439f*red/256.f -  94.154f*green/256.f -  18.285f*blue/256.f);
#else
                y_channel-> SetCellValue(i, j,  0.299000f*red + 0.587000f*green + 0.114000f*blue);
                cb_channel->SetCellValue(i, j, -0.168736f*red - 0.331264f*green + 0.500000f*blue + 128.f);
                cr_channel->SetCellValue(i, j,  0.500000f*red - 0.418688f*green - 0.081312f*blue + 128.f);
#endif
            }
        }

        y_channels.push_back(y_channel);
        cb_channels.push_back(cb_channel);
        cr_channels.push_back(cr_channel);
    }
}

void FabemdFusion::YCbCrToRGB()
{
    fused_image = std::make_unique<QImage>(WIDTH, HEIGHT, QImage::Format_RGB888);

    for ( int i = 0; i < WIDTH; ++i ) {
        for ( int j = 0; j < HEIGHT; ++j ) {
            float y  = fused_y->ValueAt(i,j);
            float cb = fused_cb->ValueAt(i,j);
            float cr = fused_cr->ValueAt(i,j);

            //TODO values get out of range...not sure what causes it (wrong conversion here, or wrong conversion at rgb2ycbcr)
#if 0
            int r = static_cast<int>(298.082f*y/256.f + 408.583f*cr/256.f - 222.921f);
            int g = static_cast<int>(298.082f*y/256.f - 100.291f*cb/256.f - 208.120f*cr/256.f + 135.576f);
            int b = static_cast<int>(298.082f*y/256.f + 516.412f*cb/256.f - 276.836f);
#else
            int r = static_cast<int>( y + 1.402f*(cr - 128.f) );
            int g = static_cast<int>( y - 0.344136f*(cb - 128.f) - 0.714136f*(cr - 128.f) );
            int b = static_cast<int>( y + 1.772f*(cb - 128.f) );
#endif

            //Fix pixels out or range
            r = (r > 255) ? 255 : r;
            g = (g > 255) ? 255 : g;
            b = (b > 255) ? 255 : b;

            r = (r < 0) ? 0 : r;
            g = (g < 0) ? 0 : g;
            b = (b < 0) ? 0 : b;

            QRgb color = qRgb(r, g, b);

            fused_image->setPixel(i, j, color);
        }
    }

    fused_image->save("Images/results/output.png");
}

void FabemdFusion::DecomposeY()
{
    //window minimum size
    int upper_env_win_size = 2;
    int lower_env_win_size = 2;
    int win_size           = 2;

    QVector<float> maxima_distances;
    QVector<float> minima_distances;
    Matrix2D<float> local_maxima(static_cast<uint>(WIDTH), static_cast<uint>(HEIGHT));
    Matrix2D<float> local_minima(static_cast<uint>(WIDTH), static_cast<uint>(HEIGHT));

    //Init extrema count for the first iteration
    int extrema_count = 5000;
    imfs.resize(y_channels.length());
    const int extrema_threshold = 10;

    //Control the decomposition granularity. Trade-off between performance for image quality
    int imf_count = 0;

    while ( (extrema_count > extrema_threshold) && (imf_count < 5) ) {

        int win_size_prev = win_size;

        //Step 1
        for ( int k = 0; k < y_channels.length(); ++k ) {

            maxima_distances.clear();
            minima_distances.clear();

            local_maxima.Fill(-1.f);
            local_minima.Fill(-1.f);
            Matrix2D<float> *cur_y = y_channels[k];

            //TODO create function detect_local_maxima()
            for ( int i = 1; i < WIDTH - 1; ++i ) {
                for ( int j = 1; j < HEIGHT - 1; ++j ) {

                    if ( ( cur_y->ValueAt(i, j) > cur_y->ValueAt(i - 1, j - 1) ) &&
                         ( cur_y->ValueAt(i, j) > cur_y->ValueAt(i - 1, j)     ) &&
                         ( cur_y->ValueAt(i, j) > cur_y->ValueAt(i - 1, j + 1) ) &&
                         ( cur_y->ValueAt(i, j) > cur_y->ValueAt(i, j - 1)     ) &&
                         ( cur_y->ValueAt(i, j) > cur_y->ValueAt(i, j + 1)     ) &&
                         ( cur_y->ValueAt(i, j) > cur_y->ValueAt(i + 1, j - 1) ) &&
                         ( cur_y->ValueAt(i, j) > cur_y->ValueAt(i + 1, j)     ) &&
                         ( cur_y->ValueAt(i, j) > cur_y->ValueAt(i + 1, j +1)  )){
                            local_maxima.SetCellValue( i, j,  cur_y->ValueAt(i, j));
                            //Fill the maxima distances with the biggest possible distance
                            maxima_distances.push_back(static_cast<float>(qSqrt(WIDTH*WIDTH + HEIGHT*HEIGHT)));
                     }

                    if((cur_y->ValueAt(i, j) < cur_y->ValueAt(i - 1, j - 1)) &&
                       (cur_y->ValueAt(i, j) < cur_y->ValueAt(i - 1, j))     &&
                       (cur_y->ValueAt(i, j) < cur_y->ValueAt(i - 1, j + 1)) &&
                       (cur_y->ValueAt(i, j) < cur_y->ValueAt(i, j - 1))     &&
                       (cur_y->ValueAt(i, j) < cur_y->ValueAt(i, j + 1))     &&
                       (cur_y->ValueAt(i, j) < cur_y->ValueAt(i + 1, j - 1)) &&
                       (cur_y->ValueAt(i, j) < cur_y->ValueAt(i + 1, j))     &&
                       (cur_y->ValueAt(i, j) < cur_y->ValueAt(i + 1, j +1))){
                            local_minima.SetCellValue( i, j,  cur_y->ValueAt(i, j));
                            //Fill the maxima distances with the biggest possible distance
                            minima_distances.push_back(static_cast<float>(qSqrt(WIDTH*WIDTH + HEIGHT*HEIGHT)));
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
            std::cout << "MAX Filter:  " << std::setw(4) << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<< " ms ||" <<" Filter size "<< win_size<<"   "<<std::endl<< std::flush;
            start = std::chrono::steady_clock::now();
            upper_envelope->FilterMean(win_size);
            end = std::chrono::steady_clock::now();
            std::cout << "MEAN Filter: " << std::setw(4) << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<< " ms ||" << " Filter size "<< win_size<<"   "<<std::endl<< std::flush;


            Matrix2D<float> *lower_envelope = new Matrix2D<float>(*y_channels[k]);
            start = std::chrono::steady_clock::now();
            lower_envelope->FilterMin(win_size);
            end = std::chrono::steady_clock::now();
            std::cout << "MIN Filter:  " << std::setw(4) << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<< " ms ||" << " Filter size "<< win_size<<"   "<<std::endl<< std::flush;
            start = std::chrono::steady_clock::now();
            lower_envelope->FilterMean(win_size);
            end = std::chrono::steady_clock::now();
            std::cout << "MEAN Filter: " << std::setw(4) << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<< " ms ||" <<" Filter size "<< win_size<<"   "<<std::endl<< std::flush;


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
    for(int i = 1; i < WIDTH - 1; ++i){
        for(int j = 1; j < HEIGHT - 1; ++j){

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
                        if ( (((cur_row) > 0) && ((j+l) > 0) && ((cur_row) < WIDTH) && ((j+l) < HEIGHT)) && (extrema->ValueAt(cur_row, j+l) >= 0) ) {
                            if(static_cast<float>(qSqrt(win_half_size*win_half_size + l*l)) < distance_temp){
                                distance_temp = static_cast<float>(qSqrt(win_half_size*win_half_size + l*l));
                            }
                        }
                    }
                    //Left
                    int cur_col = j - win_half_size;
                    for(int k = -win_half_size; k < win_half_size; ++k){
                        //If curr index is valid and the is a local extrema
                        if((((i+k) > 0) && ((cur_col) > 0) && ((i+k) < WIDTH) && ((cur_col) < HEIGHT)) && (extrema->ValueAt(i + k, cur_col) >= 0)){
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
                        if((((cur_row) > 0) && ((j+l) > 0) && ((cur_row) < WIDTH) && ((j+l) < HEIGHT)) && (extrema->ValueAt(cur_row, j + l) >= 0)){
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
                        if((((i+k) > 0) && ((cur_col) > 0) && ((i+k) < WIDTH) && ((cur_col) < HEIGHT)) && (extrema->ValueAt(i + k, cur_col) >= 0)){
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

void FabemdFusion::FuseIMFs(const int win_size)
{
    const int half_win     = static_cast<int>(0.5*win_size);
    const int images_count = imfs.length();
    const int imfs_depth   = imfs.at(0).length();

    QVector<float> weights;

    fused_y = new Matrix2D<float>(static_cast<uint>(WIDTH), static_cast<uint>(HEIGHT));

    auto start = std::chrono::steady_clock::now();
    for ( int i = 0; i < imfs_depth; ++i ) {
        Matrix2D<float> *fused_imf = new Matrix2D<float>(static_cast<uint>(WIDTH), static_cast<uint>(HEIGHT));

        for ( int x = 0; x < WIDTH; ++x ) {
            for ( int y = 0; y < HEIGHT; ++y ) {
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
                            } else if ( row >= WIDTH ) {
                                row -= WIDTH;
                            }

                            if ( column < 0 ) {
                                column = -column;
                            } else if ( column >= HEIGHT ) {
                                column -= HEIGHT;
                            }

                            local_energy += imfs.at(j).at(i)->ValueAt(row, column) * imfs.at(j).at(i)->ValueAt(row, column);
                        }
                    }
                    nom   += local_energy*imfs.at(j).at(i)->ValueAt(x, y);
                    denom += local_energy;

                }//images_count

                // Avoid divisions with very small values, set pixel to 0
                if ( denom < 0.0001f){
                    nom   = 0.f;
                    denom = 1.f;
                }
                fused_imf->SetCellValue(x, y, nom/denom);
            } //columns
        } //rows

        fused_image = fused_imf->ConvertToQImage();
        QString filename = "Images/results/imf_fused_" + QString::number(i) + ".png";
        fused_image->save(filename);
        filename = "Images/results/imf_fused_" + QString::number(i) + ".txt";
        fused_imf->SaveToFile(filename);

        *fused_y = *fused_y + *fused_imf;

    } //imfs_depth

    auto end = std::chrono::steady_clock::now();
    std::cout << "Fuse IMFs runtime: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<< " ms"<<std::endl<< std::flush;

    if (scale_y == true) {
        fused_y->ScaleToInterval(0, 255);
    }
}

void FabemdFusion::FuseCbCr()
{
    fused_cb = new Matrix2D<float>(static_cast<uint>(WIDTH), static_cast<uint>(HEIGHT));
    fused_cr = new Matrix2D<float>(static_cast<uint>(WIDTH), static_cast<uint>(HEIGHT));

    const int images_count = cb_channels.length();

    RestoreYChannels();

    for ( int i = 0; i < WIDTH; ++i ) {
        for ( int j = 0; j < HEIGHT; ++j ) {

            float y  = y_channels.at(0)->ValueAt(i, j);
            float cb = cb_channels.at(0)->ValueAt(i, j);
            float cr = cr_channels.at(0)->ValueAt(i, j);

            for (int k = 1; k < images_count; ++k ) {

                if ( fabs(y_channels.at(k)->ValueAt(i, j) - 128.f) < fabs(y - 128.f) ) {
                    y  = y_channels.at(k)->ValueAt(i, j);
                    cb = cb_channels.at(k)->ValueAt(i, j);
                    cr = cr_channels.at(k)->ValueAt(i, j);
                }
            }

            fused_cb->SetCellValue(i, j, cb);
            fused_cr->SetCellValue(i, j, cr);
        }
    }
}

void FabemdFusion::RestoreYChannels()
{
    const int images_count = imfs.length();
    const int imfs_depth   = imfs.at(0).length();

    for ( int i = 0; i < images_count; ++i ) {
        for ( int j =0; j < imfs_depth; ++j ) {
            *y_channels[i] = *y_channels[i] + *imfs.at(i).at(j);
        }
    }
}
