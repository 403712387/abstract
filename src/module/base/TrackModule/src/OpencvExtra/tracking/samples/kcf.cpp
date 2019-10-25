/*----------------------------------------------
 * Usage:
 * example_tracking_kcf <video_name>
 *
 * example:
 * example_tracking_kcf Bolt/img/%04.jpg
 * example_tracking_kcf faceocc2.webm
 *--------------------------------------------------*/

#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <cstring>
#include "samples_utility.hpp"
#include "Common.h"

#include "trackingSample.h"

using namespace std;
using namespace cv;

int KCFTest( int argc, char** argv ){
  // show help
  if(argc<2){
    cout<<
      " Usage: example_tracking_kcf <video_name>\n"
      " examples:\n"
      " example_tracking_kcf Bolt/img/%04.jpg\n"
      " example_tracking_kcf faceocc2.webm\n"
      << endl;
    return 0;
  }

  // create the tracker
  Ptr<Tracker> tracker = TrackerKCF::create();

  // set input video
  std::string video = argv[1];
  VideoCapture cap(video);

  Mat frame;

  // get bounding box
  cap >> frame;
#if 0
  Rect2d roi= selectROI("tracker", frame, true, false);

  //quit if ROI was not selected
  if(roi.width==0 || roi.height==0)
    return 0;
#else
  Rect2d roi = {827,553,40,30};
#endif
  // initialize the tracker
  tracker->init(frame,roi);

  // do the tracking
  printf("Start the tracking process, press ESC to quit.\n");
  for ( ;; ){
    // get frame from the video
    cap >> frame;

    // stop the program if no more images
    if(frame.rows==0 || frame.cols==0)
      break;

    // update the tracking result
    bool isfound = tracker->update(frame,roi);
    if(!isfound)
    {
        cout << "The target has been lost...\n";
        waitKey(0);
        return 0;
    }

    // draw the tracked object
    rectangle( frame, roi, Scalar( 255, 0, 0 ), 2, 1 );
#if 0
    // show image with the tracked object
    imshow("tracker",frame);

    //quit on ESC button
    if(waitKey(1)==27)break;
#else
    static int index = 0;
    const std::string outputPath = "trackResult";
    Common::createPath(outputPath);
    std::string outputFile = outputPath + "/" + std::to_string(index++) + ".jpg";
    std::vector<int> param;
    param.push_back(cv::IMWRITE_JPEG_QUALITY);
    param.push_back(85);
    imwrite(outputFile, frame, param);
#endif
  }

}
