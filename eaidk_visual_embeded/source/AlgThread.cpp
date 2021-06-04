#include "AlgThread.h"

using namespace std;
#ifdef FASTCV
using namespace fcv;
#else
using namespace cv;
#endif

#if ((defined WINDOWS) || (defined ANDROID))
void Thread::run(void *ptr) {
	((AlgThread*)ptr)->run();
}
#endif

void AlgThread::sendFrame(Mat &mat, Mat &src)
{
    mt.lock();
    m_Mat = mat.clone();
    m_srcMat = src.clone();
    mt.unlock();
}
void AlgThread::getframe(Mat& m)
{
    mt.lock();
    m = m_Mat.clone();
    mt.unlock();
}
void AlgThread::getSrcframe(Mat& m)
{
    mt.lock();
    m = m_srcMat.clone();
    mt.unlock();
}
Mface AlgThread::getFace()
{
    Mface r;
    r = m_face;
    return r;
}
void AlgThread::FaceDemoInit(float a, float b, float c, float d, float factor, int mim_size,int clarity)
{
    const char* name[]={"a","b","c","d","factor","min_size","clarity","none"};
    float value[]={a,b,c,d,factor,(float)mim_size,(float)clarity};
    faceapp::SetParameter(mFace,name,value);
#if (!defined WINDOWS)
    unsigned char contract_id[16] = {
        0x10,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x01, //OEM ID
        0x00,
        0x01,  //contract ID
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x01  //Device
    };
    unsigned char user_password[16] = {
        0x01,
        0x23,
        0x45,
        0x67,
        0x89,
        0xab,
        0xcd,
        0xef,
        0x01,
        0x23,
        0x45,
        0x67,
        0x89,
        0xab,
        0xcd,
        0xef
    };
    //faceapp::AuthorizedDevice(contract_id,user_password);
#endif

}
int AlgThread::CompareFaceDB(float feature[])
{
    float score=0;
    ScoreItem max_score;
    max_score.i=-1;
    max_score.score=0.0;
#ifndef USE_SDK_FACEDB
    for (int i=0;i<face_id;++i) {
        faceapp::Compare(mFace,facedb[i].feature,feature,&score);
        if (score>param.threshold) {
            if (max_score.score<score) {
                max_score.score=score;
                max_score.i=i;
            }
        }
    }
    if (max_score.i==-1) {
        m_face.name = "unknown";
    }else{
        m_face.name=facedb[max_score.i].name;
    }
#else
    const char *fname;
    faceapp::FaceQueryDB(mFace,feature,&score,&fname);
    m_face.score=score;
    printf("%s %d score:%f fname:%s, param.threshold:%f\n",__FUNCTION__, __LINE__, score, fname, param.threshold);
    if (score>param.threshold) {
        max_score.score=score;
        max_score.i=0;
        m_face.name=fname;
    }
    if (max_score.i==-1) {
        m_face.name = "unknown";
    }
#endif
    printf("%s %d max_score.i:%d\n", __FUNCTION__, __LINE__, max_score.i);
    return max_score.i;
}

int AlgThread::FaceRecognize(char *name)
{
    int res;
    int index=-1;
    Mat mat;
    float feature1[FEATURE_SIZE];

//    if (param.ShowRegisterName) {
//        printf("%s :  \n", name);
//    }
    getframe(mat);
    m_face.img_status=INITIALIZE;
    int face_recognize_return_value = FaceRecognize(mat,mat,0);
    printf("face_recognize_return_value:%d\n",face_recognize_return_value);
    if(face_recognize_return_value/*FaceRecognize(mat,mat,0)*/==SUCCESS){
        int get_feature_result = faceapp::GetFeature(mFace, feature1, &res);
        if(get_feature_result/*faceapp::GetFeature(mFace,feature1,&res)*/==SUCCESS){
           index=Register(mat,feature1,name);
           if(index < 0)
           {
               return -1;
           }
        }
        else
        {
            return -1;
            printf("getfeature error....\n");
        }
    }else{
        return -1;
        if (param.useapi!=1){
          printf("  fail to register %s (no face detect)\n",name);
        }
    }

#ifndef USE_SDK_FACEDB
    float score;
    if (face_id>=MAX_FACE_SIZE) return 0;

    if (index == face_id) {
        if(strcmp(facedb[index-1].name,name)==0){
            m_face.img_status=NEW_ADD_IMAGE;
            return 0;
        }
    }
    if (index>=0 && index<face_id) {
        faceapp::Compare(mFace,facedb[index].feature,feature1,&score);
        if(strcmp(facedb[index].name,name)==0){
            m_face.img_status=SAME_IMAGE;
            return 1;
        }
        printf("  ??? on %s and %s %f\n",name,facedb[index].name,score);
        m_face.img_status=SIMILIAR_IMAGE;
    }
#else
    if (index>=0) {
        if (strcmp(m_face.name,"unknown")==0){
            m_face.img_status=NEW_ADD_IMAGE;
            if (param.RegisterMethod==1) {
                printf("%s --nomatch\n",name);
            }
            return 0;
        }
        if (strcmp(m_face.name,name)==0){
            m_face.img_status=SAME_IMAGE;
            return 1;
        }
        if (param.RegisterMethod==1) {
            printf("%s --mismatch\n",name);
        }
        printf("  ??? on %s and %s %f\n",name,m_face.name,m_face.score);
        m_face.img_status=SIMILIAR_IMAGE;
    }
#endif
    return 0;
}

