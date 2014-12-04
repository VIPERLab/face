#ifndef LANDMARKDETECTOR_H
#define LANDMARKDETECTOR_H

#include "mesh.h"
#include "landmarks.h"
#include "surfaceprocessor.h"

namespace Face {
namespace FaceData {

class LandmarkDetector
{
private:
    Mesh &mesh;
    MapConverter converter;
    Map depth;
    Map croppedDepth;
    int cropStartX;
    int cropStartY;
    CurvatureStruct curvature;
    Map peakDensity;
    //Map pitDensity;

    // detector params
    int stripeWidth;
    int depthGaussSize;
    int depthGaussIterations;
    double depthScale;
    int depthErode;
    int depthLevelSelect;
    int peakDensityWindowsSize;
    int pitsStripeSmoothKernel;
    int minYDistanceFromNosetipToEyes;
    int maxYDistanceFromNosetipToEyes;
    int minXDistanceFromNosetipToEyes;
    int maxXDistanceFromNosetipToEyes;

    void nosetip(Landmarks &l);
    void innerEyeCorners(Landmarks &l);

public:
    LandmarkDetector(Mesh &mesh);
    Landmarks detect();
};

}
}

#endif // LANDMARKDETECTOR_H