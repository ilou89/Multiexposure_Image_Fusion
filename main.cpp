#include "main_window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow *main_window = MainWindow::GetInstance();
    main_window->show();
    return a.exec();
}
