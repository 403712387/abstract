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
#include "Log.h"
#include <QDateTime>

#include "trackingSample.h"

using namespace std;
using namespace cv;

int KCFTest( int argc, char** argv ){
    const std::string mClassName = "KCF";

  // create the tracker
  Ptr<Tracker> tracker = TrackerKCF::create();
#if 0
  // set input video
  std::string video = argv[1];
  VideoCapture cap(video);

  Mat frame;

  // get bounding box
  cap >> frame;
  Rect2d roi= selectROI("tracker", frame, true, false);

  //quit if ROI was not selected
  if(roi.width==0 || roi.height==0)
    return 0;
#else
  int index = 1;
  const std::string inputPath = "/data/mars/picdata/pedestrian/";
  Mat frame = cv::imread(inputPath + std::to_string(index) + ".jpg");
  Rect2d roi = {823,601,35,35};
#endif
  // initialize the tracker
  tracker->init(frame,roi);

  // do the tracking
  long long totalTime = 0;
  int imageCount = 0;
  printf("Start the tracking process, press ESC to quit.\n");
  for ( int i = 0; i < 3000; ++i ){
    // get frame from the video
    frame = cv::imread(inputPath + std::to_string(index) + ".jpg");

    // stop the program if no more images
    if(frame.rows==0 || frame.cols==0)
      break;

    QDateTime beginTime = QDateTime::currentDateTime();
    // update the tracking result
    bool isfound = tracker->update(frame,roi);
    QDateTime endTime = QDateTime::currentDateTime();
    if(!isfound)
    {
        LOG_I(mClassName, "The target has been lost...") ;
        break;
    }

    // 计算耗时
    totalTime += endTime.toMSecsSinceEpoch() - beginTime.toMSecsSinceEpoch();
    ++imageCount;

    // draw the tracked object
    rectangle( frame, roi, Scalar( 255, 0, 0 ), 2, 1 );
#if 0
    // show image with the tracked object
    imshow("tracker",frame);

    //quit on ESC button
    if(waitKey(1)==27)break;
#else
    const std::string outputPath = "trackResult";
    Common::createPath(outputPath);
    std::string outputFile = outputPath + "/" + std::to_string(index++) + ".jpg";
    std::vector<int> param;
    param.push_back(cv::IMWRITE_JPEG_QUALITY);
    param.push_back(85);
    imwrite(outputFile, frame, param);
#endif
  }
  if (imageCount > 0)
  {
    LOG_I(mClassName, "total image:" << imageCount << ", total spend time:" << totalTime << "ms, avg:" << totalTime/imageCount << "ms");
  }
  else
  {
      LOG_I(mClassName, "fuck ");
  }
  return 0;
}
