#include "main.h"

using namespace std;
using namespace cv;
using namespace rs;

#define _SHOW_PHOTO

RMVideoCapture capture("/dev/video0", 3);
char window_name[30] = "Tuning";
Mat img;
/*************************************************************************
*  函数名称：onMouse
*  功能说明：实时显示鼠标指针位置的 RGB 值
*  参数说明：无
*  函数返回：无
*  修改时间：2017-07-27
*************************************************************************/
static void onMouse(int event, int x, int y, int f, void *)
{
    Mat image = img.clone();
    Vec3b rgb = image.at<Vec3b>(y, x);
    int B = rgb.val[0];
    int G = rgb.val[1];
    int R = rgb.val[2];

    Mat HSV;
    Mat RGB = image(Rect(x, y, 1, 1));
    cvtColor(RGB, HSV, CV_BGR2HSV);

    Vec3b hsv = HSV.at<Vec3b>(0, 0);
    int H = hsv.val[0];
    int S = hsv.val[1];
    int V = hsv.val[2];

    char name[30];
    sprintf(name, "R=%d", R);
    putText(image, name, Point(150, 40), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);

    sprintf(name, "G=%d", G);
    putText(image, name, Point(150, 80), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);

    sprintf(name, "B=%d", B);
    putText(image, name, Point(150, 120), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);

    sprintf(name, "H=%d", H);
    putText(image, name, Point(25, 40), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);

    sprintf(name, "S=%d", S);
    putText(image, name, Point(25, 80), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);

    sprintf(name, "V=%d", V);
    putText(image, name, Point(25, 120), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);

    sprintf(name, "X=%d", x);
    putText(image, name, Point(25, 300), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 0, 255), 2, 8, false);

    sprintf(name, "Y=%d", y);
    putText(image, name, Point(25, 340), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 0, 255), 2, 8, false);

    //imwrite("hsv.jpg",image);
    imshow(window_name, image);
}

// 相机参数
int exp_time = 63;
int gain = 40;
int brightness_ = 10;
int whiteness_ = 86;
int saturation_ = 60;

void on_expTracker(int, void *)
{
    //capture.set(CV_CAP_PROP_EXPOSURE,exp_time);
    capture.setExposureTime(0, ::exp_time); //settings->exposure_time);
}
void on_gainTracker(int, void *)
{
    //capture.set(CV_CAP_PROP_GAIN,::gain);
    capture.setpara(gain, brightness_, whiteness_, saturation_); // cap.setExposureTime(0, ::exp_time);//settings->exposure_time);
}
void on_brightnessTracker(int, void *)
{

    //capture.set(CV_CAP_PROP_BRIGHTNESS,::brightness_);
    capture.setpara(gain, brightness_, whiteness_, saturation_); //cap.setExposureTime(0, ::exp_time);//settings->exposure_time);
}
void on_whitenessTracker(int, void *)
{
    //capture.set(CV_CAP_PROP_WHITE_BALANCE_BLUE_U,::whiteness_);
    capture.setpara(gain, brightness_, whiteness_, saturation_); // cap.setExposureTime(0, ::exp_time);//settings->exposure_time);
}
void on_saturationTracker(int, void *)
{
    //capture.set(CV_CAP_PROP_SATURATION,::saturation_);
    capture.setpara(gain, brightness_, whiteness_, saturation_); //cap.setExposureTime(0, ::exp_time);//settings->exposure_time);
}
/*************************************************************************
*  函数名称：setcamera
*  功能说明：设定相机曝光参数
*  参数说明：无
*  函数返回：完成设定之后，按'w'关闭窗口
*  修改时间：2017-07-27
*************************************************************************/
bool setcamera()
{
    // #ifdef _SHOW_PHOTO
    //     namedWindow(wndname, 1);
    // #endif
    //RMVideoCapture capture("/dev/video0", 3);
    capture.setVideoFormat(800, 600, 1);

    //capture.open(0);
    //capture.set(CV_CAP_PROP_FRAME_WIDTH,800);
    //capture.set(CV_CAP_PROP_FRAME_HEIGHT,600);
    // capture.setExposureTime(0, 62);//settings->exposure_time);
    if (!capture.startStream())
    {
        cout << "Open Camera failure.\n";
        return false;
    }

    //printf("%d %d \n",img.cols,img.rows);
    //if(img.empty())
    //	return 0;
    capture >> img;
    cout << img.cols << " " << img.rows << endl;
    namedWindow("Tuning", 1);
    createTrackbar("exposure_time", "Tuning", &::exp_time, 100, on_expTracker);
    createTrackbar("gain", "Tuning", &::gain, 100, on_gainTracker);
    createTrackbar("whiteness", "Tuning", &::whiteness_, 100, on_whitenessTracker);
    createTrackbar("brightness_", "Tuning", &::brightness_, 100, on_brightnessTracker);
    createTrackbar("saturation", "Tuning", &::saturation_, 100, on_saturationTracker);
    on_brightnessTracker(0, 0);
    on_expTracker(0, 0);
    on_gainTracker(0, 0);
    on_saturationTracker(0, 0);
    on_whitenessTracker(0, 0);

    while (1)
    {
        capture >> img;
        if (img.empty())
            continue;
        imshow(window_name, img);
        setMouseCallback(window_name, onMouse, 0);
        int c = waitKey(20);
        if ((char)c == 'w')
            break;
    }
    destroyWindow(window_name);
    usleep(1000);
    return true;
    //RMVideoCapture cap("/dev/video0", 3);
}

