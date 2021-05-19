#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QCloseEvent>
#include "header/camera.h"
#include "ui_mainwindow.h"
//#include "header/face_rec/face_rec.hpp"
//#include "header/face_stereo/face_stereo.hpp"
//#include "header/face_attr/face_attr.hpp"
#include "header/face_attr/faceapp.hpp"
#include "source/AlgThread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Camera *cam;
    //FaceRec *face_rec;
    //FaceStereo *face_stereo;
    //ft_FaceAttr *face_attr;
    AlgThread *algThd ;
    cv::Size ResImgSiz;
    Mat ResImg;// = Mat(ResImgSiz, frame.type());

    private:
    Ui::MainWindow *ui;
    QTimer theTimer;
    
    bool face_attr_enabled;
    bool face_rec_enabled;
    bool face_stereo_enabled;
    bool face_track_enabled;

    public slots:
    void updateImage();
    void convert_to_face_attr();
    void convert_to_face_rec();
    void convert_to_face_track();
    void convert_to_face_stereo();
    void user_regist();
    void user_cancel_regist();
    void user_save_regist();


    void get_user_name();
    void back_to_face_rec();

protected:
    void closeEvent(QCloseEvent *event);
    void paintEvent(QPaintEvent *e);
    void emotion_gender_age();

    void open_camera();
    void close_camera();
    void prepare_to_convert();

    int img_length;
    std::string img_name;
    cv::Mat saved_img;

    std::string video_base_path;
    std::vector<int> video_port;
    int video_num;

	char genRandom();
	void gen();

    int save;

    void user_regist_finish();
    void user_shot_img();
    bool find_stereo_left_video(std::string);
    bool find_stereo_right_video(std::string);
    int LoadConfig(ConfigParam &param,int batch);

};

#endif // MAINWINDOW_H
