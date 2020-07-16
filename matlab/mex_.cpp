#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "mex.h"

#define MEX_ARGS int nlhs, mxArray **plhs, int nrhs, const mxArray **prhs

// Do CHECK and throw a Mex error if check fails
inline void mxCHECK(bool expr, const char* msg) {
    if (!expr) {
        mexErrMsgTxt(msg);
    }
}
inline void mxERROR(const char* msg) { mexErrMsgTxt(msg); }

// Check if a file exists and can be opened
void mxCHECK_FILE_EXIST(const char* file) {
    std::ifstream f(file);
    if (!f.good()) {
        f.close();
        std::string msg("Could not open file ");
        msg += file;
        mxERROR(msg.c_str());
    }
    f.close();
}

// mxArray to BGR image
static void mxArray2BGRImage(const mxArray * array, cv::Mat& image) 
{
    int ndims = mxGetNumberOfDimensions(array);
    const unsigned long int *dims = mxGetDimensions(array);

    // mxArray size
    int height = dims[0]; 
    int width = dims[1];
    int channel = dims[2];
    mxCHECK(mxIsUint8(array), "The image isn't uint8 format!");
    mxCHECK(ndims == 3 && channel == 3, "The image haven't RGB channels!");

    uchar * pData = reinterpret_cast<uchar *>(mxGetData(array));
    cv::Mat proxy(channel, height * width, CV_8UC1, pData);
    // printMatInt(proxy.colRange(0, 10));
    // mexPrintf("proxy end");

    std::vector<cv::Mat> channels; // store rgb channels
    // it exchanges row and col because data store col first int matlab by using reshape
    channels.push_back(proxy.row(2).reshape(0, width)); // blue channel
    channels.push_back(proxy.row(1).reshape(0, width)); // green channel
    channels.push_back(proxy.row(0).reshape(0, width)); // red channel
    
    // merge channels
    cv::merge(channels, image); // merge channels
    cv::transpose(image, image);   
}

// mxArray to cv::Mat
static void mxArray2Mat(const mxArray * array, cv::Mat& output) 
{
    mxCHECK(mxIsDouble, "The input mxArray isn't double type!");
    
    double * pData = mxGetPr(array);
    
    // it exchanges row and col because data store col first int matlab
    int rows = mxGetN(array);
    int cols = mxGetM(array);

    // copy data
    cv::Mat tmp(rows, cols, CV_64FC1, pData);
    tmp.convertTo(output, CV_32FC1);

    // cv::transpose(tmp, output);
}

// cv::Mat to mxArray
static mxArray * mat2mxArray(cv::Mat& mat)
{
    int rows = mat.rows;
    int cols = mat.cols;

    // it exchanges row and col because data store col first int matlab
    mxArray * array = mxCreateDoubleMatrix(cols, rows, mxREAL);
    double * pData = mxGetPr(array);
    
    // copy data
    cv::Mat tmp(rows, cols, CV_64FC1, pData);
    mat.convertTo(tmp, CV_64FC1);

    return array;
}

// // cv::Mat to std::vector
// template<typename _Tp>
// static std::vector<_Tp> mat2Vector(const cv::Mat &mat)
// {
// 	return (std::vector<_Tp>)(mat.reshape(1, 1)); //reshape to one row
// }
 
// /****************** vector转Mat *********************/
// template<typename _Tp>
// cv::Mat convertVector2Mat(vector<_Tp> v, int channels, int rows)
// {
// 	cv::Mat mat = cv::Mat(v);//将vector变成单列的mat
// 	cv::Mat dest = mat.reshape(channels, rows).clone();//PS：必须clone()一份，否则返回出错
// 	return dest;
// }

/* =========== net function =========== */

// Usage: geomapnet_('version')
static void version(MEX_ARGS) 
{
    mxCHECK(nrhs == 0, "Usage: geomapnet_('version')");
    // Return version string
    plhs[0] = mxCreateString("1.0");
}

/** -----------------------------------------------------------------
 ** Available commands.
 **/
struct handler_registry {
    std::string cmd;
    void (*func)(MEX_ARGS);
};

static handler_registry handlers[] = {
    // Public API functions
    { "version",                    version},
    // The end.
    { "END",                        NULL},
};

/** -----------------------------------------------------------------
 ** matlab entry point.
 **/
// Usage: geomapnet_(api_command, arg1, arg2, ...)
void mexFunction(MEX_ARGS) 
{
    // mexLock();  // Avoid clearing the mex file.
    mxCHECK(nrhs > 0, "Usage: geomapnet_(api_command, arg1, arg2, ...)");
    // Handle input command
    char* cmd = mxArrayToString(prhs[0]);
    
    std::ostringstream error_msg;
    bool dispatched = false;
    // Dispatch to cmd handler
    for (int i = 0; handlers[i].func != NULL; i++) 
    {
       
        error_msg << handlers[i].cmd << std::endl;
        if (handlers[i].cmd.compare(cmd) == 0) 
        {
            handlers[i].func(nlhs, plhs, nrhs-1, prhs+1);
            dispatched = true;
            break;
        }
    }
    if (!dispatched) {
        // std::ostringstream error_msg;
        error_msg << "Unknown command '" << cmd << "'";
        mxERROR(error_msg.str().c_str());
    }
    mxFree(cmd);
}