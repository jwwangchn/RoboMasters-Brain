#include "main.h"

using namespace std;
using namespace cv;
using namespace rs;

#define _SHOW_PHOTO
/*************************************************************************
*  函数名称：currentTimeUs
*  功能说明：任务计时
*  参数说明：无
*  函数返回：无
*  修改时间：2017-07-29
*************************************************************************/
double currentTimeUs()
{
    return cvGetTickCount() / cvGetTickFrequency();
}

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
*  TODO: 添加初始化
*************************************************************************/
void init()
{
}
/*************************************************************************
*  函数名称：calibGrabBox
*  功能说明：抓取盒子的时候，进行里程计校准
*  参数说明：无
*  函数返回：无
*  修改时间：2017-07-28
*  TODO: 添加函数体
*************************************************************************/
void calibGrabBox()
{
}
/*************************************************************************
*  函数名称：calibPutBox
*  功能说明：放盒子的时候，进行里程计校准
*  参数说明：无
*  函数返回：无
*  修改时间：2017-07-28
*  TODO: 添加函数体
*************************************************************************/
void calibPutBox()
{
}
/*************************************************************************
*  函数名称：txMoveMsg
*  功能说明：小车移动，值传递，发送给UART
*  参数说明：三个方向的位置和速度
*  函数返回：无
*  修改时间：2017-07-28
*  TODO: 添加函数体
*************************************************************************/
void txMoveMsg(int16_t positionX, int16_t speedX, int16_t positionY, int16_t speedY, int16_t positionZ, int16_t speedZ)
{
}
/*************************************************************************
*  函数名称：txClawBoardMsg
*  功能说明：爪子和挡板，值传递，发送给UART
*  参数说明：爪子和挡板的位置和速度
*  函数返回：无
*  修改时间：2017-07-28
*  TODO: 添加函数体
*************************************************************************/
void txClawBoardMsg(int16_t positionClaw, int16_t speedClaw, int16_t positionBoard, int16_t speedBoard)
{
}

/*************************************************************************
*  函数名称：flagsSet
*  功能说明：标志位, 某一位置位函数
*  参数说明：标志位, 标志位的哪一位置位, 置成 0 还是 1
*  函数返回：无
*  修改时间：2017-07-28
*  TODO: 添加函数体
*************************************************************************/
uint32_t flagsSet(uint32_t flags, int controlBit, int bit)
{
    // 置 1
    if (bit == 1)
    {
        flags |= (1u << controlBit);
    }
    else if (bit == 0)
    {
        flags &= ~(1u << controlBit);
    }
    else
    {
        return flags;
    }
    return flags;
}