/*************************************************************************
*  函数名称：KylinBotMsgPullerThreadFunc
*  功能说明：数据接收线程相关函数
*  参数说明：无
*  函数返回：无
*  修改时间：2017-07-27
×  TODO: 添加初始化
*************************************************************************/
void PullMsg()
{
    /*
    // Get fifo free space
    int len = FIFO_GetFree(&rx_fifo);
    // If fifo free space insufficient, pop one element out
    if (!len)
    {
        uint8_t b;
        len = FIFO_Pop(&rx_fifo, &b, 1);
    }
    // Read input stream according to the fifo free space left
    len = read_serial(rx_buf[1], len, TIMEOUT);
    // Push stream into fifo
    FIFO_Push(&rx_fifo, rx_buf[1], len);
    // Check if any message received
    if (Msg_Pop(&rx_fifo, rx_buf[1], &msg_head_kylin, &kylinMsg))
    {
        // Dnl_ProcKylinMsg(&kylinMsg);
    }
    if (Msg_Pop(&rx_fifo, rx_buf[1], &msg_head_sr04s, &sr04sMsg))
    {
        Maf_Proc(&sr04maf[SR04_IDX_F], sr04sMsg.fixed);
        Maf_Proc(&sr04maf[SR04_IDX_M], sr04sMsg.moble);
        Maf_Proc(&sr04maf[SR04_IDX_L], sr04sMsg.left);
        Maf_Proc(&sr04maf[SR04_IDX_R], sr04sMsg.right);
        //Dnl_ProcSr04sMsg(&sr04sMsg);
    }
    if (Msg_Pop(&rx_fifo, rx_buf[1], &msg_head_zgyro, &zgyroMsg))
    {
        //Dnl_ProcZGyroMsg(&zgyroMsg);
    }
    if (Msg_Pop(&rx_fifo, rx_buf[1], &msg_head_pos_calib, &posCalibMsg))
    {
        //Dnl_ProcPosCalibMsg(&posCalibMsg);
    }
    */
}

void *KylinBotMsgPullerThreadFunc(void *param)
{
    /*
    while (exit_flag == 0)
    {
        PullMsg();
        usleep(1000);
    }
    return NULL;
    */
}
/*************************************************************************
*  函数名称：KylinBotMsgPusherThreadFunc
*  功能说明：数据发送线程相关函数
*  参数说明：无
*  函数返回：无
*  修改时间：2017-07-27
×  TODO: 添加初始化
*************************************************************************/
void PushMsg()
{
    /*
    uint32_t len = Msg_Push(&tx_fifo, tx_buf[1], &msg_head_kylin, &txKylinMsg);
    FIFO_Pop(&tx_fifo, tx_buf[1], len);
    write_serial(tx_buf[1], len, TIMEOUT);
    */
}