unsigned long long AlgThread::get_ms()
{
    struct timeval time_v = {0};
    gettimeofday(&time_v, NULL);
    return (unsigned long long)(time_v.tv_sec*1000) + (time_v.tv_usec/1000);
}

void AlgThread::run()
{
    //struct timeval tv_start, tv_end,t0,t1;
    static unsigned long long tv_start, tv_end,t0,t1;
    float feature0[FEATURE_SIZE];
    float feature1[FEATURE_SIZE];
    float score;

    //gettimeofday(&t0, NULL);
    for (int i=0;i<FEATURE_SIZE;++i) {
        feature0[i]=0.1;
        feature1[i]=1.0;
    }
    while(1) {
        int res;
        int ret;
        Mat mat;
		Mat grayframe;

        if(thread_status_exit == 1)
            return;

        if(6 == param.useapi)
        {
            continue;
        }
        //gettimeofday(&tv_start, NULL);
        //t1 = get_ms();
        std::unique_lock<std::mutex> lck(m_mtx_reg);
        getframe(mat);
		getSrcframe(grayframe);
        if (mat.empty()) continue;
        int face_recognize_return_value = FaceRecognize(mat,grayframe,0);


        if(0x0b == param.useapi)//liveness do not need compare feature
        {
            continue;
        }
        printf("%s %d face_recognize_return_value:%d\n", __FUNCTION__, __LINE__, face_recognize_return_value);
        if(face_recognize_return_value != SUCCESS){
            printf("%s %d face_recognize fail...\n", __FUNCTION__, __LINE__);
            t0=tv_start;
            if (param.useapi==2) {
                //gettimeofday(&tv_end, NULL);
                //tv_end = get_ms();
                //m_fps = (float)1000.0 /(tv_end.tv_sec * 1000 + tv_end.tv_usec / 1000 - tv_start.tv_sec * 1000 - tv_start.tv_usec / 1000);
            }
            continue;
        }
        else
        {
            printf("%s %d face_recognize success...\n", __FUNCTION__, __LINE__);
        }
        if(1 == param.useapi)
        {
            continue;
        }
        ret=faceapp::GetFeature(mFace,feature1,&res);
        if (ret!=SUCCESS) {
            printf("%s %d get feature fail....\n", __FUNCTION__, __LINE__);
            t0=tv_start;
            if(ret==ERROR_BAD_QUALITY)
                m_face.name="unknown";
            continue;
        }

        if(regist_label){
            Register(mat, feature1, (char*)reg_name.c_str());
            regist_label = false;
            reg_name = "";
        }

        if(face_rec_label){
            ret = CompareFaceDB(feature1);
            if(ret >= 0)
            {

            }
        }
//        faceapp::Compare(mFace,feature0, feature1, &score);
//        printf("%s %d score:%f\n", __FUNCTION__, __LINE__, score);
//        if (score<param.threshold) {
//            t0=tv_start;
//            memcpy(feature0,feature1,sizeof(feature0));
//        }//else{
//            //if (t1.tv_sec-t0.tv_sec>5  && m_face.drawflag) {
//            t1 = get_ms();
//            printf("%s %d time_interval:%ld\n", __FUNCTION__, __LINE__, (t1-t0));
//            if (t1-t0>5000) {
//                //
//                t0 = get_ms();
//            }
            //CompareFaceDB(feature1);
        //}
        //gettimeofday(&tv_end, NULL);
        //tv_end = get_ms();
        //m_fps = (float)1000.0 /(tv_end.tv_sec * 1000 + tv_end.tv_usec / 1000 - tv_start.tv_sec * 1000 - tv_start.tv_usec / 1000);
    }
}