/*************************************************************************
*  函数名称：flagsRead
*  功能说明：标志位, 读取标志位函数
*  参数说明：标志位, 读取标志位的哪一位
*  函数返回：无
*  修改时间：2017-07-28
*  TODO: 添加函数体
*************************************************************************/
int flagsRead(uint32_t flags, int readBit)
{
    flags &= (1u << readBit);
    if (flags == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
/*************************************************************************
*  函数名称：graspBoxes
*  功能说明：堆叠盒子
*  参数说明：无
*  函数返回：无
*  修改时间：2017-07-28
*  TODO: 添加函数体
*************************************************************************/
bool graspBoxes()
{
    bool result = false;
    return result;
}

/*************************************************************************
*  函数名称：pileBoxes
*  功能说明：堆叠盒子
*  参数说明：无
*  函数返回：无
*  修改时间：2017-07-28
*  TODO: 添加函数体
*************************************************************************/
bool pileBoxes()
{
    bool result = false;
    return result;
}

int main()
{
    // 计时函数
    missionStartTimeUs = currentTimeUs();
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
        case 1:
            // 1. 矩形引导小车前进，直到 realsense 检测到的距离小于多少时, 切换到 case2
            detection_mode = 1; // 矩形检测
            workStageCout = "阶段 1 : ";
            workStateCout = "矩形引导小车抓盒子";

            // 发送小车前进指令
            // TODO: 根据底层数据格式修改单位, 不能忘了！！！！
            txMoveMsg(tx, X_SPEED, tz, Y_SPEED, ry, Z_SPEED);
            txClawBoardMsg(0, 0, 0, 0);

            // TODO: 根据线程中的标志位切换来修改 if 语句, 对照之前的程序
            if (tx < DISTANCE_RECTANGLE_TO_ARROW)
            {
                workState = 2;
                detection_mode = 2;
            }
            break;
        case 2:
            // 2. 箭头引导小车前进，直到盒子完全进入小车(如何判断)
            detection_mode = 2; // 箭头检测
            workStageCout = "阶段 2 : ";
            workStateCout = "箭头引导小车抓盒子";

            // 发送小车前进指令
            // TODO: 根据底层数据格式修改单位, 不能忘了！！！！
            // TODO: 确认坐标系
            txMoveMsg(tx, X_SPEED, tz, Y_SPEED, ry, Z_SPEED);
            txClawBoardMsg(0, 0, 0, 0);
            // TODO: 根据线程中的标志位切换来修改 if 语句, 对照之前的程序
            if (tx < DISTANCE_RECTANGLE_TO_ARROW)
            {
                workState = 3;
                detection_mode = 0;
            }
            break;
        case 3:
            // 3. 抓取盒子
            detection_mode = 0; // 关闭检测程序
            workStageCout = "阶段 3 : ";
            workStateCout = "抓取盒子";
            // TODO: 抓取函数
            // 小车静止不动
            txMoveMsg(0, 0, 0, 0, 0, 0);
            // TODO: 爪子应该是先合拢, 再升高, 挡板的速度需要再修改
            txClawBoardMsg(0, CLAW_SPEED, 0, BOARD_SPEED);
            // TODO: 根据爪子升高的位置来修改 if 语句
            if (rxMoveClawBoardMsg.positionClaw > CLAW_HEIGHT)
            {
                // TODO: 添加校准函数的位置
                workState = 4;
                detection_mode = 0;
            }
            break;
        case 4:
            // 4. 后退 2m, 绝对位置控制
            detection_mode = 0; // 关闭检测程序
            workStageCout = "阶段 4 : ";
            workStateCout = "小车后退约 2m , 使小车左移能到达基地区";
            // 发送小车前进指令
            // TODO: 根据底层数据格式修改单位, 不能忘了！！！！
            // TODO: 修改 DISTANCE_BACKWARDS
            txMoveMsg(0, X_SPEED, 0, Y_SPEED, 0, 0);
            // 保持爪子不动
            txClawBoardMsg(0, 0, 0, 0);
            // TODO: 到达指定位置, 满足 if 条件, 注意修改 if 条件
            if (1)
            {
                workState = 5;
                detection_mode = 0;
            }
            break;
        case 5:
            // 5. 向左移动一段距离, 绝对位置控制
            detection_mode = 0; // 关闭检测程序
            workStageCout = "阶段 5 : ";
            workStateCout = "小车向左移动一定距离 , 到达基地区边缘";
            // 发送小车前进指令
            // TODO: 根据底层数据格式修改单位, 不能忘了！！！！
            // TODO: 修改 DISTANCE_LEFTWARDS
            txMoveMsg(0, X_SPEED, 0, Y_SPEED, 0, 0);
            // 保持爪子不动
            txClawBoardMsg(0, 0, 0, 0);
            // TODO: 到达指定位置, 满足 if 条件, 注意修改 if 条件
            if (1)
            {
                workState = 6;
                detection_mode = 2;
            }
            break;
        case 6:
            // 6. 箭头检测定位基地区
            detection_mode = 2; // 箭头检测
            workStageCout = "阶段 6 : ";
            workStateCout = "箭头引导小车放盒子";

            // 发送小车前进指令
            // TODO: 根据底层数据格式修改单位, 不能忘了！！！！
            txMoveMsg(tx, X_SPEED, tz, Y_SPEED, ry, Z_SPEED);
            txClawBoardMsg(0, 0, 0, 0);
            // TODO: 根据线程中的标志位切换来修改 if 语句, 对照之前的程序
            if (tx < DISTANCE_RECTANGLE_TO_ARROW)
            {
                workState = 7;
                detection_mode = 0;
            }
            break;
        case 7:
            // 7. 小车过障碍
            detection_mode = 0; // 箭头检测
            workStageCout = "阶段 7 : ";
            workStateCout = "过障碍";
            // TODO: 根据底层程序修改
            txMoveClawBoardMsg.flags = flagsSet(txMoveClawBoardMsg.flags, CONTROL_PASS_OBSTACLE_BIT, 1);
            if (flagsRead(rxMoveClawBoardMsg.flags, READ_PASS_OBSTACLE_BIT))
            {
                workState = 8;
                detection_mode = 0;
            }
            break;
        case 8:
            // 8. 堆叠盒子
            workStageCout = "阶段 8 : ";
            workStateCout = "堆叠盒子";
            // TODO: 封装成一个函数
            if (pileBoxes())
            {
                workState = 9;
                detection_mode = 0;
            }

            break;
        case 9:
            // 9. 小车过障碍
            detection_mode = 0; // 箭头检测
            workStageCout = "阶段 9 : ";
            workStateCout = "过障碍";
            // TODO: 根据底层程序修改
            txMoveClawBoardMsg.flags = flagsSet(txMoveClawBoardMsg.flags, CONTROL_PASS_OBSTACLE_BIT, 1);
            if (flagsRead(rxMoveClawBoardMsg.flags, READ_PASS_OBSTACLE_BIT))
            {
                // TODO: 添加校准函数的位置
                workState = 10;
                detection_mode = 0;
            }
            break;
        case 10:
            // 10. 小车向右移动一段距离
            detection_mode = 0; // 关闭检测程序
            workStageCout = "阶段 10 : ";
            workStateCout = "小车向右移动一定距离 , 准备进行下一次抓取";
            // 发送小车前进指令
            // TODO: 根据底层数据格式修改单位, 不能忘了！！！！
            // TODO: 修改 DISTANCE_LEFTWARDS
            txMoveMsg(0, X_SPEED, 0, Y_SPEED, 0, 0);
            // 保持爪子不动
            txClawBoardMsg(0, 0, 0, 0);
            // TODO: 到达指定位置, 满足 if 条件, 注意修改 if 条件
            if (1)
            {
                workState = 1;
                detection_mode = 2;
                boxNum++;
            }
            break;
        default:
            break;
        }
        if (boxNum > MAX_GRASP_NUM)
        {
            break;
        }
    }
    missionEndTimeUs = currentTimeUs();
    cout << "任务用时: " << (missionEndTimeUs - missionStartTimeUs) * 1e-6 << endl;
    capture.closeStream(); // 关闭摄像头
    disconnect_serial();   // 关闭串口
    cout << "任务完成, 关闭程序!!!" << endl;
    return 0;
}
/* 急需确认项
TODO: TODO: TODO:

1. 小车坐标系的定义
2. 前进距离的单位 速度的单位 旋转角度的单位
3. 通信协议
4. 速度为 0 时, 保持当前状态不变
5. 绝对位置控制, 如何判断到达了指定位置? 以前使用 当前点和目标点的error作为条件
6. 绝对位置和相对位置如何切换
7. 小车过障碍在底层完成, 上层发送指令, 下层完成之后, 返回一个指令
*/

/* 逻辑流程

1. 矩形引导小车前进，直到realsense检测到的距离小于
2. 箭头引导小车前进，直到盒子完全进入小车(如何判断)
3. 抓取盒子
4. 后退 2m, 绝对位置控制
5. 向左移动一段距离, 绝对位置控制
6. 箭头检测定位基地区
7. 小车过障碍
8. 堆叠盒子
9. 小车过障碍
10. 小车向右移动一段距离

*/

/* 通信协议

1. 小车 x y 方向的移动距离和速度
2. 切换绝对位置和相对位置的标志位
3. 滑台的位置和速度
4. 过障碍的标志位
5. 推板的位置和速度

*/

/* 视觉引导程序

1. 矩形检测 旋转量 前后 左右偏移量
2. 箭头检测 左右偏移量 质心位置 质心距离 realsense 的距离

*/

/* 里程计校准

1. 抓取盒子时，进行校准，记录盒子个数
2. 放完盒子之后，进行校准

*/