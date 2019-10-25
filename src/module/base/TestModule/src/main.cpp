#include "faceSample.h"
#include "qualitySample.h"
#include "trackingSample.h"
#include "EyeDetect.h"
#include "FaceDetect.h"
#include "FastFaceDetect.h"
#include "PedestrianDetect.h"
int main(int argc, char *argv[])
{
    // 人脸检测
    //faceDetect();

    // 人眼检测
    //eyeDetect();

    // 快速人脸检测
    //fastFaceDetect();

    // 行人检测
    pedestrianDetect();
#if 0
    // 人脸检测
    return facemarkDemoAAM(argc, argv);
    return facemarkDemoLBF(argc, argv);
    return facemarkLBFFiltting(argc, argv);
    return facerrecDemo(argc, argv);
    return facerecEigefaces(argc, argv);
    return facerecFisherFaces(argc, argv);
    return facerecLBPH(argc, argv);
    return facerecSaveLoad(argc, argv);
    return facerecVideo(argc, argv);
    return maceWebcam(argc, argv);
    return sampleFaceSwapping(argc, argv);
    return sampleTrainLandmarkDetect(argc, argv);
    return sampleTrainLandmarkDetect2(argc, argv);
    return sampleDetectLandmark(argc, argv);
    return sampleDetectLandmarkVideo(argc, argv);
    return sampleWriteConfigFile(argc, argv);

    // 图片质量
    return brisqueEvalTid2008(argc, argv);
    return BrisqueTrainerLiveDB(argc, argv);

    // 跟踪
    return benchmark(argc, argv);
    return osrt(argc, argv);
    return KCFTest(argc, argv);
    return MultiTrackerDemo(argc, argv);
    return TrackerDemo(argc, argv);
    return TutoialOustomizing(argc, argv);
    return TotoriaIntroduction(argc, argv);
    return TotorialMultiTracker(argc, argv);
    return trackByMaching(argc, argv);
#endif
}
