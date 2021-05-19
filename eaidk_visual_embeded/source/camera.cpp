#include "header/camera.h"

#include <QString>
#include <QFileDialog>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <QPainter>
#include <QPixmap>
#include <QImage>

#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QFileDialog>

#include <iostream>
#include <sstream>
using namespace std;
Camera::Camera()
{
    ImgCount = 0;
    Gray_Flag = 0;
}

void Camera::save_camera()
{
    //---保存数据帧---

    //左摄像头图片存储路径
    //将计数的整型转换成string
    stringstream stemp;
    stemp<<ImgCount;
    string scount = stemp.str();
    //拼接字符串为完整存储路径
    string str="F:\\Code\\QT\\CoCamera\\pic\\left\\CamL_";
    str += scount;
    str += ".jpg";

    //右摄像头图片存储路径
    //将计数的整型转换成string
    stringstream stempr;
    stempr<<ImgCount;
    string scountr = stempr.str();
    //拼接字符串为完整存储路径
    string strr="F:\\Code\\QT\\CoCamera\\pic\\right\\CamR_";
    strr += scountr;
    strr += ".jpg";


    //QString qstr = QString::fromStdString(str);
    //ui->label_test->setText(qstr);


    imwrite(str,srcImageL);
    imwrite(strr,srcImageR);
    //imwrite("F:\\Code\\QT\\CoCamera\\pic\\left\\ImgR.jpg",srcImageR);
    ImgCount++;

}

void Camera::convert2gray()
{
    Gray_Flag = !Gray_Flag;
}
