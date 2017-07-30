#ifndef MAIN_H
#define MAIN_H

// OpenCV
#include <opencv2/opencv.hpp>

// realsense
#include <librealsense/rs.hpp>

// C++ 标准库
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <math.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>  
#include <sys/types.h>  

// extra
#include "serial.h"
#include "mythread.hpp"

#include "RMVideoCapture.hpp"
#include "Markdetection.h"

using namespace std;
using namespace cv;
using namespace rs;

/*************************************************************************
*  
*  
*  宏定义参数区
*  
*  
*************************************************************************/

// 1. 速度控制区
// 基本移动速度
#define X_SPEED 0
#define Y_SPEED 0
#define Z_SPEED 0

#define CLAW_SPEED 0
#define BOARD_SPEED 0


// 2. 距离控制区
// 矩形检测切换到箭头检测的距离
#define DISTANCE_RECTANGLE_TO_ARROW 0

// 小车后退的距离, 应该设定成 2m 左右
#define DISTANCE_BACKWARDS 0   

// 小车左移的距离, 根据盒子数量改变
#define DISTANCE_LEFTWARDS 0 

// 3. 位置控制区
// 爪子上升到最高点
#define CLAW_MAX_UP 0



// 4. 标志位 flags 置位区
// 发送过障碍命令的标志位位数 TODO: 根据底层程序修改
#define CONTROL_PASS_OBSTACLE_BIT 0

// 接收过障碍完成标志位的位数
#define READ_PASS_OBSTACLE_BIT 0


// 5. 其他
// 最大抓取盒子数量
#define MAX_GRASP_NUM 0

#define TIMEOUT 30  // UART 通信 TIMEOUT

#define CONST_ONE 0X0001

// 6. 光学相机和RealSense切换
// #define _USE_REALSENSE
// #define _REALSENSE_SHOW
#define _USE_LIGHT_CAMERA

// 6. 视觉相关
// 摄像头与小车轴心的固定偏移
#define DIFFCONST 101
/*************************************************************************
*  
*  
*  变量区
*  
*  
*************************#include ************************************************/
// 任务计时
double missionStartTimeUs = 0;
double missionEndTimeUs = 0;

int workState = 1;      // 工作状态
string workStageCout = "状态 0 : ";   // 工作阶段打印(cout)
string workStateCout = "未执行主逻辑";   // 工作状态打印(cout)
int boxNum = 1;         // 盒子数量
int exit_flag = 0;
volatile int coutLogicFlag = 1;

// 视觉检测部分的变量
volatile int8_t detection_mode = 0;
extern double ry, rz, rx;
extern double tx, ty, tz;

// 2. 标志位区
bool finishDetectBoxFlag_GrabBox = false;       // 矩形引导完成检测盒子
bool finishDetectBoxFlag_PutBox = false;         // 矩形引导完成放盒子
bool finishDetectArrowFlag_GrabBox = false;     // 箭头检测完成抓盒子
bool finishDetectArrowFlag_PutBox = false;      // 箭头检测完成放盒子

// 3. RealSense 区
extern context _rs_ctx;
extern device *_rs_camera;
extern intrinsics _depth_intrin;
extern intrinsics _color_intrin;
extern bool _loop;

// Window size and frame rate
extern int const INPUT_WIDTH;
extern int const INPUT_HEIGHT;
extern int const FRAMERATE;

// Named windows
extern char *const WINDOW_DEPTH;
extern char *const WINDOW_RGB;
extern char *const WINDOW_DEPTH_RGB;


/*************************************************************************
*  
*  
*  自定义变量
*  
*  
*************************************************************************/
// UART 通信数据类型
typedef struct
{
    // 数据头
    int16_t head;
    
    uint16_t flags;               // 各种状态标志位
    // 小车的三个轴
    int16_t positionX;
    int16_t speedX;
    int16_t positionY;
    int16_t speedY;
    int16_t positionZ;
    int16_t speedZ;
    
    // 爪子和挡板
    int16_t clawOpenClose;      // 爪子合拢和张开
    int16_t clawUpDown;       // 爪子升高的位置
    //int16_t speedClaw;          // 爪子上升的速度

    // TODO: 挡板只有推和未推两种状态? 可以精简
    int16_t boardForwardBack;      // 挡板的位置
    // int16_t speedBoard;         // 挡板的速度

    int16_t end;
}MoveClawBoardMsgStruct;

// UART 通信数据
MoveClawBoardMsgStruct txMoveClawBoardMsg;  //发送的数据
MoveClawBoardMsgStruct rxMoveClawBoardMsg;  //接收的数据



/*************************************************************************
*  
*  
*  函数声明
*  
*  
*************************************************************************/
bool graspBoxes();
bool pileBoxes();
extern bool initialize_streaming();
extern void setup_windows();

#endif