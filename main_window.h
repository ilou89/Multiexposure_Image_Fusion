#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLabel>
#include <QImage>
#include <memory>
#include "fabemd_fusion.h"
#include "zoom_graphics_view.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow *GetInstance();
    ~MainWindow();
    void SetFusedImage(QImage *fused_image);

private slots:
    void on_actionOpen_Images_triggered();
    void on_pushButton_released();

private:
    MainWindow(QWidget *parent = nullptr);
    static MainWindow *instance;

    void resizeEvent(QResizeEvent* event);
    Ui::MainWindow *ui;
    QVector<QLabel*> in_im_widgets;
    QVector<QImage*> inputImages;
    std::unique_ptr<FabemdFusion> fabemdDecomposer;
    QGraphicsScene *scene;
    QGraphicsPixmapItem* output_picture;
    int image_count;
    float aspect_ratio;
};

#endif // MAIN_WINDOW_H
