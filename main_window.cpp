#include "main_window.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QPixmap>

MainWindow* MainWindow::instance = nullptr;

MainWindow *MainWindow::GetInstance()
{
    if ( instance == nullptr ) {
          instance = new MainWindow();
    }

    return instance;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    fabemdDecomposer =  std::make_unique<FabemdFusion>();
    fabemdDecomposer->SetInputImages(&inputImages);

    scene = new QGraphicsScene(this);
    ui->GraphicsView->setScene(scene);
}

void MainWindow::SetFusedImage(QImage *fused_image)
{
    QPixmap pix;
    pix = QPixmap::fromImage(*fused_image);
    if(pix.isNull()==0){
        scene->addPixmap(pix);
    }

//    ui->graphicsView->fitInView(pix.rect());
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
        //TODO: not sure if memory dealocation should be performed as it is...
        //Clear previous input
        for(int i = 0; i < in_im_widgets.length(); ++i){
            delete in_im_widgets.at(i);
        }

        for(int i = 0; i < inputImages.length(); ++i){
            delete inputImages.at(i);
        }
        in_im_widgets.clear();
        inputImages.clear();

        //Create ui widgets, load images
        image_count = filenames.count();
        std::unique_ptr<QImage> first_image = std::make_unique<QImage>(QImage(filenames.at(0)));
        aspect_ratio = static_cast<float>(first_image->width())/static_cast<float>(first_image->height());
        int height = ui->GraphicsView->height()/filenames.count();
        int width  = static_cast<int>(aspect_ratio*height);
        for (int i =0;i<filenames.count();i++){
            QLabel* imageLabel = new QLabel(this);
            imageLabel->setScaledContents(true);
            in_im_widgets.push_back(imageLabel);

            ui->verticalLayout_3->insertWidget(0, imageLabel);//   addWidget(imageLabel);
            QImage* image = new QImage(filenames.at(i));
            inputImages.push_back(image);

            imageLabel->setPixmap(QPixmap::fromImage(*image));
            imageLabel->setFixedSize(width, height);
        }

        scene->clear();
    }
}

void MainWindow::on_pushButton_released()
{ 
    if ( inputImages.length() == 0 ) {
        return;
    }

    int resy = inputImages.at(0)->height();
    int resx = inputImages.at(0)->width();

    fabemdDecomposer->FuseImages(resx, resy);
    SetFusedImage(fabemdDecomposer->GetFusedImage().get());
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    ui->GraphicsView->mapToScene(ui->GraphicsView->viewport()->rect().center());
}
