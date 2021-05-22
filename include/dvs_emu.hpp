#ifndef DVS_EMU_HPP
#define DVS_EMU_HPP

#include <iostream>
#include <stdint.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "dvs_op.hpp"

class PyDVS{

public:
    PyDVS();
    PyDVS(const size_t w, const size_t h, const size_t fps=0);
    ~PyDVS();
    bool init(const int cam_id=0, const float thr=12.75f,
              const float relaxRate=1.0f, const float adaptUp=1.0f, 
              const float adaptDown=1.0f);
    bool init(const std::string& filename, const float thr=12.75f,
              const float relaxRate=1.0f, const float adaptUp=1.0f, 
              const float adaptDown=1.0f);
    bool init(const char* filename, const float thr=12.75f,
              const float relaxRate=1.0f, const float adaptUp=1.0f, 
              const float adaptDown=1.0f);

    void setFPS(const size_t fps);
    void setWidth(const size_t w);
    void setHeight(const size_t h);
    void setRelaxRate(const float r);
    void setAdaptUp(const float u);
    void setAdaptDown(const float d);

    size_t getFPS();
    size_t getWidth();
    size_t getHeight();
    float getRelaxRate();
    float getAdaptUp();
    float getAdaptDown();
    cv::Mat& getRaw();
    cv::Mat& getInput();
    cv::Mat& getReference();
    cv::Mat& getDifference();
    cv::Mat& getEvents();
    cv::Mat& getThreshold();

    bool update();
    void setAdapt(const float relaxRate, const float adaptUp, 
                  const float adaptDown, const float threshold);

private:
    cv::VideoCapture _cap;
    cv::Mat _in;
    cv::Mat _frame;
    cv::Mat _ref;
    cv::Mat _diff;
    cv::Mat _thr;
    cv::Mat _events;
    cv::Mat _gray;

    float _relaxRate;
    float _adaptUp;
    float _adaptDown;
    float _baseThresh;

    size_t _w, _h, _fps;
    bool _open;
    bool _is_vid;
    DVSOperator _dvsOp;

    void _get_size();
    void _get_fps();
    bool _set_size();
    bool _set_fps();
    void _initMatrices(const float thr_init=-1.0f);
};




#endif //DVS_EMU_HPP