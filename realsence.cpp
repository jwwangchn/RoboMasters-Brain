#include "realsense.h"

using namespace std;
using namespace cv;
using namespace rs;

context _rs_ctx;
device *_rs_camera = NULL;
intrinsics _depth_intrin;
intrinsics _color_intrin;
bool _loop = true;

// Window size and frame rate
int const INPUT_WIDTH = 640;
int const INPUT_HEIGHT = 480;
int const FRAMERATE = 30;

// Named windows
char *const WINDOW_DEPTH = "Depth Image";
char *const WINDOW_RGB = "RGB Image";
char *const WINDOW_DEPTH_RGB = "DEPTH RGB Image";


// Initialize the application state. Upon success will return the static app_state vars address
bool initialize_streaming()
{
    bool success = false;
    if (_rs_ctx.get_device_count() > 0)
    {
        _rs_camera = _rs_ctx.get_device(0);

        _rs_camera->enable_stream(rs::stream::color, INPUT_WIDTH, INPUT_HEIGHT, rs::format::rgb8, FRAMERATE);
        _rs_camera->enable_stream(rs::stream::depth, INPUT_WIDTH, INPUT_HEIGHT, rs::format::z16, FRAMERATE);

        _rs_camera->start();

        success = true;
    }
    return success;
}


/////////////////////////////////////////////////////////////////////////////
// Create the depth and RGB windows, set their mouse callbacks.
// Required if we want to create a window and have the ability to use it in
// different functions
/////////////////////////////////////////////////////////////////////////////
// void setup_windows()
// {
//     cv::namedWindow(WINDOW_DEPTH, WINDOW_AUTOSIZE);
//     cv::namedWindow(WINDOW_RGB, WINDOW_AUTOSIZE);
//     cv::namedWindow(WINDOW_DEPTH_RGB, WINDOW_AUTOSIZE);

//     cv::setMouseCallback(WINDOW_DEPTH, onMouse, WINDOW_DEPTH);
//     cv::setMouseCallback(WINDOW_RGB, onMouse, WINDOW_RGB);
//     cv::setMouseCallback(WINDOW_DEPTH_RGB, onMouse, WINDOW_RGB);
// }

