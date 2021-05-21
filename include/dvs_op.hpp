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
    DVSOperator(cv::UMat* _src, cv::UMat* _diff, 
                cv::UMat* _ref, cv::UMat* _thr, cv::UMat* _ev,
                float _relax, float _up, float _down);
    void init(cv::UMat* _src, cv::UMat* _diff, 
              cv::UMat* _ref, cv::UMat* _thr, cv::UMat* _ev,
              const float _relax, const float _up, const float _down);
    void operator()(const cv::Range& range) const;

private:
    cv::UMat* src;
    cv::UMat* diff;
    cv::UMat* ref; 
    cv::UMat* thr; 
    cv::UMat* ev;
    float relax;
    float up;
    float down;

};

#endif // DVS_OP_HPP