#include "main_window.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    fabemdDecomposer =  std::make_unique<fabemd_decomposer>();
    fabemdDecomposer->setInputImages(&inputImages);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_Images_triggered()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Open Images"), QDir::currentPath(), tr("Image Files (*.png *.jpg *.bmp)"));
    if( !filenames.isEmpty() )
    {
        //TODO: not sure if memory free should be performed as it is...
        //Clear previous input
        //********************
        for(int i = 0; i < in_im_widgets.length(); ++i){
            delete in_im_widgets.at(i);
        }

        for(int i = 0; i < inputImages.length(); ++i){
            delete inputImages.at(i);
        }
        in_im_widgets.clear();
        inputImages.clear();
        //********************


        //Create ui widgets, load images
        //********************
        for (int i =0;i<filenames.count();i++){
            QLabel* imageLabel = new QLabel(this);
            imageLabel->setScaledContents(true);
            in_im_widgets.push_back(imageLabel);

            ui->verticalLayout_3->insertWidget(0, imageLabel);//   addWidget(imageLabel);
            QImage* image = new QImage(filenames.at(i));
            inputImages.push_back(image);

            float image_ratio = static_cast<float>(image->width())/static_cast<float>(image->height());
            qDebug()<<image->width() << image->height()<<image_ratio;
            imageLabel->setPixmap(QPixmap::fromImage(*image));
            imageLabel->setFixedSize(320, 240);
        }
        //********************
    }
}

void MainWindow::on_pushButton_released()
{
    fabemdDecomposer->set_resx(inputImages.at(0)->width());
    fabemdDecomposer->set_resy(inputImages.at(0)->height());
    fabemdDecomposer->getIMFs();

    QPixmap pix;
    pix = QPixmap::fromImage(*fabemdDecomposer->getTestImage());
//    pix = QPixmap::fromImage(*inputImages[0]) ;
    if(pix.isNull()==0){
        scene->addPixmap(pix);
    }
}
