#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "header/camera.h"
//#include "header/face_rec/face_rec.hpp"

#include <QMessageBox>
#include <QAction>
#include <iostream>
#include <fstream>
#include <string>

#include "json.hpp"
#include <arpa/inet.h>
#include <cstdio>
#include <netinet/in.h>
#include <sys/socket.h>

using json = nlohmann::json;

using namespace cv;
using namespace std;

#define FAN 71

#define FRAME_H 240
#define FRAME_W 360

#define NORMAL_FRAME_H 480
#define NORMAL_FRAME_W 640

#define CONFIG_FILE_PATH "demo.conf"

int sock = socket(AF_INET, SOCK_STREAM, 0);
sockaddr_in serv_addr;
bool flag = true;
std::string host;
std::string port;

int SendAll(int &sock, unsigned char *data_sent, int data_len) {
    int sent_data_num;
    int remain_size = data_len;
    int point = 0;
    while (remain_size > 0) {
        sent_data_num = send(sock, data_sent, data_len, 0);
        remain_size = remain_size - sent_data_num;
        point += sent_data_num;
        printf("send %d bytes data, remain %d bytes data\n", sent_data_num,
               remain_size);
    }
    return point;
}

int MainWindow::LoadConfig(ConfigParam &param,int batch)
{
    if (!Config::Instance()->LoadConfig(CONFIG_FILE_PATH)) {
        cerr << "load config failed" << endl;
        return 1;
    }
    int videoWidth = Util::String2Int(Config::Instance()->GetValue("VideoWidth"));
    int videoHeight = Util::String2Int(Config::Instance()->GetValue("VideoHeight"));
    int scale = Util::String2Int(Config::Instance()->GetValue("Scale"));
    int useapi = Util::String2Int(Config::Instance()->GetValue("UseApi"));
    int dbsize = Util::String2Int(Config::Instance()->GetValue("Dbsize"));
//    int rstart = Util::String2Int(Config::Instance()->GetValue("RegisterStart"));
    int rmethod = Util::String2Int(Config::Instance()->GetValue("RegisterMethod"));
    float threshold  = Util::String2Double(Config::Instance()->GetValue("ThresHold"));
    std::string facePicturePATH = Config::Instance()->GetValue("FacePicturePATH");
    int width = videoWidth / scale;
    int height = videoHeight / scale;
    param={videoWidth,videoHeight,useapi,dbsize, rmethod, width, height, scale, facePicturePATH,threshold,(bool)batch};
    return 0;
}

