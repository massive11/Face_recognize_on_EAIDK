#ifndef __FACE_APP__HPP__
#define __FACE_APP__HPP__

#include <opencv2/opencv.hpp>

#ifdef WINDOWS
#ifdef SDK_LIB
#define FuncAttribute   __declspec( dllexport )
#else
#define FuncAttribute   __declspec( dllimport )
#endif
#else
#define FuncAttribute   
#endif


#define STATUS_NO_PERSON   3
#define STATUS_DIFF_PERSON 2
#define STATUS_SAME_PERSON 1
#define SUCCESS 0
#define ERROR_INVALID_PARAM -1
#define ERROR_TOO_MANY_REQUESTS -2
#define ERROR_NOT_EXIST -3
#define ERROR_FAILURE -4
#define ERROR_BAD_QUALITY -5
#define ERROR_BAD_LIVENESS_DETECT -6

//#define FEATURE_SIZE 512
#define MAX_FACE_NUM 32

typedef void * FaceAPP;
typedef void * FaceMeas;

namespace faceapp{
typedef struct tagFaceQualityData
{
    float score;                        //�������������Ŷ�
    float leftRight;                    //���ҽǶ�
    float upDown;                       //���½Ƕ�
    float horizontal;                   //ˮƽ�Ƕ�
    float clarity;                      //ͼƬ������( >250 ��ʾͼƬ������)
    float bright;
}FaceQualityData;

//��������������
typedef struct tagFaceAttribute
{
    int gender;						   //�Ա� 0:���ԣ�1:Ů��
    int emotion;                       //���� 0:ƽ����1:����
    int age;                           //����
}FaceAttributeData;

typedef struct tagFaceLandmark{
        float x[5];
        float y[5];
    }FaceLandmarkData;

typedef struct tagJKFaceLandmark{
        float x[106];
        float y[106];
    }JKFaceLandmarkData;

typedef struct tagFacePosData
{
    float x0;                           //�������Ͻ�x0����
    float y0;                           //�������Ͻ�y0����
    float x1;                           //�������Ͻ�x1����
    float y1;                           //�������Ͻ�y1����
    float score;                        //检测框的置信度
}FacePosData;

typedef struct tagFaceDetectData{
    FacePosData box;
    FaceLandmarkData landmark;
}FaceDetctData;

class Image{
public:
    FuncAttribute Image(cv::Mat * mat);
    int width;
    int height;
    void* pmat;
};


FuncAttribute const char* GetVersion(FaceAPP face);
FuncAttribute int DetectRecognize(FaceAPP face,Image image, int policy, int *res);
FuncAttribute int DetectRecognize(FaceAPP face,Image image,Image grayimage,int policy, int *res);
FuncAttribute int Detect(FaceAPP face,Image image,int *res);
FuncAttribute int DetectTrack(FaceAPP face,Image image,bool &detected, int *res);
FuncAttribute int DetectLiveness(FaceAPP face,Image image,int *res);
FuncAttribute int DetectLiveness(FaceAPP face,Image rgbimage,Image grayimage,int *res);
FuncAttribute int Recognize(FaceAPP face,Image image,FaceDetctData* data,int *res);
FuncAttribute int RecognizeAligned(FaceAPP face,Image image,int *res);
FuncAttribute int Landmark(FaceAPP face,Image image,FaceDetctData* data,int *res);
FuncAttribute int FetchFaceQuality(FaceAPP face,Image image,FaceDetctData* data,int *res);
FuncAttribute int Liveness(FaceAPP face,Image image,FaceDetctData* data,int *res);
FuncAttribute int FilterWB(FaceAPP face,Image image,FaceDetctData* data,int *res);


FuncAttribute int Compare(FaceAPP face,float *origin, float *chose, float *score);
FuncAttribute int FaceOpenDB(FaceAPP face);
FuncAttribute int FaceAddDB(FaceAPP face, float* feature, const char *name);
FuncAttribute int FaceDelDB(FaceAPP face, const char *name);
FuncAttribute int FaceSaveDB(FaceAPP face);
FuncAttribute int FaceDelAllDB(FaceAPP face);

FuncAttribute int FaceQueryDB(FaceAPP face,float* feature,float *score,const char **name);
FuncAttribute int FaceQueryRecentDB(FaceAPP face,int n,char *name);
FuncAttribute int FaceQuerySevDB(FaceAPP face,float* feature,float *score,const char **name);
FuncAttribute int FaceCloseDB(FaceAPP face);

FuncAttribute int FastQueryInit(FaceAPP face);
FuncAttribute int FastQueryFlush(FaceAPP face,float *data,int num);
FuncAttribute int FastQueryQuery(FaceAPP face,float *data,float *feature,float *scores);

FuncAttribute int FetchFaceAttr(FaceAPP face,Image image,FaceDetctData* data,FaceAttributeData *face_attr,int *res);
FuncAttribute int GetData(FaceAPP face,const char* name,float *data);
FuncAttribute int GetDetectData(FaceAPP face,FaceDetctData* data,int *res);
FuncAttribute int GetDetectData(FaceAPP face,int *num,FaceDetctData* data,int *res);
FuncAttribute int GetFacePosition(FaceAPP face,FacePosData *pos,int *res);
FuncAttribute int GetFacePosition(FaceAPP face,int *num,FacePosData *pos,int *res);
FuncAttribute int GetFaceAttribute(FaceAPP face,int *num,FaceAttributeData *attr,int *res);
FuncAttribute int GetFeature(FaceAPP face,float *feature,int *res);
FuncAttribute int GetJkLandmark(FaceAPP face,float *landmark,int *res);

FuncAttribute int GetLandmark(FaceAPP face,FaceLandmarkData *landmark,int *res);
FuncAttribute int GetLandmark(FaceAPP face,int *num,FaceLandmarkData *landmark,int *res);
FuncAttribute int GetFaceQuality(FaceAPP face,FaceQualityData *quality,int *res);
FuncAttribute int GetFaceQuality(FaceAPP face,int *num,FaceQualityData *quality,int *res);
FuncAttribute int GetLivenessStatus(FaceAPP face,int *liveness,int *res);
FuncAttribute int SetData(FaceAPP face,const char* name, float *value);
FuncAttribute int SetParameter(FaceAPP face,const char* name[], float *value);
FuncAttribute void Destroy(FaceAPP face);

FuncAttribute FaceMeas CreateFaceMeas(FaceAPP face);
FuncAttribute void ReleaseMeas(FaceAPP face,FaceMeas meas);
FuncAttribute int QueryFaceMeas(FaceAPP face,FaceMeas meas,Image image, std::vector<FaceDetctData> &detect,std::vector<FaceQualityData> &faceQuality);

FuncAttribute FaceAPP CreateFaceAPPInstance();
FuncAttribute int AuthorizedDevice(unsigned char *contract_id,unsigned char* user_password);
}
#endif
