#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // 创建Qapplication应用对象
    QApplication a(argc, argv);
    // 窗口对象
    MainWindow w;

    const QSize MAIN_SIZE_MAX = QSize(1677715, 1677715);
    w.setMaximumSize(MAIN_SIZE_MAX);


    // 窗口对象默认不显示，需要调用show方法进行显示
    w.show();

    //让应用程序进入消息循环机制，代码阻塞进行
    return a.exec();
}
