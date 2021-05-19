#ifdef FASTCV
#include "fastcv.hpp"
#include "openai_io.hpp"
#else
#include <opencv2/opencv.hpp>
#endif

#ifndef WINDOWS
#ifndef ANDROID
//#include <thread.h>
#include "source/thread.h"
#endif
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <dirent.h>
#else
#include <thread>
#define F_OK 0
#endif

#ifdef ANDROID
#include <thread>
#endif



#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <algorithm>
#include <vector>
#include <chrono>
#include <mutex>

#include <sys/stat.h>
#include <string.h>
//#include "util.h"
#include "source/config.h"
#include "source/util.h"
//#include "config.h"
//#include "faceapp.hpp"
#include "header/face_attr/faceapp.hpp"


#ifndef MODEL_DIR
#define  MODEL_DIR "./models/"
#endif

#define MAX_FACE_SIZE 10000
#define MAX_NAME_LEN 256
#define CALIBRATION_NUM 10
using namespace std;
#ifdef FASTCV
using namespace fcv;
#else
using namespace cv;
#endif
#define FEATURE_SIZE 512
struct FaceDB{
    char name[MAX_NAME_LEN+1];
    float feature[FEATURE_SIZE];
    int status; //0--init 1--used 
};
enum Image_Status {
    INITIALIZE,
    NEW_ADD_IMAGE,
    SIMILIAR_IMAGE,
    SAME_IMAGE,
};
typedef struct mutisensor_Face {
    CvRect pos[MAX_FACE_NUM];
    faceapp::FacePosData box[MAX_FACE_NUM];
    faceapp::FaceLandmarkData landmark[MAX_FACE_NUM];
    faceapp::FaceAttributeData attr[MAX_FACE_NUM];
    faceapp::FaceQualityData quality[MAX_FACE_NUM];
    int num;
    const char* name;
	int time;
    int drawflag = 0;
    Image_Status img_status;
#ifdef USE_SDK_FACEDB
    float score;
#endif
} Mface;
typedef struct Recognized_Face {
	int id;
	Mat facemat;
    faceapp::FaceDetctData ddata;
    faceapp::FaceAttributeData attr;
    faceapp::FaceQualityData quality;
    const char* name;
	int timestart;
	int newfaceFlag = 0;
    int recognizedFlag = 0;
	int drawFlag = 0;
	pthread_mutex_t Mutex;
#ifdef USE_SDK_FACEDB
    float score;
#endif
} Rface;

typedef struct {
    float score;
    int i;
}ScoreItem;

typedef struct {
//    int DeviceID;
    int videoWidth;
    int videoHeight;
//    int showfps;
//    int showlandmark;
    int useapi; // 0- detect+recognize 1- detect only 2- detect,recoginze seperately 
//    int step;
    int dbsize;
//    int ShowRegisterName;
//    int RegisterStart;
    int RegisterMethod;

    int width;
    int height;
    int scale;
    string path;
    float threshold;
    bool batchregist;
}ConfigParam;

#if ((defined WINDOWS) || (defined ANDROID))
class Thread{
public:
   Thread(){}
   void start(){
       auto func=std::bind(run,this);
       th=thread(func);
   }
   void join(){th.join();}
   static void run(void *ptr);
private:
    thread th;

};
#endif
class AlgThread : public Thread
{
private:
    ConfigParam param;
    Mat m_Mat;
    Mat m_srcMat;

	Mat show_registed;
    int thread_status_exit;


    cv::Mutex mt;
    std::mutex m_mtx_reg;

    Rect res;
    FaceAPP mFace;
    int face_id;
#ifndef USE_SDK_FACEDB
    FaceDB *facedb;
#endif
    float m_fps;
	float calibration_num = CALIBRATION_NUM;
public:
    Mface  m_face;
	float g2cScale = 0.0;
	cv::Rect g2cRect = {0,0,0,0};
	Rface r_face;
    bool regist_label = false;
    string reg_name = "";
    bool face_rec_label = false;
    AlgThread(ConfigParam& param_)
    {
        param=param_;
#ifndef USE_SDK_FACEDB
        facedb=new FaceDB[MAX_FACE_SIZE];
#endif
        show_registed = Mat(720,120,CV_8UC3);
        mFace= faceapp::CreateFaceAPPInstance();
        face_id=0;
        thread_status_exit = 0;
        m_face.name="";
#ifndef USE_SDK_FACEDB
        LoadData();
#else
        faceapp::SetData(mFace,"dbsize",(float*)&param.dbsize); 
        face_id=faceapp::FaceOpenDB(mFace);
        if(face_id<0){
            printf("can't  open face db\n");
            exit(1);
        }

        //printf("debuginfo ------USE_SDK_FACEDB is ture.\n");
#endif
        ShowRegister();
        int minsize = Util::String2Int(Config::Instance()->GetValue("MinFaceSize"));
        int clarity = Util::String2Int(Config::Instance()->GetValue("Clarity"));
        FaceDemoInit(0.9,0.9,0.9,0.8,0.65, minsize,clarity);
    };
    ~AlgThread(){
#ifndef USE_SDK_FACEDB
        delete facedb;
#else
        faceapp::FaceCloseDB(mFace);
#endif
        faceapp::Destroy(mFace);
    }
    void setUseApiParams(int id);
    void sendFrame(Mat &mat, Mat &src);
    void run();
    Mface getFace();
    Mat& getRegister();
    void set_thread_exit(){thread_status_exit = 1;usleep(50*1000);}
    float getFPS(){return m_fps;}
    int FaceRecognize(char *name);
    void HandleRegist(Mat imge);
	int Tracker(Mat &mat,Mat &src);
#ifdef USE_SDK_FACEDB
    int RemoveDBData(const char *name){
        return faceapp::FaceDelDB(mFace,name);
    }
#endif
    unsigned long long get_ms();
private:
    void getframe(Mat& m);
    void getSrcframe(Mat& m);
    void FaceDemoInit(float a, float b, float c, float d, float factor, int mim_size, int clarity);
    int CompareFaceDB(float feature[]);
    int Register(Mat &mat,float *feature,char *name=NULL);
	int BinocularCalibration(Mat &grayframe, Mat &colorframe,float *scale,cv::Rect &Rect);
    int FaceRecognize(Mat &mat,Mat &gray,int policy);
	int LivenessCheck(cv::Mat &frame, faceapp::FacePosData& box);

#ifndef USE_SDK_FACEDB
    void LoadData();
    void SaveData();
#endif
    void ShowRegister();
public:
    int liveness_status_ = false;
};
bool check_single();

