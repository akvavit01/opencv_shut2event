#pragma once

#include <iostream> // for I/O stream

#include <opencv2/core.hpp> // core library
#include <opencv2/videoio.hpp> // for video I/O
#include <opencv2/highgui.hpp> // window I/O
#include <opencv2/core/utility.hpp> // for time utilities
#include <opencv2/imgproc.hpp> // for writing

// For multithreading // TO DO
/*#include <thread>
#include <tbb/pipeline.h>
#include <tbb/concurrent_queue.h>

// Container of images
struct ProcessingChainData
{
    cv::UMat rawFrame;
    cv::UMat eventFrame;
};*/