void *KylinBotMsgPusherThreadFunc(void *param)
{
    /*
    while (exit_flag == 0)
    {
        PushMsg();
        usleep(4000);
    }
    */
}
/*************************************************************************
*  函数名称：KylinBotMarkDetecThreadFunc
*  功能说明：检测算法线程函数
*  参数说明：无
*  函数返回：无
*  修改时间：2017-07-27
×  TODO: 添加初始化
*************************************************************************/
void *KylinBotMarkDetecThreadFunc(void *param)
{
    /*
    Mat frame;
    vector<vector<Point>> squares;
    int lostFlag = false;
    //KylinBotMsgPullerThreadFunc(NULL);
    //printf("hjhklhjllllllhkl\n");
    printf("exit_flag=%d\n", exit_flag);
    int lostCount = 0;
    int CountVframe = 0;
    int fflage = -1;
    while (exit_flag == 0) //&&(capture.read(frame)))
    {
        squares.clear();
        double t = (double)getTickCount();

        capture >> frame;
        if (frame.empty())
            continue;

        int dif_x = 0, dif_y = 0;
        Mat src = frame.clone();
        //cout << "detection_mode=" << (int)detection_mode << endl;

        cout << "1. Ultrasonic: "
             << " Left: " << sr04maf[SR04_IDX_L].avg << "  Right: " << sr04maf[SR04_IDX_R].avg << "  Fixed: " << sr04maf[SR04_IDX_F].avg << "  Mobile: " << sr04maf[SR04_IDX_M].avg << endl;
        cout << "2. WorkState: "
             << "coutLogicFlag: " << coutLogicFlag << " coutLogicFlag_PutBox: " << coutLogicFlag_PutBox << " coutLogicFlag_PutBox2toBox1: " << coutLogicFlag_PutBox2toBox1 << endl;
        cout << "3. State: " << workStageCout << workStateCout << endl;
        cout << "abs(tz): " << abs(tz) << " lostFlag: " << lostFlag << endl;
        cout << "deltaAngle: " << deltaAngle << " zgyroMsg.angle: " << zgyroMsg.angle << " lastZGyroMsg.angle: " << lastZGyroMsg.angle << endl;
        cout << "txKylinMsg.cbus.cp.x: " << txKylinMsg.cbus.cp.x << " txKylinMsg.cbus.cp.y: " << txKylinMsg.cbus.cp.y << " txKylinMsg.cbus.cp.z: " << txKylinMsg.cbus.cp.z << endl;
        cout << "KylinMsg.cbus.cp.x: " << kylinMsg.cbus.cp.x << " kylinMsg.cbus.cp.y: " << kylinMsg.cbus.cp.y << " kylinMsg.cbus.cp.z: " << kylinMsg.cbus.cp.z << endl;
        cout << "txKylinMsg.cbus.gp.e: " << txKylinMsg.cbus.gp.e << " txKylinMsg.cbus.gp.c: " << txKylinMsg.cbus.gp.c << endl;
        cout << "kylinMsg.cbus.gp.e: " << kylinMsg.cbus.gp.e << " kylinMsg.cbus.gp.c: " << kylinMsg.cbus.gp.c << endl;
        cout << "txKylinMsg.cbus.gv.e: " << txKylinMsg.cbus.gv.e << " txKylinMsg.cbus.gv.c: " << txKylinMsg.cbus.gv.c << endl;
        cout << "kylinMsg.cbus.gv.e: " << kylinMsg.cbus.gv.e << " kylinMsg.cbus.gv.c: " << kylinMsg.cbus.gv.c << endl;

        cout << "------------------------------------------------------------------------------" << endl;
        
        switch (detection_mode)

        {
        case 0: //do nothing
            //TODO:
            //imshow("IM",frame);
            //cout << "detection_mode=" << (int)detection_mode << endl;
            break;
        case 1: //detect squares
            //cout << "detection_mode=" << (int)detection_mode << endl;
            findSquares(src, frame, squares);
            LocationMarkes(squares);
            drawSquares(frame, squares);
            if (squares.size() > 0)
            {
                lostCount = 0;
                lostFlag = false;
                CountVframe++;
                // txKylinMsg.cbus.cp.x = tx;
                // txKylinMsg.cbus.cv.x = 500;
                // txKylinMsg.cbus.cp.y = tz;
                // txKylinMsg.cbus.cv.y = 800;
                // txKylinMsg.cbus.cp.z = ry * 3141.592654f / 180;
                // txKylinMsg.cbus.cv.z = 500;
                // txKylinMsg.cbus.gp.e = ty;
                // txKylinMsg.cbus.gv.e = 0;
            }
            if (squares.size() == 0)
            {
                lostCount++;
                if (lostCount >= 3)
                {
                    printf("lost frame\n");
                    lostCount = 0;
                    lostFlag = true;
                    tx = DIFFCONST;
                    ty = 0;
                    tz = 0;
                    rx = 0;
                    ry = 0;
                    rz = 0;
                    rstRmp();
                }
            }
            //TODO: 本 if 语句使用 fixed 还是 mobile 超声波?
            //TODO: 矩形检测 flag 置 true 过程中, 超声波阈值宏定义
            if ((sr04maf[SR04_IDX_M].avg < 410 && sr04maf[SR04_IDX_F].avg < 500) || (abs(tz) < 700 && (lostFlag == false) && CountVframe > 15))
            { //Usue ultra sonic distance for controlling. Detection_mode will be changed in main.
                finishDetectBoxFlag = true;
                CountVframe = 0;
            }
            else
            {
                finishDetectBoxFlag = false;
            }
            if (coutLogicFlag == 9 && ((sr04maf[SR04_IDX_M].avg < 460 && sr04maf[SR04_IDX_F].avg < 550) || (abs(tz) < 500 && (lostFlag == false) && CountVframe > 10)))
            {
                finishDetectBoxFlag_PutBox = true;
                CountVframe = 0;
            }
            else
            {
                finishDetectBoxFlag_PutBox = false;
            }
            printf("tz=%lf\n", tz);
            break;
        case 2: //detect green area
            //cout << "detection_mode=" << (int)detection_mode << endl;

            fflage = Color_detect(src, dif_x, dif_y);
            if (fflage == 0)
                CountVframe++;
            tx = 3 * (dif_x - DIF_CEN);
            // txKylinMsg.cbus.cp.x = 10 * dif_x;
            cout << "tx=" << tx << endl;

            if (abs(tx) < 40 && (CountVframe > 100 || fflage)) //number of pixels
            {
                CountVframe = 0;
                finishDetectCentroidFlag = true;

                
                if (coutLogicFlag == INT_MAX)
                {
                    finishDetectCentroidFlag_PutBox2toBox1 = true;
                }
            }
            else
            {
                finishDetectCentroidFlag = false;
                finishDetectCentroidFlag_PutBox2toBox1 = false;
            }
            break;
        case 3: //follow line
            //cout << "detection_mode=" << (int)detection_mode << endl;
            break;
        case 4: //follow line
            //cout << "detection_mode=" << (int)detection_mode << endl;
            break;
        case 5: //follow line

            break;
        default:
            break;
        }
        int c = waitKey(1);
        if ((char)c == 'q')
            break;
    }
    */
}
/*************************************************************************
*  函数名称：init
*  功能说明：程序初始化
*  参数说明：无
*  函数返回：无
*  修改时间：2017-07-27
×  TODO: 添加初始化
*************************************************************************/
void init()
{
}
int main()
{
    // 相机相关参数设定
    if (!setcamera())
    {
        cout << "Setup camera failure. Won't do anything." << endl;
        return -1;
    }

    // 系统初始化
    init();

    // 打开串口
    const char *device = "/dev/ttyTHS2";
    if (connect_serial(device, 115200) == -1)
    {
        printf("serial open error!\n");
        return -1;
    }

    // 创建线程
    MyThread kylibotMsgPullerTread;     //数据读取
    MyThread kylibotMsgPusherTread;     //数据发送
    MyThread kylibotMarkDetectionTread; //检测线程

    kylibotMsgPullerTread.create(KylinBotMsgPullerThreadFunc, NULL);
    kylibotMsgPusherTread.create(KylinBotMsgPusherThreadFunc, NULL);
    kylibotMarkDetectionTread.create(KylinBotMarkDetecThreadFunc, NULL);

    //主逻辑循环
    while (1)
    {
        // workState: 0->
        switch (workState)
        {
        case 0:
            break;
        case 1:
            break;
        default:
            break;
        }
    }
    return 0;
}