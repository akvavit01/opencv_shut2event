#include "main.hpp"

int main(int argc, char *argv[])
{
    // Showing OpenCV version
    std::cout << "OpenCV Version: " << cv::getVersionString() << '\n';

    enum Errors
    {
        NO_ERROR,
        UNREADABLE_VIDEO
    };

    // CLI argument parser keys
    const std::string keys{ "{h help usage ?        |                   | show help message                 }"
                            "{vid-name              | /dev/video0       | link of video stream              }"
                            "{show-raw-frame        |                   | show conventional frame           }"
                            "{show-event-frame      |                   | show event frame                  }"
                            "{write-fps             |                   | show fps count on raw frame       }"
                            "{write-fps-freq        | 1000              | how fast to print fps count in ms }"
                            "{max-ram               | 2.0               | max RAM used in GB                }"
                            "{ef-conv-meth          | absdiff           | conversion method                 }" 
                            "{frame-mode            | rgb               | image channels                    }"};

    cv::CommandLineParser args(argc, argv, keys);

    // Showing usage instruction
    if (args.has("h")       ||
        args.has("?")       ||
        args.has("help")    ||
        args.has("usage")   )
    {
        std::cout   << "---------------------------------------------------------------------------------"  << '\n'
                    << "- This is a test program to convert conventional camera frame into event frame. -"  << '\n'
                    << "- The code here will be ported into ROS2, for more flexibility and modularity.  -"  << '\n'
                    << "---------------------------------------------------------------------------------"  << '\n';

        // Showing help for each flag

        // Details for flag on video stream path
        if (args.get<std::string>("h")     == "vid-name"    ||
            args.get<std::string>("?")     == "vid-name"    ||
            args.get<std::string>("help")  == "vid-name"    ||
            args.get<std::string>("usage") == "vid-name"    )
        {
            std::cout << "Link or path to video stream. Can be live stream or video recording.\n\n";
        }

        // Details for flag on showing conventional frames
        else if (   args.get<std::string>("h")     == "show-event-frame"    ||
                    args.get<std::string>("?")     == "show-event-frame"    ||
                    args.get<std::string>("help")  == "show-event-frame"    ||
                    args.get<std::string>("usage") == "show-event-frame"    )
        {
            std::cout << "Toggle to show conventional frame streams.\n\n";
        }

        // Details for flag on showing conventional frames
        else if (   args.get<std::string>("h")     == "show-raw-frame"      ||
                    args.get<std::string>("?")     == "show-raw-frame"      ||
                    args.get<std::string>("help")  == "show-raw-frame"      ||
                    args.get<std::string>("usage") == "show-raw-frame"      )
        {
            std::cout << "Toggle to show event frame streams.\n\n";
        }

        // Details for flag on showing fps count
        else if (   args.get<std::string>("h")     == "write-fps"   ||
                    args.get<std::string>("?")     == "write-fps"   ||
                    args.get<std::string>("help")  == "write-fps"   ||
                    args.get<std::string>("usage") == "write-fps"   )
        {
            std::cout << "Toggle to show event FPS count.\n\n";
        }

        // Details for flag on showing fps count frequency
        else if (   args.get<std::string>("h")     == "write-fps-freq"  ||
                    args.get<std::string>("?")     == "write-fps-freq"  ||
                    args.get<std::string>("help")  == "write-fps-freq"  ||
                    args.get<std::string>("usage") == "write-fps-freq"  )
        {
            std::cout << "To set max RAM to be used.\n\n";
        }

        // Details for max RAM usage
        else if (   args.get<std::string>("h")     == "max-ram" ||
                    args.get<std::string>("?")     == "max-ram" ||
                    args.get<std::string>("help")  == "max-ram" ||
                    args.get<std::string>("usage") == "max-ram" )
        {
            std::cout << "To set FPS count frequency in ms.\n\n";
        }

        // Details on setting color change direction
        else if (   args.get<std::string>("h")      == "ef-conv-meth"   ||
                    args.get<std::string>("?")      == "ef-conv-meth"   ||
                    args.get<std::string>("help")   == "ef-conv-meth"   ||
                    args.get<std::string>("usage")  == "ef-conv-meth"   )
        {
            std::cout   << "Conversion method for producing event frame.\n"
                        << "Option: - absdiff: using absolute difference\n"
                        << "        - substract: using substract, negative pixel will be zero\n\n";
        }

        // Details on setting image color channel
        else if (   args.get<std::string>("h")      == "frame-mode" ||
                    args.get<std::string>("?")      == "frame-mode" ||
                    args.get<std::string>("help")   == "frame-mode" ||
                    args.get<std::string>("usage")  == "frame-mode" )
        {
            std::cout   << "Conversion method for producing event frame.\n"
                        << "Option: - rgb: using BGR color scheme\n"
                        << "        - grayscale: using grayscale color scheme\n\n";
        }

        // Showing general usage instructions
        args.printMessage();

        return NO_ERROR;
    }

    // Capturing CLI arguments value
    const std::string vidName       { args.get<std::string>("vid-name") }; // video stream link
    const bool showRawFrame         { args.has("show-raw-frame") }; // show conv frame or not
    const bool showEventFrame       { args.has("show-event-frame") }; // show event frame or not
    const bool showFPSCount         { args.has("write-fps") }; // show fps count
    const size_t showFPSCountPeriod { args.get<size_t>("write-fps-freq") }; // show fps count frequency
    const float maxRAM              { args.get<float>("max-ram") }; // max RAM to be used
    short int convMeth; // which method for producing event frame
    if ( args.get<std::string>("ef-conv-meth") == "absdiff" )
    {
        convMeth = 0;
    }
    else if ( args.get<std::string>("ef-conv-meth") == "substract" )
    {
        convMeth = 1;
    }
    short int frameMode; // which color scheme for processed frames
    if ( args.get<std::string>("frame-mode") == "rgb" )
    {
        frameMode = 0;
    }
    else if ( args.get<std::string>("frame-mode") == "grasyscale" )
    {
        frameMode = 1;
    }

    // Windows
    const std::string rawStreamWinName      {"Raw Stream"};
    const std::string eventStreamWinName    {"Event Frames Stream"};
    if (showRawFrame)
    {
        cv::namedWindow(rawStreamWinName, cv::WINDOW_OPENGL);
    }
    if (showEventFrame)
    {
        cv::namedWindow(eventStreamWinName, cv::WINDOW_OPENGL);
        //cv::namedWindow(eventStreamWinName + " Diff", cv::WINDOW_OPENGL);
    }

    // Checking raw stream
    cv::VideoCapture rawStream(vidName);
    if (!rawStream.isOpened())
    {
        std::cerr << "Video stream unreadable\n";
        return UNREADABLE_VIDEO;
    }
    std::cout << "Stream is starting...\n";

    // Initialize parallel processing pipeline // TO DO
    //tbb::concurrent_bounded_queue<ProcessingChainData*> frameQueue;
    //frameQueue.set_capacity(maxRAM);
    //auto pipelineRunner{ std::thread() };

    // For storing frames
    cv::UMat prevRawFrame;
    cv::UMat currRawFrame;
    cv::UMat eventFrame;

    // Initialize raw frame
    rawStream >> prevRawFrame;
    if (frameMode) // grayscale mode
    {
        cv::cvtColor(prevRawFrame, prevRawFrame, cv::COLOR_BGR2GRAY);
    }

    // Initialize fps counter time
    cv::TickMeter FPSTickMeter;
    if (showFPSCount)
    {
        FPSTickMeter.start();
    }

    // Show streams on windows
    while (true)
    {
        // Checking if stream is still opened
        if ( !rawStream.isOpened() )
        {
            std::cerr << "Video stream unreadable\n";
            return UNREADABLE_VIDEO;
        }

        // Checking if streaming has ended
        if ( static_cast<char>( cv::waitKey(1) ) == 27 )
        {
            std::cout   << "ESC Pressed\n"
                        << "Stream ending...\n";
            break;
        }

        // Update raw frames
        rawStream >> currRawFrame;
        if (frameMode) // grayscale mode
        {
            cv::cvtColor(currRawFrame, currRawFrame, cv::COLOR_BGR2GRAY);
        }

        // Checking if video stream is still open
        if ( currRawFrame.empty() || prevRawFrame.empty() )
        {
            std::cerr << "--- Video unaccessible ---\n";
            return UNREADABLE_VIDEO;
        }

        // Show FPS count
        if (showFPSCount)
        {
            /*cv::putText(currRawFrame,
                        std::to_string(rawStream.get(cv::CAP_PROP_FPS)) + " fps",
                        cv::Point(50, 50),
                        cv::FONT_HERSHEY_COMPLEX, 1,
                        (0, 0, 0),
                        2,
                        cv::LINE_4);*/ // write fps count on terminal instead
            
            FPSTickMeter.stop();
            //std::cout << FPSTickMeter.getTimeMilli() << '\n'; // for debugging purpose
            if ( FPSTickMeter.getTimeMilli() >= showFPSCountPeriod )
            {
                std::cout << "FPS Count: " << std::to_string(rawStream.get(cv::CAP_PROP_FPS)) << " fps\n";
                FPSTickMeter.reset();
            }
            FPSTickMeter.start();
        }

        // Displaying frames
        if (showRawFrame)
        {
            cv::imshow(rawStreamWinName, currRawFrame);
        }
        if (showEventFrame)
        {
            if (!convMeth) // absdiff
            {
                cv::absdiff(currRawFrame, prevRawFrame, eventFrame);
                cv::imshow(eventStreamWinName, eventFrame);
            }
            else if (convMeth == 1) // substract
            {
                cv::subtract(currRawFrame, prevRawFrame, eventFrame);
                cv::imshow(eventStreamWinName, eventFrame);
            }

            currRawFrame.copyTo(prevRawFrame);
        }
    }

    return NO_ERROR;
}