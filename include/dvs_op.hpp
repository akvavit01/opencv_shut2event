#ifndef DVS_OP_HPP
#define DVS_OP_HPP

#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class DVSOperator: public cv::ParallelLoopBody
{
public:
    DVSOperator();
    DVSOperator(cv::Mat* _src, cv::Mat* _diff, 
                cv::Mat* _ref, cv::Mat* _thr, cv::Mat* _ev,
                float _relax, float _up, float _down);
    void init(cv::Mat* _src, cv::Mat* _diff, 
              cv::Mat* _ref, cv::Mat* _thr, cv::Mat* _ev,
              const float _relax, const float _up, const float _down);
    void operator()(const cv::Range& range) const;

private:
    cv::Mat* src;
    cv::Mat* diff;
    cv::Mat* ref; 
    cv::Mat* thr; 
    cv::Mat* ev;
    float relax;
    float up;
    float down;

};

#endif // DVS_OP_HPP