void AlgThread::HandleRegist(Mat image)
{

}

#ifndef USE_SDK_FACEDB
void AlgThread::SaveData()
{
    FILE *fp;
    fp=fopen("facedb.dat","wb");
    if (fp) {
        fwrite(facedb, sizeof(FaceDB), MAX_FACE_SIZE, fp);
        fclose(fp);
    }
}
void AlgThread::LoadData()
{
    FILE *fp;
    fp=fopen("facedb.dat","rb");
    if (fp) {
        fread(facedb, sizeof(FaceDB), MAX_FACE_SIZE, fp);
        fclose(fp);
    }else{
        memset(facedb,0,sizeof(FaceDB)* MAX_FACE_SIZE);
    }

    for (int i=0;i<MAX_FACE_SIZE;++i) {
        if (facedb[i].status==1) {
            ++face_id;
        }
    }
}
#endif

void AlgThread::ShowRegister()
{
    int step = 0;
    char string_m[MAX_NAME_LEN+1];
#ifndef USE_SDK_FACEDB
    for(int m = face_id-1; m >=face_id-6 && m>=0; --m) {
        strcpy(string_m,facedb[m].name);
#else
    for (int m=0;m<6;++m){
        if(faceapp::FaceQueryRecentDB(mFace,m,string_m)!=SUCCESS)
            break;
#endif
        std::string facepath = "./faces/" + String(string_m) + ".png";
#ifdef FASTCV
        Mat rsmat = fcv::imread(facepath, fcv::_IMREAD_UNCHANGED);
#else
        Mat rsmat = imread(facepath, CV_LOAD_IMAGE_UNCHANGED);
#endif
		if (rsmat.rows == 0 || rsmat.cols == 0)break;
        if (rsmat.channels() == 1){
            cv::cvtColor(rsmat, rsmat, cv::COLOR_GRAY2BGR);
        }
        cv::rectangle(rsmat, cvPoint(0, 0), cvPoint(120, 120), cvScalar(255, 255, 255, 0), 3);
        if (rsmat.empty()) {
            continue;
        }
        Mat bgROIMat = show_registed(Rect(0,step*120, 120, 120));
        step++;
        if (!bgROIMat.empty()) {
            rsmat.copyTo(bgROIMat);
        }
    }
}
//return 
//  <0 failure 
//  0..MAX_FACE_SIZE-1 data index
//  MAX_FACE_SIZE data is full
//  face_id the next idex
int AlgThread::Register(Mat &mat,float *feature,char *fname)
{
    int ret;
    char path[64];
    if (param.batchregist) {
        if (m_face.pos[0].width==0 || m_face.pos[0].height==0) {
            return -1;
        }
    }
    ret=CompareFaceDB(feature);
    printf("%s %d ret:%d\n", __FUNCTION__, __LINE__, ret);
    if (ret>=0) {
        //return ret;
    }
    if (param.RegisterMethod==1) {
        return face_id+1;
    }
    sprintf(path, "./faces/%s.png", fname);
    if (access(path,F_OK)==0) {
        fname=NULL;
    }
#ifndef USE_SDK_FACEDB
    if (face_id>=MAX_FACE_SIZE) {
        return MAX_FACE_SIZE;
    }
    memcpy(facedb[face_id].feature, feature, sizeof(facedb[face_id].feature));
    if (fname) {
        strcpy(facedb[face_id].name,fname);
    }else{
        sprintf(facedb[face_id].name, "id_%d", face_id);
    }
    facedb[face_id].status=1;
    char *name=facedb[face_id].name;
    SaveData();
    face_id ++;
#else
    char name[MAX_NAME_LEN+1];
    if (fname) {
        strcpy(name,fname);
    }else{
        sprintf(name, "id_%d", face_id);
    }
    if(SUCCESS!=faceapp::FaceAddDB(mFace,feature,name)){
        return -2;
    }
    faceapp::FaceSaveDB(mFace);
    face_id ++;
#endif
    Rect rcb;
    rcb.x = m_face.pos[0].x - (m_face.pos[0].width >> 2);
    rcb.width = (m_face.pos[0].width * 3) >> 1;
    rcb.y = m_face.pos[0].y - (m_face.pos[0].height >> 2);
    rcb.height = (m_face.pos[0].height * 3) >> 1;
    if(rcb.width > mat.cols) {
        rcb.width = mat.cols;
    }
    if(rcb.height > mat.rows) {
        rcb.height = mat.rows;
    }
    if((rcb.x) < 0) {
        rcb.x = 0;
    }
    if((rcb.y) < 0) {
        rcb.y = 0;
    }
    if((rcb.x + rcb.width) > mat.cols) {
        rcb.x = mat.cols - rcb.width;
    }
    if((rcb.y + rcb.height) > mat.rows) {
        rcb.y = mat.rows - rcb.height;
    }

    Mat src;
    getSrcframe(src);
    Mat mat_tmp ;
    if (param.batchregist) {
        mat_tmp = src;
    }else{
        mat_tmp = src(Rect(rcb.x * param.scale, rcb.y * param.scale, rcb.width * param.scale, rcb.height * param.scale));
    }
    Mat rsmat = Mat(Size(120, 120), src.type());
    if (mat_tmp.channels()==4) {
        cv::cvtColor(mat_tmp, mat_tmp, cv::COLOR_BGRA2BGR);
    }
#ifdef FASTCV
    fcv::resize(mat_tmp, rsmat, Size(120, 120), fcv::_INTER_LINEAR);
    sprintf(path, "./faces/%s.png", name);
    fcv::imwrite(String(path), rsmat);
#else
    resize(mat_tmp, rsmat, Size(120, 120), CV_INTER_LINEAR);
    sprintf(path, "./faces/%s.png", name);
    imwrite(String(path), rsmat);
#endif
    ShowRegister();

    return face_id;
}

Mat& AlgThread::getRegister()
{
	return show_registed;
}

int AlgThread::BinocularCalibration(Mat &colorframe, Mat &grayframe, float *convertScale,cv::Rect &rect) {

    int ret;//return status
    faceapp::FacePosData pos_gray,pos_color;//record pos and landmark
    struct timeval tv1,tv2;
    gettimeofday(&tv1,NULL);
    int res = faceapp::Detect(mFace,faceapp::Image(&grayframe),&ret);
    gettimeofday(&tv2,NULL);
    if(res==SUCCESS){
        if(SUCCESS== faceapp::GetFacePosition(mFace,&pos_gray,&ret)){
            int res = faceapp::Detect(mFace,faceapp::Image(&colorframe),&ret);
            if(res==SUCCESS){
                if(SUCCESS== faceapp::GetFacePosition(mFace,&pos_color,&ret)){
                    *convertScale = (pos_color.x1 - pos_color.x0)/(pos_gray.x1 - pos_gray.x0);
                    rect.x = (int)pos_color.x0 - (int)(pos_gray.x0 * *convertScale);
                    rect.y = (int)pos_color.y0 - (int)(pos_gray.y0 * *convertScale);
                    rect.width = (int)(grayframe.cols * *convertScale);
                    rect.height = (int)(grayframe.rows * *convertScale);
                    return SUCCESS;
                }
            }
            return SUCCESS;
        }
    }else{
        if(ERROR_INVALID_PARAM==ret){
            return ERROR_FAILURE;
        }
    }
    return ERROR_FAILURE;

}


int AlgThread::Tracker(Mat &mat,Mat &src)
{
	int res;
    int ret=ERROR_FAILURE;
	bool detected = false;
    unsigned long long time_start = get_ms();
	ret=faceapp::DetectTrack(mFace, faceapp::Image(&mat), detected, &res);
	if (ret== SUCCESS) {
		int num=MAX_FACE_NUM;
		if(faceapp::GetFacePosition(mFace,&num,m_face.box,&res)==SUCCESS){
             for (int i=0;i<num;++i) {
                m_face.pos[i].x      = m_face.box[i].x0;
                m_face.pos[i].y      = m_face.box[i].y0;
                m_face.pos[i].width  = m_face.box[i].x1-m_face.box[i].x0;
                m_face.pos[i].height = m_face.box[i].y1-m_face.box[i].y0;
				if(detected){
					r_face.ddata.box.x0 = m_face.box[i].x0 * param.scale;
					r_face.ddata.box.y0 = m_face.box[i].y0 * param.scale;
					r_face.ddata.box.x1 = m_face.box[i].x1 * param.scale;
					r_face.ddata.box.y1 = m_face.box[i].y1 * param.scale;
				}
            }
			if(detected){
				struct timeval tv_start;
				gettimeofday(&tv_start, NULL);
				faceapp::GetLandmark(mFace,&r_face.ddata.landmark,&res);
				r_face.ddata.landmark.x[0] = r_face.ddata.landmark.x[0] * param.scale;
				r_face.ddata.landmark.x[1] = r_face.ddata.landmark.x[1] * param.scale;
				r_face.ddata.landmark.x[2] = r_face.ddata.landmark.x[2] * param.scale;
				r_face.ddata.landmark.x[3] = r_face.ddata.landmark.x[3] * param.scale;
				r_face.ddata.landmark.x[4] = r_face.ddata.landmark.x[4] * param.scale;
				r_face.ddata.landmark.y[0] = r_face.ddata.landmark.y[0] * param.scale;
				r_face.ddata.landmark.y[1] = r_face.ddata.landmark.y[1] * param.scale;
				r_face.ddata.landmark.y[2] = r_face.ddata.landmark.y[2] * param.scale;
				r_face.ddata.landmark.y[3] = r_face.ddata.landmark.y[3] * param.scale;
				r_face.ddata.landmark.y[4] = r_face.ddata.landmark.y[4] * param.scale;
				r_face.facemat = src.clone();
				r_face.newfaceFlag = 1;
				r_face.timestart = tv_start.tv_sec;
			}
            faceapp::GetLandmark(mFace,&num,m_face.landmark,&res);
            //printf("m_face.landmark********************************************************* is %f %f %f %f %f\n",m_face.landmark[0].x[0], m_face.landmark[0].x[1], m_face.landmark[0].x[2], m_face.landmark[0].x[3],m_face.landmark[0].x[4]);
            m_face.drawflag=1;
            m_face.num=num;

        }else{
            m_face.drawflag=0;
        }
	}
    unsigned long long time_end = get_ms();
    m_fps = (float)(1000/(time_end - time_start));
    printf("-------------------------------DetectTrack time :%lld\n", time_end - time_start);
	return SUCCESS;
}

void AlgThread::setUseApiParams(int id)
{
    param.useapi = id;
}

int AlgThread::FaceRecognize(Mat &mat,Mat &gray,int policy)
{
    int res;
    int ret=ERROR_FAILURE;
	bool detected = false;
    //m_face.num = 0;
    printf("%s %d param.useapi:%d\n", __FUNCTION__, __LINE__, param.useapi);
    switch (param.useapi) {
    default:
    case 0:
    {
        unsigned long long time_start = get_ms();
        ret=faceapp::DetectRecognize(mFace, faceapp::Image(&mat), policy, &res);
        unsigned long long time_end = get_ms();
        m_fps = (float)(1000/(time_end - time_start));
        printf("detect_recognize_time:%lld\n", time_end - time_start);
        break;
    }
    case 2:
		if(calibration_num > 0){
			float scale = 0;
			cv::Rect rect;
			if(SUCCESS == BinocularCalibration(mat, gray, &scale, rect)){
				calibration_num --;
				g2cScale += scale;
				g2cRect.x += rect.x;
				g2cRect.y += rect.y;
				g2cRect.width += rect.width;
				g2cRect.height += rect.height;
			}
		}else if(calibration_num == 0){
			g2cScale = g2cScale/CALIBRATION_NUM;
			g2cRect.x = g2cRect.x/CALIBRATION_NUM;
			g2cRect.y = g2cRect.y/CALIBRATION_NUM;
			g2cRect.width = g2cRect.width/CALIBRATION_NUM;
			g2cRect.height = g2cRect.height/CALIBRATION_NUM;
			if(g2cRect.x < 0)g2cRect.x = 0;
            if(g2cRect.y < 0)g2cRect.y = 0;
			if((g2cRect.width + g2cRect.x) > mat.cols)g2cRect.width = mat.cols - g2cRect.x;
            if((g2cRect.height + g2cRect.y)> mat.rows)g2cRect.height = mat.rows - g2cRect.y;
			calibration_num --;
		}else{
			Mat mat_input = mat(g2cRect);
			Mat gray_input = Mat(Size((int)(gray.cols * g2cScale),(int)(gray.rows * g2cScale)), gray.type());;
			cv::resize(gray, gray_input, Size((int)(gray.cols * g2cScale),(int)(gray.rows * g2cScale)));
			ret=faceapp::DetectRecognize(mFace, faceapp::Image(&mat_input), faceapp::Image(&gray_input), policy, &res);
		}
		break;
    case 1:
    {
        unsigned long long time_start = get_ms();
        ret=faceapp::Detect(mFace, faceapp::Image(&mat), &res);
        unsigned long long time_end = get_ms();
        m_fps = (float)(1000/(time_end - time_start));
        printf("detect_time:%lld\n", time_end - time_start);
        break;
    }
    case 0x0a:
    {
        unsigned long long time_start = get_ms();
        ret=faceapp::DetectRecognize(mFace, faceapp::Image(&mat), policy, &res);
        unsigned long long time_end = get_ms();
        m_fps = (float)(1000/(time_end - time_start));
        printf("DetectRecognize_time:%lld\n", time_end - time_start);
        break;
    }
    case 0x0b:
    {
        unsigned long long time_start = get_ms();
        //ret=faceapp::DetectRecognize(mFace, faceapp::Image(&mat), policy, &res);
        ret=faceapp::Detect(mFace, faceapp::Image(&mat), &res);
        faceapp::DetectLiveness(mFace, faceapp::Image(&mat), &res);
        unsigned long long time_end = get_ms();
        printf("Detectandliveness_time:%lld\n", time_end - time_start);
        break;
    }
	case 3:
        ret=faceapp::DetectTrack(mFace, faceapp::Image(&mat), detected, &res);
        break;
	case 4:
	    usleep(200000);
        //ret=faceapp::Recognize(mFace, faceapp::Image(&r_face.facemat), &r_face.ddata,&res);
		printf("---------------------------------------recognize\n");
        break;
    }
    if (ret== SUCCESS) {
        int num=MAX_FACE_NUM;
        int liveness;
        if(faceapp::GetLivenessStatus(mFace,&liveness,&res)==SUCCESS){
            //printf("---------------------------------------GetLivenessStatus in.\n");
            if(1 == liveness)
            {
                liveness_status_ = true;
            }
            else if(2 == liveness || 0 == liveness)
            {
                liveness_status_ = false;
            }
            else
            {
                liveness_status_ = false;
            }
        }
        else
        {
            liveness_status_ = false;
        }
        if(faceapp::GetFaceQuality(mFace,&num,m_face.quality,&res)==SUCCESS){
            for (int i=0;i<num;++i) {
                //printf("face [%d] quality score %f clarity %f (%f %f %f)\n",i, m_face.quality[i].score, m_face.quality[i].clarity, m_face.quality[i].leftRight, m_face.quality[i].upDown, m_face.quality[i].horizontal);
            }
        }
		if (param.useapi!=4) {
	        if(faceapp::GetFacePosition(mFace,&num,m_face.box,&res)==SUCCESS){
	            for (int i=0;i<num;++i) {
	                m_face.pos[i].x      = m_face.box[i].x0;
	                m_face.pos[i].y      = m_face.box[i].y0;
	                m_face.pos[i].width  = m_face.box[i].x1-m_face.box[i].x0;
	                m_face.pos[i].height = m_face.box[i].y1-m_face.box[i].y0;

                    if (m_face.pos[i].y < 0) m_face.pos[i].y = 0;
                    if (m_face.pos[i].x < 0) m_face.pos[i].x = 0;
                    if (m_face.pos[i].x + m_face.pos[i].width > mat.cols)  m_face.pos[i].width = mat.cols - m_face.pos[i].x;
                    if (m_face.pos[i].y + m_face.pos[i].height > mat.rows) m_face.pos[i].height = mat.rows - m_face.pos[i].y;
	            }
	            faceapp::GetLandmark(mFace,&num,m_face.landmark,&res);

	            m_face.drawflag=1;
	            m_face.num=num;
        }
            if (param.useapi==2) {
                faceapp::FaceDetctData data;
                faceapp::GetDetectData(mFace,&data,&res);
                if(faceapp::Recognize(mFace,faceapp::Image(&mat),&data,&res)!=SUCCESS)
                    return ERROR_FAILURE;
            }
        }else{
            m_face.drawflag=0;
        }
        if (param.useapi!=3) {
            for(int i=0;i<num;i++){
                faceapp::FaceDetctData data={m_face.box[i],m_face.landmark[i]};
                if(faceapp::FetchFaceAttr(mFace,faceapp::Image(&mat),&data,m_face.attr,&res)==SUCCESS){
                       //printf("age %d gender %d emotion %d\n",m_face.attr[i].age,m_face.attr[i].gender,m_face.attr[i].emotion);
                }
            }
		}

        if (param.useapi==2 || param.useapi==3) {
            return ERROR_FAILURE;
        }
        return SUCCESS;
    }

    m_face.drawflag=0;
    return ERROR_FAILURE;
}

