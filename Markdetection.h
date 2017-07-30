#ifndef MARKDETECTION_H
#define MARKDETECTION_H

//Image processing
#include "opencv2/opencv.hpp"
#define DIF_CEN 135
#define PROP 1.2
#define DIF_AN - 4.3
#define CX   343   //To change to calibration parameter.
#define CY   320;   //the same with cameraMatrix.cx,cy

// 箭头检测的各种阈值
#define THRESHOLD_C 200       //轮廓周长阈值（最小）
#define THRESHOLD_S 700       //轮廓面积阈值（最小）
#define THRESHOLD_LW_min 0.50 //最小正外接矩形长宽比最小值
#define THRESHOLD_LW_max 0.95 //最小正外接矩形长宽比最大值
#define THRESHOLD_B_R 10      //  矩形中心像素块BLUE-RED值
#define THRESHOLD_B_G 10      //  矩形中心像素块BLUE-GREEN值

// #define _SHOW_PHOTO
#define _ARROW_SHOW
#define _RECTANGLE_SHOW
//#define _SHOW_OUTPUT
//#define Camera_One
//#define Camera_Two
#define REALSENSE_CAMERA
using namespace cv;
class MyPoint
{
public:
    MyPoint(Point pnt)
    {
        x=pnt.x;
        y=pnt.y;
    };
    int x;
    int y;
    bool operator<(const MyPoint&p)const
    {
        return x<p.x;
    }
};

extern int senterX,senterY;        // 箭头检测质心位置

void findSquares( Mat src,const Mat& image, vector<vector<Point> >& squares );
void drawSquares( Mat& image, const vector<vector<Point> >& squares );
void LocationMarkes(const vector<vector<Point> >& squares);
void Calcu_attitude(Point3f world_pnt_tl,Point3f world_pnt_tr,Point3f world_pnt_br,Point3f world_pnt_bl,Point2f pnt_tl_src,Point2f pnt_tr_src,Point2f pnt_br_src,Point2f pnt_bl_src);
int Color_judge(Mat &src,int area);
void Sort_rect(vector<Point>& approx);
double angle( Point pt1, Point pt2, Point pt0 );
int Color_detect(Mat frame, int &diff_x, int &diff_y);
bool arrowDetect(Mat frame);

#endif