MainWindow::MainWindow(QWidget *parent) :
                       QMainWindow(parent),
                       ui(new Ui::MainWindow)
{
    img_length = 21;
    ui->setupUi(this);
    //ui->save_camera->setEnabled(0);
    //ui->close_camera->setEnabled(0);
    QAction *about = new QAction("&About");
    ui->menuBar->addAction(about);
    connect(about, &QAction::triggered, this, [=]{QMessageBox::about(this, "Version", " Version:    1.0");});

    face_attr_enabled = false;
	face_rec_enabled = false;
	face_stereo_enabled = false;
    face_track_enabled = false;

    this->cam = new Camera();
    video_base_path = "/sys/class/video4linux/video";
	video_num = 0;
	video_port.push_back(-1);
	video_port.push_back(-1);

    //use socket or not
    ifstream file;
    file.open("/home/openailab/Desktop/eaidk310_face_demo/eaidk_visual_embeded/socket.txt");
    if (file.fail())
    {
        printf("file open fail...");
    }else{  
        std::string data;
        int cc = 0;
        while(getline(file,data)){
            ++cc;
            if(data[0]=='n'){
                flag = false;
                break;
            }else{
                if(cc==2) host = data;
                if(cc==3){
                    port = data;
                    break;
                }
            }
        }
        if(flag){
            //定义socket
            memset(&serv_addr, 0, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;                     //使用IPv4地址
            serv_addr.sin_addr.s_addr = inet_addr(host.c_str()); //具体的IP地址
            //serv_addr.sin_addr.s_addr = inet_addr("192.168.123.78");
            serv_addr.sin_port = htons(std::stoi(port));                   //端口
            ::connect(sock, (sockaddr *)&serv_addr, sizeof(serv_addr));
            //end socket
            printf("success connect");
        }
    }

    /* connects */
    connect(&theTimer, &QTimer::timeout, this, &MainWindow::updateImage);

    connect(ui->face_attr_2, SIGNAL(clicked(bool)), this, SLOT(convert_to_face_attr()));
    connect(ui->face_rec_2, SIGNAL(clicked(bool)), this, SLOT(convert_to_face_rec()));
    connect(ui->rb_face_track, SIGNAL(clicked(bool)), this, SLOT(convert_to_face_track()));
    //connect(ui->stereo_2, SIGNAL(clicked(bool)), this, SLOT(convert_to_face_stereo()));

    /* regist related */
    connect(ui->regist, SIGNAL(clicked(bool)), this, SLOT(user_regist()));
    connect(ui->cancel, SIGNAL(clicked(bool)), this, SLOT(user_cancel_regist()));
    connect(ui->ok, SIGNAL(clicked(bool)), this, SLOT(user_save_regist()));
    connect(ui->ok_2, SIGNAL(clicked(bool)), this, SLOT(get_user_name()));
    connect(ui->cancel_2, SIGNAL(clicked(bool)), this, SLOT(back_to_face_rec()));

    /* hide controls */
    ui->img_label_2->hide();
    ui->cancel->hide();
    ui->regist->hide();
    ui->name->hide();
    ui->ok->hide();
    ui->lineEdit->hide();
    ui->ok_2->hide();
    ui->cancel_2->hide();
    ui->regist_info->hide();

    this->setFixedSize(this->width(),this->height());
    open_camera();

    save = 0;

    ConfigParam param;
    LoadConfig(param,false);

    algThd = new AlgThread(param);
    algThd->start();
    ResImgSiz = cv::Size(NORMAL_FRAME_W, NORMAL_FRAME_H);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    printf("MainWindow::closeEvent call.\n");
    algThd->set_thread_exit();
    theTimer.stop();
    usleep(100*1000);
}

void MainWindow::updateImage()
{
    ui->label_diku->clear();
    ui->label_diku_2->clear();
    cam->videoCapL>>cam->srcImageL;

    //cv::flip(cam->srcImageL, cam->srcImageL, 0);
    if(cam->srcImageL.data)
    {
        if (cam->srcImageL.cols != 0 && cam->srcImageL.rows!= 0)
		{
            Mat ResImg = Mat(ResImgSiz, cam->srcImageL.type());
        	if(face_rec_enabled)
        	{

                //face_rec->face_rec_run(cam->srcImageL);
                algThd->setUseApiParams(0);
                algThd->face_rec_label = true;
                cv::resize(cam->srcImageL, ResImg, ResImgSiz, CV_INTER_LINEAR);
                algThd->sendFrame(ResImg, cam->srcImageL);
                Mface face_result = algThd->getFace();

                char facepath[256];
                sprintf(facepath,"%s/%s.png","./faces/", face_result.name);
                if(!face_result.drawflag){
                    //return;
                }

                // socket send
                if(flag&&face_result.drawflag && strcmp(face_result.name, "")!=0){
                    json json_data;
                    std::string sstr = face_result.name;

                    //构造json_str数据
                    std::string j_s = "[{\"left\":" + std::to_string(face_result.pos[0].x) +
                                      ",\"bottom\" : " + to_string(face_result.pos[0].y+face_result.pos[0].height) +
                                      ", \"right\" : " + to_string(face_result.pos[0].x+face_result.pos[0].width) +
                                      ", \"top\" : " + std::to_string(face_result.pos[0].y) +
                                      ", \"classNo\" : " + "\""+sstr +"\""+
                                      ", \"prob\" :  \"" + std::to_string(100*face_result.score) + "%\" }]";


                    //json_data["name"] = str;
                    cv::imwrite("/home/openailab/Desktop/eaidk310_face_demo/eaidk_visual_embeded/temp_face.jpg",ResImg);
                    cv::Mat img1 = ResImg;
                    //socket 发送
                    std::vector<int> param{CV_IMWRITE_JPEG_QUALITY, 80};
                    std::vector<uchar> image_buff; // encode后的图片数据
                    cv::imencode(".jpg", img1, image_buff, param);
                    int image_size = image_buff.size();
                    printf("image size %d\n", image_size);
                    //发送长度
                    char image_data_length[7];
                    sprintf(image_data_length, "%06d", image_size);

                    int send_len_num = send(sock, image_data_length,
                                            6, 0);
                    printf("len data sent %d bytes\n", send_len_num);
                    unsigned char image_buff_send[image_size];
                    for (int i = 0; i < image_size; ++i) {
                        image_buff_send[i] = image_buff[i];
                    }

                    //发送json,json 数据长度固定为4位
                    std::string json_sent = json_data.dump();
                    char json_data_length[5];
                    sprintf(json_data_length, "%04d", j_s.length());
                    std::string json_data_num = std::to_string(j_s.length());
                    printf("json data length %s\n", json_data_length);
                    send(sock, json_data_length, 4, 0);

                    sleep(1);
                    printf("sleeping\n");
                    int sent_json_num = send(sock, j_s.c_str(), j_s.length(), 0);
                    printf("sent json %d bytes\n", sent_json_num);
                    int send_image_num = SendAll(sock, image_buff_send, image_size);
                    printf("image data sent %d bytes\n", send_image_num);
                    printf("%s\n",j_s.c_str());
                    sleep(6);
                }

                if(face_result.drawflag && strcmp(face_result.name, "")!=0 && strcmp(face_result.name, "unknown")!=0){
                    if(access(facepath, F_OK) < 0)
                    {
                        printf("%s:%d %s not exist.\n", __func__, __LINE__, facepath);
                        return;
                    }

                    int x = face_result.pos[0].x;
                    int y = face_result.pos[0].y;
                    int w = face_result.pos[0].width;
                    int h = face_result.pos[0].height;
                    line(cam->srcImageL, cvPoint(x, y), cvPoint(x, y + 20), cvScalar(0, 255, 0, 0), 2);
                    line(cam->srcImageL, cvPoint(x, y), cvPoint(x + 20, y), cvScalar(0, 255, 0, 0), 2);

                    line(cam->srcImageL, cvPoint(x + w - 20, y), cvPoint(x + w, y), cvScalar(0, 255, 0, 0), 2);
                    line(cam->srcImageL, cvPoint(x + w, y), cvPoint(x + w, y + 20), cvScalar(0, 255, 0, 0), 2);

                    line(cam->srcImageL, cvPoint(x, y + h), cvPoint(x + 20, y + h), cvScalar(0, 255, 0, 0), 2);
                    line(cam->srcImageL, cvPoint(x, y + h), cvPoint(x, y + h - 20), cvScalar(0, 255, 0, 0), 2);

                    line(cam->srcImageL, cvPoint(x + w, y + h), cvPoint(x + w, y + h - 20), cvScalar(0, 255, 0, 0), 2);
                    line(cam->srcImageL, cvPoint(x + w, y + h), cvPoint(x + w - 20, y + h), cvScalar(0, 255, 0, 0), 2);


                    Mat diku = cv::imread(facepath);
                    cvtColor(diku, diku, CV_BGR2RGB);
                    QImage imagel = QImage((uchar*)(diku.data), diku.cols, diku.rows, QImage::Format_RGB888);
                    ui->label_diku->setPixmap(QPixmap::fromImage(imagel));
                    ui->label_diku->resize(imagel.size());
                    ui->label_diku->show();
                    //QString qsname = QString::fromUtf8(face_result.name);
                    //ui->label_name->setText(qsname);

                    cv::Mat realtime_face;
                    cam->srcImageL(Rect(face_result.pos[0].x, face_result.pos[0].y, face_result.pos[0].width, face_result.pos[0].height)).copyTo(realtime_face);
                    cvtColor(realtime_face, realtime_face, CV_BGR2RGB);
                    cv::resize(realtime_face, realtime_face, Size(120, 120), CV_INTER_LINEAR);
//                    cv::imshow("test", realtime_face); //test
                    QImage imagel2 = QImage((uchar*)(realtime_face.data), realtime_face.cols, realtime_face.rows, realtime_face.cols*realtime_face.channels(), QImage::Format_RGB888);
                    ui->label_diku_2->setPixmap(QPixmap::fromImage(imagel2));
                    ui->label_diku_2->resize(imagel2.size());
                    ui->label_diku_2->show();

                    char faceinfostr[1024];
                    sprintf(faceinfostr,"人脸识别：\n Age: %d\n Gender:%d\n Emotion:%d\n Position:x %d, y %d, w %d, h %d\n Quality: %f\n Score: %f\n Name: %s\n", \
                            face_result.attr[0].age, face_result.attr[0].gender, face_result.attr[0].emotion, \
                            face_result.pos[0].x, face_result.pos[0].y, face_result.pos[0].width, face_result.pos[0].height, face_result.quality[0].score, \
                            face_result.score, face_result.name);

                    ui->textEdit_info->setText(QString::fromUtf8(faceinfostr));
                }else{
                    ui->label_diku->clear();
                    ui->label_diku_2->clear();
                    ui->textEdit_info->clear();
                }

        	}
        	else if(face_attr_enabled)
        	{
                //face_attr->face_attr_run(cam->srcImageL);
                algThd->setUseApiParams(1);

                cv::resize(cam->srcImageL, ResImg, ResImgSiz, CV_INTER_LINEAR);
                algThd->sendFrame(ResImg, cam->srcImageL);
                Mface face_result = algThd->getFace();
                //printf("debuginfo, num:%d,====================================\n", face_result.num);
                if(face_result.num >= 1)
                {
                    //ui->textEdit_info->clear();
                    //return;
                }

                int x = face_result.pos[0].x;
                int y = face_result.pos[0].y;
                int w = face_result.pos[0].width;
                int h = face_result.pos[0].height;
                line(cam->srcImageL, cvPoint(x, y), cvPoint(x, y + 20), cvScalar(0, 255, 0, 0), 2);
                line(cam->srcImageL, cvPoint(x, y), cvPoint(x + 20, y), cvScalar(0, 255, 0, 0), 2);

                line(cam->srcImageL, cvPoint(x + w - 20, y), cvPoint(x + w, y), cvScalar(0, 255, 0, 0), 2);
                line(cam->srcImageL, cvPoint(x + w, y), cvPoint(x + w, y + 20), cvScalar(0, 255, 0, 0), 2);

                line(cam->srcImageL, cvPoint(x, y + h), cvPoint(x + 20, y + h), cvScalar(0, 255, 0, 0), 2);
                line(cam->srcImageL, cvPoint(x, y + h), cvPoint(x, y + h - 20), cvScalar(0, 255, 0, 0), 2);

                line(cam->srcImageL, cvPoint(x + w, y + h), cvPoint(x + w, y + h - 20), cvScalar(0, 255, 0, 0), 2);
                line(cam->srcImageL, cvPoint(x + w, y + h), cvPoint(x + w - 20, y + h), cvScalar(0, 255, 0, 0), 2);

                char faceinfostr[5][1024];
                QString qstrFace;
                for(int i=0;i<face_result.num && i<5; i++){
                    sprintf(faceinfostr[i], "第%d个人\n Age:%d Gender:%d Emotion:%d\n Position:x %d, y %d, w %d, h %d\n Quality: %f\n", \
                            i+1, face_result.attr[i].age, face_result.attr[i].gender, face_result.attr[i].emotion, \
                            face_result.pos[i].x, face_result.pos[i].y, face_result.pos[i].width, face_result.pos[i].height, face_result.quality[i].score);
                    qstrFace += QString::fromUtf8(faceinfostr[i]);
                }

                ui->textEdit_info->setText(qstrFace);
            }else if(face_track_enabled){

                algThd->setUseApiParams(6);

                cv::resize(cam->srcImageL, ResImg, ResImgSiz, CV_INTER_LINEAR);
                algThd->Tracker(ResImg,cam->srcImageL);
                cv::resize(cam->srcImageL, ResImg, ResImgSiz, CV_INTER_LINEAR);
                algThd->sendFrame(ResImg, cam->srcImageL);
                Mface face_result = algThd->getFace();

                int x = face_result.pos[0].x;
                int y = face_result.pos[0].y;
                int w = face_result.pos[0].width;
                int h = face_result.pos[0].height;
                line(cam->srcImageL, cvPoint(x, y), cvPoint(x, y + 20), cvScalar(0, 255, 0, 0), 2);
                line(cam->srcImageL, cvPoint(x, y), cvPoint(x + 20, y), cvScalar(0, 255, 0, 0), 2);

                line(cam->srcImageL, cvPoint(x + w - 20, y), cvPoint(x + w, y), cvScalar(0, 255, 0, 0), 2);
                line(cam->srcImageL, cvPoint(x + w, y), cvPoint(x + w, y + 20), cvScalar(0, 255, 0, 0), 2);

                line(cam->srcImageL, cvPoint(x, y + h), cvPoint(x + 20, y + h), cvScalar(0, 255, 0, 0), 2);
                line(cam->srcImageL, cvPoint(x, y + h), cvPoint(x, y + h - 20), cvScalar(0, 255, 0, 0), 2);

                line(cam->srcImageL, cvPoint(x + w, y + h), cvPoint(x + w, y + h - 20), cvScalar(0, 255, 0, 0), 2);
                line(cam->srcImageL, cvPoint(x + w, y + h), cvPoint(x + w - 20, y + h), cvScalar(0, 255, 0, 0), 2);

                char faceinfostr[1024];
                sprintf(faceinfostr,"人脸跟踪：\nPosition:x %d, y %d, w %d, h %d\n", \
                        face_result.pos[0].x, face_result.pos[0].y, face_result.pos[0].width, face_result.pos[0].height);

                ui->textEdit_info->setText(QString::fromUtf8(faceinfostr));

            }
		}

        cvtColor(cam->srcImageL, cam->srcImageL, CV_BGR2RGB);
        QImage imagel = QImage((uchar*)(cam->srcImageL.data), cam->srcImageL.cols, cam->srcImageL.rows, QImage::Format_RGB888);
        ui->img_label->setPixmap(QPixmap::fromImage(imagel));
        ui->img_label->resize(imagel.size());
        ui->img_label->show();

    }
}

void MainWindow::paintEvent(QPaintEvent *e)
{
}

void MainWindow::open_camera()
{
	std::string file_name;
	bool video_exist = false;
    int left = -1;
    int right = -1;
	for (int i=0; i<=10; i++)
        {
                file_name = video_base_path;
                file_name += to_string(i);
                file_name += "/name";
                video_exist = find_stereo_left_video(file_name);
		if (video_exist)
		{
			//video_port[video_num] = i;
			//video_num++;
			left = i;
		}

                video_exist = find_stereo_right_video(file_name);
		if (video_exist)
		{
			//video_port[video_num] = i;
			//video_num++;
			right = i;
		}

		//if (video_num == 2)
		//	break;
        }

        if(cam->videoCapL.open(0))
    	{

            cam->srcImageL = Mat::zeros(cam->videoCapL.get(CV_CAP_PROP_FRAME_HEIGHT), cam->videoCapL.get(CV_CAP_PROP_FRAME_WIDTH), CV_8UC3);
            theTimer.start(33);
    	}
}

void MainWindow::close_camera()
{
    theTimer.stop();         // stop update img
}

void MainWindow::convert_to_face_attr()
{
    if(ui->face_attr_2->isChecked()){
    ui->regist->hide();
//    ui->user_info->hide();

//    ui->user_face_info1->hide();
//    ui->user_face_info2->hide();
//    ui->user_face_info3->hide();
//    ui->user_face_info4->hide();
//    ui->user_face_info5->hide();

    face_attr_enabled = true;
	face_rec_enabled = false;
	face_stereo_enabled = false;
    face_track_enabled = false;

    cam->videoCapL.set(CV_CAP_PROP_FRAME_HEIGHT, NORMAL_FRAME_H);
    cam->videoCapL.set(CV_CAP_PROP_FRAME_WIDTH, NORMAL_FRAME_W);
    }
}

void MainWindow::convert_to_face_rec()
{
    if(ui->face_rec_2->isChecked()){

    /* control hide and show */
    ui->regist->show();

    face_attr_enabled = false;
	face_rec_enabled = true;
	face_stereo_enabled = false;
    face_track_enabled = false;

    cam->videoCapL.set(CV_CAP_PROP_FRAME_HEIGHT, NORMAL_FRAME_H);
    cam->videoCapL.set(CV_CAP_PROP_FRAME_WIDTH, NORMAL_FRAME_W);
    }
}

void MainWindow::convert_to_face_track()
{
    if(ui->rb_face_track->isChecked()){

    face_attr_enabled = false;
    face_rec_enabled = false;
    face_stereo_enabled = false;
    face_track_enabled = true;

    cam->videoCapL.set(CV_CAP_PROP_FRAME_HEIGHT, NORMAL_FRAME_H);
    cam->videoCapL.set(CV_CAP_PROP_FRAME_WIDTH, NORMAL_FRAME_W);
    }
}

void MainWindow::convert_to_face_stereo()
{

}

void MainWindow::user_regist()
{
    face_attr_enabled = false;
    face_rec_enabled = false;
    face_stereo_enabled = false;

    ui->regist->hide();
    ui->regist_info->show();
    ui->cancel_2->show();
    ui->ok_2->show();
}

void MainWindow::get_user_name()
{
    ui->ok_2->hide();
    ui->cancel_2->hide();
    ui->regist_info->hide();

    ui->name->show();
    ui->lineEdit->show();
    ui->ok->show();
    ui->cancel->show();

    user_shot_img();
}

void MainWindow::back_to_face_rec()
{
    ui->ok_2->hide();
    ui->cancel_2->hide();
    ui->regist_info->hide();
    ui->regist->show();

    user_regist_finish();
}

void MainWindow::user_cancel_regist()
{
    ui->img_label_2->hide();
    ui->img_label->show();

    ui->cancel->hide();
    ui->ok->hide();
    ui->lineEdit->hide();
    ui->name->hide();
    ui->regist->show();

    user_regist_finish();
}

void MainWindow::user_save_regist()
{
    /* save db_config */
    QString username = ui->lineEdit->text();
    if (username.isEmpty())
    {
        QMessageBox::critical(this, "critical", "user name cannot be empty!");
        ui->img_label_2->hide();
        ui->img_label->show();

        ui->cancel->hide();
        ui->ok->hide();
        ui->lineEdit->hide();
        ui->name->hide();
        ui->regist->show();

        return;
    }

    /* save img file to ./face */
    gen();
    std::string img_path = "./face/" + img_name;
    cv::cvtColor(saved_img, saved_img, COLOR_BGR2RGB);
    cv::imwrite(img_path, saved_img);

    ui->img_label_2->hide();
    ui->img_label->show();

    algThd->reg_name = username.toStdString();
    QString content = username.append(":").append(QString::fromStdString(img_name)).append("\n");
    QString path = "./face/db_config";
    QFile file(path);

    file.open(QIODevice::Append);
    file.write(content.toUtf8());
    file.close();

    ui->cancel->hide();
    ui->ok->hide();
    ui->lineEdit->hide();
    ui->name->hide();
    ui->regist->show();

    //algThd->HandleRegist(saved_img);
    algThd->regist_label = true;

    printf("reg_name is %s \n", algThd->reg_name.c_str());

    user_regist_finish();
}

void MainWindow::user_shot_img()
{
    ui->img_label->hide();
    ui->img_label_2->show();

    ui->cancel->show();
    ui->ok->show();
    ui->lineEdit->show();
    ui->name->show();

    //显示左摄像头彩色帧
    saved_img = cam->srcImageL.clone();
    //QImage imagel = QImage((uchar*)(cam->srcImageL.data), cam->srcImageL.cols, cam->srcImageL.rows, QImage::Format_RGB888);
    QImage imagel = QImage((uchar*)(saved_img.data), saved_img.cols, saved_img.rows, QImage::Format_RGB888);
    ui->img_label_2->setPixmap(QPixmap::fromImage(imagel));
    ui->img_label_2->resize(imagel.size());
    ui->img_label_2->show();

}

void MainWindow::user_regist_finish()
{
    face_rec_enabled = true;
}

char MainWindow::genRandom()  
{
    static const char alphanum[] = "0123456789"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "abcdefghijklmnopqrstuvwxyz";
    int stringLength = sizeof(alphanum) - 1;

    return alphanum[rand() % stringLength];
}

void MainWindow::gen()
{
	img_name = "";
	srand(time(0));
    for(int z=0; z < img_length; z++)
    {
        img_name += genRandom();
    }

    img_name += ".jpg";
}

bool MainWindow::find_stereo_left_video(std::string file_name)
{
	//std::string video_name = "Stereo Vision 1";
	std::string video_name = "Stereo Vision 2";

	ifstream fin(file_name, ios::in);
	if(!fin)
	{
		return false;
	}
	char str[50];
	fin.getline(str, 50);

	fin.close();

	if (video_name == str)
	{
		return true;
	}

	return false;
}

bool MainWindow::find_stereo_right_video(std::string file_name)
{
	//std::string video_name = "Stereo Vision 2";
	std::string video_name = "Stereo Vision 1";

	ifstream fin(file_name, ios::in);
	if(!fin)
	{
		return false;
	}
	char str[50];
	fin.getline(str, 50);

	fin.close();

	if (video_name == str)
	{
		return true;
	}

	return false;
}

