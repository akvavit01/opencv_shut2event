#include "dvs_op.hpp"

// Constructor
DVSOperator::DVSOperator()
    : src(nullptr), diff(nullptr), ref(nullptr), thr(nullptr),
      ev(nullptr), relax(1.0f), up(1.0f), down(1.0f)
{

}
    
DVSOperator::DVSOperator(cv::Mat* _src, cv::Mat* _diff, 
                         cv::Mat* _ref, cv::Mat* _thr, cv::Mat* _ev,
                         float _relax, float _up, float _down)
    : src(_src), diff(_diff), ref(_ref), thr(_thr),
      relax(_relax), up(_up), down(_down)
{

}

// Init method
void DVSOperator::init(cv::Mat* _src, cv::Mat* _diff, 
                       cv::Mat* _ref, cv::Mat* _thr, cv::Mat* _ev,
                       const float _relax, const float _up, const float _down)
{
    std::cout << "In DVS_OP init function \n";
    src = _src;
    diff = _diff;
    ref = _ref; 
    thr = _thr;
    ev = _ev;
    relax = _relax; 
    up = _up;
    down = _down;
    std::cout << "relax "<< relax << " up " << up << " down " << down << '\n';
}

void DVSOperator::operator()(const cv::Range& range) const
{
    for (int row{range.start}; row < range.end; ++row) 
    {
        float const* it_src{src->ptr<float>(row)};
        float* it_diff{diff->ptr<float>(row)};
        float* it_ref{ref->ptr<float>(row)};
        float* it_thr{thr->ptr<float>(row)};

        for (int col(0); col < src->cols; ++col) 
        {
            (*it_diff) = (*it_src) - (*it_ref);
            bool test {(((*it_diff) < -(*it_thr)) || ((*it_diff) > (*it_thr)))};
            (*it_diff) = (*it_diff) * (static_cast<float>(test));
            (*it_ref) = (relax * (*it_ref)) + (*it_diff);
            
            if(test)
            {
                (*it_thr) = (*it_thr) * up;
            }
            else
            {
                (*it_thr) = (*it_thr) * down;
            }

            // Processing event frame
            cv::Vec3f color(0.0f, 0.0f, 0.0f);
            float val {diff->at<float>(row, col)};
            if(val > (*it_thr)) // negative event
            {
                color[2] = 0.0; // red
                color[1] = 0.0; // green
                color[0] = 1.0; // blue
            } 
            else if(val < -(*it_thr)) // positive event
            {
                color[2] = 1.0; // red
                color[1] = 0.0; // green
                color[0] = 0.0; // blue
            } 
            else // no event
            {
                color[2] = 0.0; // red
                color[1] = 0.0; // green
                color[0] = 0.0; // blue
            }
            ev->at<cv::Vec3f>(row, col) = color;
            
            //advance pointers
            ++it_src; 
            ++it_diff; 
            ++it_ref;
            ++it_thr; 
        }
    }
}