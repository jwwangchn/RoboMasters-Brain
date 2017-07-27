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

// extra
#include "serial.h"
#include "mythread.hpp"

#include "RMVideoCapture.hpp"
#include "Markdetection.h"

// 变量区
int workState = 0;  //工作状态





#endif