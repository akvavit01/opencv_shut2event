#include "dvs_emu.hpp"

// Constructor
PyDVS::PyDVS()
    : _relaxRate(1.0f), _adaptUp(1.0f), _adaptDown(1.0f),
      _baseThresh(12.0f), _w(0), _h(0), _fps(0), _open(false)
{

}

PyDVS::PyDVS(size_t w, size_t h, size_t fps)
{
    _w = w;
    _h = h;
    _fps = fps;
}

// Destructor
PyDVS::~PyDVS()
{
    if(_open)
    {
        _cap.release();
    }
}

// Init method
bool PyDVS::init(const int cam_id, const float thr, const float relaxRate, 
                 const float adaptUp, const float adaptDown)
{
    std::vector<int> vidParams;
    vidParams.push_back(cv::CAP_PROP_HW_ACCELERATION); // hardware acceleration
    vidParams.push_back(cv::VIDEO_ACCELERATION_ANY);

    _cap = cv::VideoCapture(cam_id, cv::CAP_ANY, vidParams);
    _open = _cap.isOpened();
    if(!_open)
    {
        std::cerr << "Error. Cannot open video feed!\n";
        return false;
    }
    
    bool success{true};
    _is_vid = false;
    if(_w == 0 || _h == 0 || _fps == 0)
    {
        _get_size();
        _get_fps();
    } 
    else 
    {
        success &= _set_size();
        success &= _set_fps();
    }

    setAdapt(relaxRate, adaptUp, adaptDown, thr);
    _initMatrices(thr);

    return success;
}

bool PyDVS::init(const std::string& filename, const float thr, const float relaxRate, 
                 const float adaptUp, const float adaptDown)
{
    std::vector<int> vidParams;
    vidParams.push_back(cv::CAP_PROP_HW_ACCELERATION); // hardware acceleration
    vidParams.push_back(cv::VIDEO_ACCELERATION_ANY);

    _cap = cv::VideoCapture(filename, cv::CAP_ANY, vidParams);
    _open = _cap.isOpened();
    if(!_open)
    {
        std::cerr << "Init. Cannot open video feed!\n";
        return false;
    }

    _is_vid = true;
    _get_size();
    _get_fps();

    // set output format as 32-bit floating point with a single channel
    _cap.set(cv::CAP_PROP_FORMAT, CV_32FC1);
    setAdapt(relaxRate, adaptUp, adaptDown, thr);
    _initMatrices(thr);

    return true;
}

bool PyDVS::init(const char* filename, const float thr, const float relaxRate, 
                 const float adaptUp, const float adaptDown)
{
    std::vector<int> vidParams;
    vidParams.push_back(cv::CAP_PROP_HW_ACCELERATION); // hardware acceleration
    vidParams.push_back(cv::VIDEO_ACCELERATION_ANY);

    _cap = cv::VideoCapture(filename, cv::CAP_ANY, vidParams);
    _open = _cap.isOpened();
    if(!_open)
    {
        std::cerr << "Init. Cannot open video feed!\n";
        return false;
    }

    _is_vid = true;
    _get_size();
    _get_fps();
    // set output format as 32-bit floating point with a single channel
    _cap.set(cv::CAP_PROP_FORMAT, CV_32FC1);
    setAdapt(relaxRate, adaptUp, adaptDown, thr);
    _initMatrices(thr);

    return true;
}

void PyDVS::_initMatrices(const float thr_init)
{
    // CV_32F 32-bit floating point numbers
    _gray  = cv::Mat::zeros(_h, _w, CV_8UC1);
    _in  = cv::Mat::zeros(_h, _w, CV_32F);
    _ref = cv::Mat::zeros(_h, _w, CV_32F);
    _diff = cv::Mat::zeros(_h, _w, CV_32F);
    _events = cv::Mat::zeros(_h, _w, CV_32F);

    std::cout << _relaxRate << "," << _adaptUp << "," << _adaptDown << '\n';
    if(thr_init > _baseThresh)
    {
        _baseThresh = thr_init;
    }
    _thr = _baseThresh * cv::Mat::ones(_h, _w, CV_32F);
    _dvsOp.init(&_in, &_diff, &_ref, &_thr,
                _relaxRate, _adaptUp, _adaptDown);

}

// Update frames from camera stream method
bool PyDVS::update()
{
    _cap >> _frame;
    if (_frame.empty())
    {
        return false;
    }

    cv::cvtColor(_frame, _gray, cv::COLOR_BGR2GRAY);
    _gray.convertTo(_in, CV_32F);
    cv::parallel_for_(cv::Range(0, _gray.rows), _dvsOp);
    
    return true;
}

// Set parameter methods
bool PyDVS::_set_size()
{
    if(_open)
    {
        size_t w = _cap.get(cv::CAP_PROP_FRAME_WIDTH);
        size_t h = _cap.get(cv::CAP_PROP_FRAME_HEIGHT);
        
        bool success {true};
        success &= _cap.set(cv::CAP_PROP_FRAME_WIDTH, _w);
        success &= _cap.set(cv::CAP_PROP_FRAME_HEIGHT, _h);

        if (!success)
        {
            _cap.set(cv::CAP_PROP_FRAME_WIDTH, w);
            _cap.set(cv::CAP_PROP_FRAME_HEIGHT, h);
            _w = w; 
            _h = h;
        }
        return success;
    }
    return false;
}

bool PyDVS::_set_fps()
{
    if(_open)
    {
        size_t fps = _cap.get(cv::CAP_PROP_FPS);
        
        bool success{true};
        success &= _cap.set(cv::CAP_PROP_FPS, _fps);
        if(!success)
        {
            _cap.set(cv::CAP_PROP_FPS, fps);
            _fps = fps;
        }
        return success;
    }
    return false;
}

void PyDVS::setFPS(const size_t fps)
{
    _fps = fps;
}

void PyDVS::setWidth(const size_t w)
{
    _w = w;
}

void PyDVS::setHeight(const size_t h)
{
    _h = h;
}

void PyDVS::setRelaxRate(const float r)
{
    _relaxRate = r;
}

void PyDVS::setAdaptUp(const float u)
{
    _adaptUp = u;
}

void PyDVS::setAdaptDown(const float d)
{
    _adaptDown = d;
}

void PyDVS::setAdapt(const float relaxRate, const float adaptUp, 
                     const float adaptDown, const float threshold)
{
    _relaxRate = relaxRate;
    _adaptUp = adaptUp;
    _adaptDown = adaptDown;
    _baseThresh = threshold;
}

// Get parameter methods
void PyDVS::_get_size()
{
    if(_open)
    {
        _w = _cap.get(cv::CAP_PROP_FRAME_WIDTH);
        _h = _cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    }
}

void PyDVS::_get_fps()
{
    if(_open)
    {
        _fps = _cap.get(cv::CAP_PROP_FPS);
    }
}

size_t PyDVS::getFPS()
{
    return _fps;
}

size_t PyDVS::getWidth()
{
    return _w;
}

size_t PyDVS::getHeight()
{
    return _h;
}

float PyDVS::getRelaxRate()
{
    return _relaxRate;
}

float PyDVS::getAdaptUp()
{
    return _adaptUp;
}

float PyDVS::getAdaptDown()
{
    return _adaptDown;
}

cv::Mat& PyDVS::getInput()
{
    return _in;
}

cv::Mat& PyDVS::getReference()
{
    return _ref;
}

cv::Mat& PyDVS::getDifference()
{
    return _diff;
}

cv::Mat& PyDVS::getEvents()
{
    return _events;
}

cv::Mat& PyDVS::getThreshold()
{
    return _thr;
}