#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <cstring>
#include "samples_utility.hpp"

#include "trackingSample.h"
#include "Common.h"
#include "Log.h"
#include <QDateTime>

using namespace std;
using namespace cv;

static const char* keys =
{ "{@tracker_algorithm | | Tracker algorithm }"
    "{@video_name      | | video name        }"
    "{@start_frame     |0| Start frame       }"
    "{@bounding_frame  |0,0,0,0| Initial bounding frame}"};

static void help()
{
  cout << "\nThis example shows the functionality of \"Long-term optical tracking API\""
       "-- pause video [p] and draw a bounding box around the target to start the tracker\n"
       "Example of <video_name> is in opencv_extra/testdata/cv/tracking/\n"
       "Call:\n"
       "./tracker <tracker_algorithm> <video_name> <start_frame> [<bounding_frame>]\n"
       "tracker_algorithm can be: MIL, BOOSTING, MEDIANFLOW, TLD, KCF, GOTURN, MOSSE.\n"
       << endl;

  cout << "\n\nHot keys: \n"
       "\tq - quit the program\n"
       "\tp - pause video\n";
}

int TrackerDemo( int argc, char** argv ){
    const std::string mClassName = "TrackerDemo";
  CommandLineParser parser( argc, argv, keys );

  String tracker_algorithm = parser.get<String>(0);
#if 0
  String video_name = parser.get<String>( 1 );
  int start_frame = parser.get<int>(2);

  if( tracker_algorithm.empty() || video_name.empty() )
  {
    help();
    return -1;
  }

  int coords[4]={0,0,0,0};
  bool initBoxWasGivenInCommandLine=false;
  {
      String initBoundingBox=parser.get<String>(3);
      for(size_t npos=0,pos=0,ctr=0;ctr<4;ctr++){
        npos=initBoundingBox.find_first_of(',',pos);
        if(npos==string::npos && ctr<3){
           printf("bounding box should be given in format \"x1,y1,x2,y2\",where x's and y's are integer coordinates of opposed corners of bdd box\n");
           printf("got: %s\n",initBoundingBox.substr(pos,string::npos).c_str());
           printf("manual selection of bounding box will be employed\n");
           break;
        }
        int num=atoi(initBoundingBox.substr(pos,(ctr==3)?(string::npos):(npos-pos)).c_str());
        if(num<=0){
           printf("bounding box should be given in format \"x1,y1,x2,y2\",where x's and y's are integer coordinates of opposed corners of bdd box\n");
           printf("got: %s\n",initBoundingBox.substr(pos,npos-pos).c_str());
           printf("manual selection of bounding box will be employed\n");
           break;
        }
        coords[ctr]=num;
        pos=npos+1;
      }
      if(coords[0]>0 && coords[1]>0 && coords[2]>0 && coords[3]>0){
          initBoxWasGivenInCommandLine=true;
      }
  }

  //open the capture
  VideoCapture cap;
  cap.open( video_name );
  cap.set( CAP_PROP_POS_FRAMES, start_frame );

  if( !cap.isOpened() )
  {
    help();
    cout << "***Could not initialize capturing...***\n";
    cout << "Current parameter's value: \n";
    parser.printMessage();
    return -1;
  }

  Mat frame;
  namedWindow( "Tracking API", 1 );
  Mat image;
  Rect2d boundingBox;
  bool paused = false;
#else
  int index = 1;
  const std::string inputPath = "/data/mars/picdata/pedestrian/";
  Mat frame = cv::imread(inputPath + std::to_string(index) + ".jpg");
  Rect2d boundingBox = {823,601,35,35};
#endif

  //instantiates the specific Tracker
  Ptr<Tracker> tracker = createTrackerByName(tracker_algorithm);
  if (!tracker)
  {
    cout << "***Error in the instantiation of the tracker...***\n";
    return -1;
  }
#if 0
  //get the first frame
  cap >> frame;
  frame.copyTo( image );
  if(initBoxWasGivenInCommandLine){
      boundingBox.x = coords[0];
      boundingBox.y = coords[1];
      boundingBox.width = std::abs( coords[2] - coords[0] );
      boundingBox.height = std::abs( coords[3]-coords[1]);
      printf("bounding box with vertices (%d,%d) and (%d,%d) was given in command line\n",coords[0],coords[1],coords[2],coords[3]);
      rectangle( image, boundingBox, Scalar( 255, 0, 0 ), 2, 1 );
  }
  else
    boundingBox = selectROI("Tracking API", image);

  imshow( "Tracking API", image );
#else
  //initializes the tracker
  if( !tracker->init( frame, boundingBox ) )
  {
    cout << "***Could not initialize tracker...***\n";
    return -1;
  }
#endif

  // do the tracking
  long long totalTime = 0;
  int imageCount = 0;
  for ( int i = 0; i < 3000; ++i )
  {
      // get frame from the video
      frame = cv::imread(inputPath + std::to_string(index) + ".jpg");

      QDateTime beginTime = QDateTime::currentDateTime();
      //updates the tracker
      if( tracker->update( frame, boundingBox ) )
      {
          rectangle( frame, boundingBox, Scalar( 255, 0, 0 ), 2, 1 );
          QDateTime endTime = QDateTime::currentDateTime();

          // 计算耗时
          totalTime += endTime.toMSecsSinceEpoch() - beginTime.toMSecsSinceEpoch();
          ++imageCount;

          // draw the tracked object
          rectangle( frame, boundingBox, Scalar( 255, 0, 0 ), 2, 1 );

          const std::string outputPath = "trackResult";
          Common::createPath(outputPath);
          std::string outputFile = outputPath + "/" + std::to_string(index++) + ".jpg";
          std::vector<int> param;
          param.push_back(cv::IMWRITE_JPEG_QUALITY);
          param.push_back(85);
          imwrite(outputFile, frame, param);
      }
      else
      {
          break;
      }
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
