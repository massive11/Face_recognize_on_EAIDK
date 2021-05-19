#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
//    w.setWindowFlags(w.windowFlags() | Qt::WindowMinMaxButtonsHint);
    const QSize MAIN_SIZE_MAX = QSize(1677715, 1677715);
    w.setMaximumSize(MAIN_SIZE_MAX);


//    w.setWindowState(Qt::WindowMaximized);
    w.show();
    return a.exec();
}