/////////////////////////////////////////////////////////////////////////////
// Called every frame gets the data from streams and displays them using OpenCV.
/////////////////////////////////////////////////////////////////////////////
bool display_next_frame()
{
    // Get current frames intrinsic data.
    _depth_intrin = _rs_camera->get_stream_intrinsics(rs::stream::depth);
    _color_intrin = _rs_camera->get_stream_intrinsics(rs::stream::color);

    // Create depth image
    cv::Mat depth16(_depth_intrin.height, _depth_intrin.width, CV_16U, (uchar *)_rs_camera->get_frame_data(rs::stream::depth));

    // Create color image
    cv::Mat rgb(_color_intrin.height, _color_intrin.width, CV_8UC3, (uchar *)_rs_camera->get_frame_data(rs::stream::color));

    // Create color and depth image
    cv::Mat depth16_rgb(_color_intrin.height, _color_intrin.width, CV_8UC3, (uchar *)_rs_camera->get_frame_data(rs::stream::color_aligned_to_depth));

    // < 800
    cv::Mat depth8u = depth16;
    depth8u.convertTo(depth8u, CV_8UC1, 255.0 / 1000);

    // imshow(WINDOW_DEPTH, depth8u);
    // cvWaitKey(1);

    // cv::cvtColor(rgb, rgb, cv::COLOR_BGR2RGB);
    // imshow(WINDOW_RGB, rgb);
    // cvWaitKey(1);

    // cv::cvtColor(depth16_rgb, depth16_rgb, cv::COLOR_BGR2RGB);
    // imshow(WINDOW_DEPTH_RGB, depth16_rgb);
    // cvWaitKey(1);
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Get distance function
/////////////////////////////////////////////////////////////////////////////
float getDistance(rs::device *dev, int x, int y)
{
    cout << "ok1" << endl;
    uint16_t *depthImage = (uint16_t *)dev->get_frame_data(rs::stream::depth);
    cout << "ok2" << endl;
    float scale = dev->get_depth_scale();
    cout << "ok3" << endl;
    rs::intrinsics depthIntrin = dev->get_stream_intrinsics(rs::stream::depth);
    cout << "ok4" << endl;
    cout << y << " "<< depthIntrin.width << " " << x << endl;
    uint16_t depthValue = depthImage[y * depthIntrin.width + x];
    cout << "ok5" << endl;
    float depthInMeters = depthValue * scale;
    cout << "ok6" << endl;
    return depthInMeters;
}

pair<Mat, float> getDistanceMatrix(rs::device *dev)
{
    uint16_t *depthImage = (uint16_t *)dev->get_frame_data(rs::stream::depth);
    float scale = dev->get_depth_scale();
    rs::intrinsics depthIntrin = dev->get_stream_intrinsics(rs::stream::depth);
    cv::Mat depthValueMat = cv::Mat(depthIntrin.height, depthIntrin.width, CV_16U, depthImage);

    pair<Mat, float> result;
    result.first = depthValueMat;
    result.second = scale;
    // Mat depthValueFloat = (float)(depthValueMat * scale);
    return result;
}

/////////////////////////////////////////////////////////////////////////////
// Detect function
/////////////////////////////////////////////////////////////////////////////
void detectSquareBox(Mat srcImage, Mat distanceMatrix)
{
    int rowNum = distanceMatrix.rows;
    int colNum = distanceMatrix.cols;

    int blackPixel[rowNum];
    int lineAxis[rowNum];
    bool lineFlag = false;
    vector<Point2f> points;
    Vec4f verticalLine;
    points.clear();
    //从下网上扫描
    for (int row = rowNum - 1; row >= 50; row--)
    {
        //黑色像素
        blackPixel[row] = 0;
        //从左往右扫描
        for (int col = 0; col < colNum - 1; col++)
        {
            //检测相邻两个元素的像素值
            int pixelValueNow = distanceMatrix.at<uint16_t>(row, col);
            int pixelValueNext = distanceMatrix.at<uint16_t>(row, col + 1);

            //如果当前像素为盒子所在像素
            if (pixelValueNow > 400 && pixelValueNow < 700)
            {
                //黑色像素计数加一
                blackPixel[row]++;
                //如果黑色像素数量大于某个值，同时下一个像素值为白色像素，认为出现线段
                if (blackPixel[row] > 10 && pixelValueNext > 700)
                {
                    //出现线段
                    lineFlag = true;
                    //计算中心位置
                    lineAxis[row] = (col + col - blackPixel[row]) / 2;
                    //存储当前行的中心像素位置
                    points.push_back(Point2f(lineAxis[row], row));
                    //cout << lineAxis << endl;
                    break;
                }
            }
            else
            {
                lineFlag = false;
                blackPixel[row] = 0;
            }
        }
    }
    if (points.size() > 5)
    {
        fitLine(Mat(points), verticalLine, CV_DIST_L1, 0, 0.01, 0.01);
    }

    // double k = verticalLine[1] / verticalLine[0];
    // double step = 100;
    // cv::line(srcImage, cvPoint(verticalLine[2] - step, verticalLine[3] - k * step), cvPoint(verticalLine[2] + step, k * step + verticalLine[3]), Scalar(0, 0, 255), 5);
    // imshow("dstImage", srcImage);
    // cout << "是否检测到盒子: " << lineFlag << endl;
}

/////////////////////////////////////////////////////////////////////////////
// If the left mouse button was clicked on either image, stop streaming and close windows.
/////////////////////////////////////////////////////////////////////////////
static void onMouse(int event, int x, int y, int, void *window_name)
{
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        _loop = false;
    }
}