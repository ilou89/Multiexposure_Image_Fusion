#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLabel>
#include <QImage>
#include <memory>
#include "fabemd_decomposer.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_Images_triggered();

    void on_pushButton_released();

private:
    Ui::MainWindow *ui;
    QVector<QLabel*> in_im_widgets;
    QVector<QImage*> inputImages;
    std::unique_ptr<fabemd_decomposer> fabemdDecomposer;
    QGraphicsScene *scene;
    QGraphicsPixmapItem* output_picture;

};

#endif // MAIN_WINDOW_H
