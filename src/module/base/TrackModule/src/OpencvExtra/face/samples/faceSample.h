#ifndef FACE_SAMPLE_H
#define FACE_SAMPLE_H
#include "Base.h"

int ABSTRACT_EXPORT facemarkDemoAAM(int argc, char** argv );
int ABSTRACT_EXPORT facemarkDemoLBF(int argc, char** argv);
int ABSTRACT_EXPORT facemarkLBFFiltting(int argc, char** argv );
int ABSTRACT_EXPORT facerrecDemo(int argc, char **argv);
int ABSTRACT_EXPORT facerecEigefaces(int argc, char **argv);
int ABSTRACT_EXPORT facerecFisherFaces(int argc, char **argv);
int ABSTRACT_EXPORT facerecLBPH(int argc, char **argv);
int ABSTRACT_EXPORT facerecSaveLoad(int argc, char **argv);
int ABSTRACT_EXPORT facerecVideo(int argc, char **argv);
int ABSTRACT_EXPORT maceWebcam(int argc, char **argv);
int ABSTRACT_EXPORT sampleFaceSwapping( int argc, char** argv);
int ABSTRACT_EXPORT sampleTrainLandmarkDetect(int argc,char** argv);
int ABSTRACT_EXPORT sampleTrainLandmarkDetect2(int argc,char** argv);
int ABSTRACT_EXPORT sampleDetectLandmark(int argc,char** argv);
int ABSTRACT_EXPORT sampleDetectLandmarkVideo(int argc,char** argv);
int ABSTRACT_EXPORT sampleWriteConfigFile(int argc, char ** argv);
#endif
