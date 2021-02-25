#include "main.hpp"

int main(int argc, char *argv[])
{
    enum Errors
    {
        NO_ERROR,
        UNREADABLE_VIDEO
    };

    // CLI argument parser keys
    const std::string keys{ "{h help usage ?        |                   | show help message             }"
                            "{vid-name              | /dev/video0       | link of video stream          }"
                            "{show-raw-frame        |                   | show conventional frame       }"
                            "{show-event-frame      |                   | show event frame              }"
                            "{write-fps             |                   | show fps count on raw frame   }" };

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
            std::cout << "Link or path to video stream. Can be live stream or video recording.\n";
        }

        // Details for flag on showing conventional frames
        else if (   args.get<std::string>("h")     == "show-event-frame"    ||
                    args.get<std::string>("?")     == "show-event-frame"    ||
                    args.get<std::string>("help")  == "show-event-frame"    ||
                    args.get<std::string>("usage") == "show-event-frame"    )
        {
            std::cout << "Toggle to show conventional frame streams.\n";
        }

        // Details for flag on showing conventional frames
        else if (   args.get<std::string>("h")     == "show-raw-frame"      ||
                    args.get<std::string>("?")     == "show-raw-frame"      ||
                    args.get<std::string>("help")  == "show-raw-frame"      ||
                    args.get<std::string>("usage") == "show-raw-frame"      )
        {
            std::cout << "Toggle to show event frame streams.\n";
        }

        // Details for flag on showing fps count
        else if (   args.get<std::string>("h")     == "write-fps"   ||
                    args.get<std::string>("?")     == "write-fps"   ||
                    args.get<std::string>("help")  == "write-fps"   ||
                    args.get<std::string>("usage") == "write-fps"   )
        {
            std::cout << "Toggle to show event fFPS count.\n";
        }

        // Showing general usage instructions
        args.printMessage();

        return NO_ERROR;
    }

    // Capturing CLI arguments value
    const std::string vidName   { args.get<std::string>("vid-name") }; // video stream link
    const bool showRawFrame     { args.has("show-raw-frame") }; // show conv frame or not
    const bool showEventFrame   { args.has("show-event-frame") }; // show event frame or not
    const bool showFPSCount     { args.has("write-fps") }; // show fps count

    // Checking raw stream
    cv::VideoCapture rawStream(vidName);
    if (!rawStream.isOpened())
    {
        std::cerr << "Video stream unreadable\n";
        return UNREADABLE_VIDEO;
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
    }

    // For storing frames
    cv::UMat prevRawFrame;
    cv::UMat currRawFrame;
    cv::UMat eventFrame;

    // Initialize raw frame
    rawStream >> prevRawFrame;

    // Show streams on windows
    for (   char keyPressed{ static_cast<char>( cv::waitKey(1) ) }; 
            keyPressed != 27; 
            keyPressed = static_cast<char>( cv::waitKey(1) ) )
    {
        // Update raw frames
        rawStream >> currRawFrame;

        // Checking if video stream is still open
        if ( currRawFrame.empty() || prevRawFrame.empty() )
        {
            std::cerr << "--- Video unaccessible ---\n";
            return UNREADABLE_VIDEO;
        }

        // Displaying
        if (showRawFrame)
        {
            if (showFPSCount)
            {
                cv::putText(currRawFrame,
                            std::to_string(rawStream.get(cv::CAP_PROP_FPS)) + " fps",
                            cv::Point(50, 50),
                            cv::FONT_HERSHEY_COMPLEX, 1,
                            (0, 0, 0),
                            2,
                            cv::LINE_4);
            }
            
            cv::imshow(rawStreamWinName, currRawFrame);
        }
        if (showEventFrame)
        {
            //cv::subtract(currRawFrame, prevRawFrame, eventFrame); // use absdiff instead, as all negative pixel values will be converted into zero
            cv::absdiff(currRawFrame, prevRawFrame, eventFrame);
            cv::imshow(eventStreamWinName, eventFrame);

            currRawFrame.copyTo(prevRawFrame);
        }
    }

    return NO_ERROR;
}