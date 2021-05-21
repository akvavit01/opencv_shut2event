#include "dvs_op.hpp"

// Constructor
DVSOperator::DVSOperator()
    : src(nullptr), diff(nullptr), ref(nullptr), thr(nullptr),
      ev(nullptr), relax(1.0f), up(1.0f), down(1.0f)
{

}
    
DVSOperator::DVSOperator(cv::UMat* _src, cv::UMat* _diff, 
                         cv::UMat* _ref, cv::UMat* _thr, cv::UMat* _ev,
                         float _relax, float _up, float _down)
    : src(_src), diff(_diff), ref(_ref), thr(_thr),
      ev(_ev), relax(_relax), up(_up), down(_down)
{

}

// Init method
void DVSOperator::init(cv::UMat* _src, cv::UMat* _diff, 
                       cv::UMat* _ref, cv::UMat* _thr, cv::UMat* _ev,
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
        float const* it_src{(*src).getMat(cv::ACCESS_RW).ptr<float>(row)};
        float* it_diff{(*diff).getMat(cv::ACCESS_RW).ptr<float>(row)};
        float* it_ref{(*ref).getMat(cv::ACCESS_RW).ptr<float>(row)};
        float* it_thr{(*thr).getMat(cv::ACCESS_RW).ptr<float>(row)};
        float* it_ev{(*ev).getMat(cv::ACCESS_RW).ptr<float>(row)};

        for (int col(0); col < src->cols; ++col) 
        {
            (*it_diff) = (*it_src) - (*it_ref);
            bool test {(((*it_diff) < -(*it_thr)) || ((*it_diff) > (*it_thr)))};
            (*it_diff) = (*it_diff) * ((float)test);
            (*it_ref) = (relax * (*it_ref)) + (*it_diff);
            
            if(test)
            {
                (*it_thr) = (*it_thr) * up;
            }
            else
            {
                (*it_thr) = (*it_thr) * down;
            }
            
            //advance pointers
            ++it_src; 
            ++it_diff; 
            ++it_ref;
            ++it_thr; 
            ++it_ev;
        }
    }
}
