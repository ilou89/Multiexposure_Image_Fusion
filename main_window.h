#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QLabel>
#include <QImage>

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

private:
    Ui::MainWindow *ui;
    QVector<QLabel*> in_im_widgets;
    QVector<QImage*> inputImages;
};

#endif // MAIN_WINDOW_H
