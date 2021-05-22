// STL
#include <iostream> // for I/O stream

// OpenCV
#include <opencv2/core.hpp> // core library
#include <opencv2/videoio.hpp> // for video I/O
#include <opencv2/highgui.hpp> // window I/O
#include <opencv2/core/utility.hpp> // for time utilities
#include <opencv2/imgproc.hpp> // for writing

// pyDVS
#include "dvs_emu.hpp"

void diffToBGR(cv::Mat& bgr, const cv::Mat& gray, const cv::Mat& diff, const float thr)
{
    constexpr int B{0};
    constexpr int G{1};
    constexpr int R{2};

    for(size_t row{0}; row < gray.rows; ++row)
    {
        for(size_t col{0}; col < gray.cols; ++col)
        {
            cv::Vec3f color(0.0f, 0.0f, 0.0f);
            float val {diff.at<float>(row, col)};
            if(val > thr)
            {
                color[G] = 1.0;
            } 
            else if(val < -thr)
            {
                color[R] = 1.0;
            } 
            else 
            {
                val = gray.at<float>(row, col)/255.0f;
                color[R] = val;
                color[G] = val;
                color[B] = val;
            }
            bgr.at<cv::Vec3f>(row, col) = color;
        }
    }

}

int main(int argc, char *argv[])
{
    // Showing OpenCV version
    std::cout   << "OpenCV version : " << CV_VERSION << '\n'
                << "Major version : " << CV_MAJOR_VERSION << '\n'
                << "Minor version : " << CV_MINOR_VERSION << '\n'
                << "Subminor version : " << CV_SUBMINOR_VERSION << '\n';

    enum Errors
    {
        NO_ERROR,
        UNREADABLE_VIDEO
    };

    // CLI argument parser keys
    const std::string keys{ "{h help usage ?        |                   | show help message                 }"
                            "{vid-name              | /dev/video0       | link of video stream              }"
                            "{show-all-frame        |                   | show all frame                    }"
                            "{show-raw-frame        |                   | show raw frame                    }"
                            "{show-ref-frame        |                   | show ref frame                    }"
                            "{show-gray-frame       |                   | show grayscale frame              }"
                            "{show-event-frame      |                   | show event frame                  }"
                            "{show-diff-frame       |                   | show difference frame             }"
                            "{write-fps             |                   | show fps count on raw frame       }"
                            "{write-fps-freq        | 1000              | how fast to print fps count in ms }"
                            "{thr                   | 10.0              | pyDVS emulator threshold          }"
                            "{rel-rate              | 0.999             | pyDVS emulator relax rate         }"
                            "{adapt-up              | 1.5               | pyDVS emulator adapt up           }"
                            "{adapt-down            | 0.99              | pyDVS emulator adapt down         }" };

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

        // Details for flag on showing all frames
        else if (   args.get<std::string>("h")     == "show-all-frame"      ||
                    args.get<std::string>("?")     == "show-all-frame"      ||
                    args.get<std::string>("help")  == "show-all-frame"      ||
                    args.get<std::string>("usage") == "show-all-frame"      )
        {
            std::cout << "Toggle to show all frame streams.\n\n";
        }

        // Details for flag on showing event frames
        else if (   args.get<std::string>("h")     == "show-event-frame"    ||
                    args.get<std::string>("?")     == "show-event-frame"    ||
                    args.get<std::string>("help")  == "show-event-frame"    ||
                    args.get<std::string>("usage") == "show-event-frame"    )
        {
            std::cout << "Toggle to show event frame streams.\n\n";
        }

        // Details for flag on showing raw frames
        else if (   args.get<std::string>("h")     == "show-raw-frame"      ||
                    args.get<std::string>("?")     == "show-raw-frame"      ||
                    args.get<std::string>("help")  == "show-raw-frame"      ||
                    args.get<std::string>("usage") == "show-raw-frame"      )
        {
            std::cout << "Toggle to show raw frame streams.\n\n";
        }

        // Details for flag on showing reference frames
        else if (   args.get<std::string>("h")     == "show-ref-frame"      ||
                    args.get<std::string>("?")     == "show-ref-frame"      ||
                    args.get<std::string>("help")  == "show-ref-frame"      ||
                    args.get<std::string>("usage") == "show-ref-frame"      )
        {
            std::cout << "Toggle to show reference frame streams.\n\n";
        }

        // Details for flag on showing grayscale frames
        else if (   args.get<std::string>("h")     == "show-gray-frame"      ||
                    args.get<std::string>("?")     == "show-gray-frame"      ||
                    args.get<std::string>("help")  == "show-gray-frame"      ||
                    args.get<std::string>("usage") == "show-gray-frame"      )
        {
            std::cout << "Toggle to show grayscale frame streams.\n\n";
        }

        // Details for flag on showing difference frames
        else if (   args.get<std::string>("h")     == "show-diff-frame"      ||
                    args.get<std::string>("?")     == "show-diff-frame"      ||
                    args.get<std::string>("help")  == "show-diff-frame"      ||
                    args.get<std::string>("usage") == "show-diff-frame"      )
        {
            std::cout << "Toggle to show difference frame streams.\n\n";
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
            std::cout << "To show how fast FPS count will be shown.\n\n";
        }

        // Details for flag on setting pyDVS threshold value
        else if (   args.get<std::string>("h")     == "thr" ||
                    args.get<std::string>("?")     == "thr" ||
                    args.get<std::string>("help")  == "thr" ||
                    args.get<std::string>("usage") == "thr" )
        {
            std::cout << "To set threshold value for pyDVS processing.\n\n";
        }

        // Details for flag on setting pyDVS relax rate
        else if (   args.get<std::string>("h")     == "rel-rate"    ||
                    args.get<std::string>("?")     == "rel-rate"    ||
                    args.get<std::string>("help")  == "rel-rate"    ||
                    args.get<std::string>("usage") == "rel-rate"    )
        {
            std::cout << "To set relax rate value for pyDVS processing.\n\n";
        }

        // Details for flag on setting pyDVS adapt up
        else if (   args.get<std::string>("h")     == "adapt-up"    ||
                    args.get<std::string>("?")     == "adapt-up"    ||
                    args.get<std::string>("help")  == "adapt-up"    ||
                    args.get<std::string>("usage") == "adapt-up"    )
        {
            std::cout << "To set adapt up value for pyDVS processing.\n\n";
        }

        // Details for flag on setting pyDVS adapt up
        else if (   args.get<std::string>("h")     == "adapt-down"  ||
                    args.get<std::string>("?")     == "adapt-down"  ||
                    args.get<std::string>("help")  == "adapt-down"  ||
                    args.get<std::string>("usage") == "adapt-down"  )
        {
            std::cout << "To set adapt down value for pyDVS processing.\n\n";
        }

        // Showing general usage instructions
        args.printMessage();

        return NO_ERROR;
    }

    // Capturing CLI arguments value
    const std::string vidName       { args.get<std::string>("vid-name") }; // video stream link
    const bool showAllFrame         { args.has("show-all-frame") }; // show all frame
    bool showRawFrame               { args.has("show-raw-frame") }; // show raw frame or not
    bool showRefFrame               { args.has("show-ref-frame") }; // show raw frame or not
    bool showGrayFrame              { args.has("show-gray-frame") }; // show grayscale frame or not
    bool showDiffFrame              { args.has("show-diff-frame") }; // show difference frame or not
    bool showEventFrame             { args.has("show-event-frame") }; // show event frame or not
    const bool showFPSCount         { args.has("write-fps") }; // show fps count
    const size_t showFPSCountPeriod { args.get<size_t>("write-fps-freq") }; // show fps count frequency
    const float thr                 { args.get<float>("thr") }; // threshold value for pyDVS processing
    const float relRate             { args.get<float>("rel-rate") }; // relax rate value for pyDVS processing
    const float adaptUp             { args.get<float>("adapt-up") }; // adapt up value for pyDVS processing
    const float adaptDown           { args.get<float>("adapt-down") }; // adapt down value for pyDVS processing

    if (showAllFrame)
    {
        showRawFrame = true;
        showRefFrame = true;
        showGrayFrame = true;
        showDiffFrame = true;
        showEventFrame = true;
    }

    // PyDVS object
    PyDVS DVS;

    // Check video stream
    bool ok { DVS.init(vidName, thr, relRate, adaptUp, adaptDown) };
    if(!ok)
    {
        std::cerr << "Unable to open video source.\n";
        return UNREADABLE_VIDEO;
    }
    std::cout << "Stream is starting...\n";

    // Windows
    const std::string rawStreamWinName      {"Raw Stream"};
    const std::string refStreamWinName      {"Reference Stream"};
    const std::string grayStreamWinName     {"Grayscale Stream"};
    const std::string diffStreamWinName     {"Difference Stream"};
    const std::string eventStreamWinName    {"Event Frames Stream"};
    if (showRawFrame)
    {
        cv::namedWindow(rawStreamWinName, cv::WINDOW_OPENGL);
    }
    if (showRefFrame)
    {
        cv::namedWindow(refStreamWinName, cv::WINDOW_OPENGL);
    }
    if (showGrayFrame)
    {
        cv::namedWindow(grayStreamWinName, cv::WINDOW_OPENGL);
    }
    if (showDiffFrame)
    {
        cv::namedWindow(diffStreamWinName, cv::WINDOW_OPENGL);
    }
    if (showEventFrame)
    {
        cv::namedWindow(eventStreamWinName, cv::WINDOW_OPENGL);
    }

    // Initialize fps counter time
    cv::TickMeter FPSTickMeter;
    if (showFPSCount)
    {
        FPSTickMeter.start();
    }

    // Set DVS parameters
    DVS.setAdapt(relRate, adaptUp, adaptDown, thr);
    std::cout   << "Relax rate = " << DVS.getRelaxRate() << '\n'
                << "Adapt up = " << DVS.getAdaptUp() << '\n'
                << "Adapt down = " << DVS.getAdaptDown() << '\n';

    // UMat objects for storing frames
    size_t w {DVS.getWidth()};
    size_t h {DVS.getHeight()};
    cv::Mat frame(h, w, CV_32FC3); // raw frame
    cv::Mat fullGray(h, w, CV_32F); // grayscale frame
    cv::Mat fullRef(h, w, CV_32F); // reference frame
    cv::Mat fullDiff(h, w, CV_32F); // difference frame
    cv::Mat fullOut(h, w, CV_32FC3); // output frame

    // Show frames
    for(; ok; ok = DVS.update())
    {
        // Show FPS count
        if (showFPSCount)
        {
            FPSTickMeter.stop();
            if ( FPSTickMeter.getTimeMilli() >= showFPSCountPeriod )
            {
                std::cout << "FPS Count: " << std::to_string(DVS.getFPS()) << "\n";
                FPSTickMeter.reset();
            }
            FPSTickMeter.start();
        }

        // Process to grayscale frame
        fullGray = DVS.getInput()*(1.0f/255.0f);

        // Reference frame
        fullRef = DVS.getReference()*(1.0f/255.0f);

        // Process to difference frame
        fullDiff = DVS.getDifference()*(1.0f/255.0f);

        // Event frame
        diffToBGR(frame, DVS.getInput(), DVS.getDifference(), 0.0);
        frame.copyTo(fullOut);


        // Displaying frames
        if (showRawFrame)
        {

        }
        if (showRefFrame)
        {
            cv::imshow(refStreamWinName, fullRef);
        }
        if (showGrayFrame)
        {
            cv::imshow(grayStreamWinName, fullGray);
        }
        if (showDiffFrame)
        {
            cv::imshow(diffStreamWinName, fullDiff);
        }
        if (showEventFrame)
        {
            cv::imshow(eventStreamWinName, fullOut);
        }

        // Check if stream has ended
        char c {(static_cast<char>(cv::pollKey()))};
        if(c==27 || c == 'q' || c == 'Q')
        {
            std::cout   << "ESC Pressed\n"
                        << "Stream ending...\n";
            break;
        }
    }

    return NO_ERROR;
}