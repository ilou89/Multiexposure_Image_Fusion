#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLabel>
#include <QImage>
#include <memory>
#include "fabemd_fusion.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void SetFusedImage(QImage *fused_image);
    ~MainWindow();

private slots:
    void on_actionOpen_Images_triggered();
    void on_pushButton_released();

private:
    void resizeEvent(QResizeEvent* event);
    Ui::MainWindow *ui;
    QVector<QLabel*> in_im_widgets;
    QVector<QImage*> inputImages;
    std::unique_ptr<FabemdFusion> fabemdDecomposer;
    QGraphicsScene *scene;
    QGraphicsPixmapItem* output_picture;
};

#endif // MAIN_WINDOW_H
