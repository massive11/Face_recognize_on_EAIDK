#ifndef CAMERA
#define CAMERA

#include <QObject>
#include <QTimer>
#include <QPaintEvent>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <sstream>
using namespace cv;

class Camera : public QObject
{
    Q_OBJECT

    public:
    Camera();

    public slots:
    void save_camera();
    void convert2gray();


    public:
    int ImgCount;
    int Gray_Flag;

    Mat srcImageL;
    Mat srcImageR;
    Mat imgSave;
    VideoCapture videoCapL;
    VideoCapture videoCapR;

};

#endif // CAMERA

