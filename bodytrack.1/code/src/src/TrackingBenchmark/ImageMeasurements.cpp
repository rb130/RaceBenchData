

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "ImageMeasurements.h"
#include <math.h>

using namespace std;

inline double mag(Point &p) {
    return sqrt((double)(p.x * p.x + p.y * p.y));
}

inline void SampleEdgePoint(float xf, float yf, const FlexImage8u &EdgeMap, int &error, int &samplePoints) {
    int x = int(xf + 0.5f), y = int(yf + 0.5f);
    if ((x >= 0) && (x < EdgeMap.Width()) && (y >= 0) && (y < EdgeMap.Height())) {
        int e = 255 - EdgeMap(x, y);
        error += (e * e);
        samplePoints++;
    }
}

void ImageMeasurements::EdgeError(const ProjectedCylinder &ProjCyl, const FlexImage8u &EdgeMap, float &error, int &samplePoints) {
    int ErrorSSD = 0;
    const Point &p1 = ProjCyl.mPts[0];
    Point s1;
    s1.Set(ProjCyl.mPts[1].x - p1.x, ProjCyl.mPts[1].y - p1.y);
    int n1 = max((int)(mag(s1) / mStep + 0.5), 4);
    float d1 = 1.0f / (float)n1++;

    const Point &p2 = ProjCyl.mPts[2];
    Point s2;
    s2.Set(ProjCyl.mPts[3].x - p2.x, ProjCyl.mPts[3].y - p2.y);
    int n2 = max((int)(mag(s2) / mStep + 0.5), 4);
    float d2 = 1.0f / (float)n2++;

    float delta = 0;
    for (int i = 0; i < n1; i++) {
        float x = p1.x + delta * s1.x;
        float y = p1.y + delta * s1.y;
        SampleEdgePoint(x, y, EdgeMap, ErrorSSD, samplePoints);
        delta += d1;
    }
    delta = 0;
    for (int i = 0; i < n2; i++) {
        float x = p2.x + delta * s2.x;
        float y = p2.y + delta * s2.y;
        SampleEdgePoint(x, y, EdgeMap, ErrorSSD, samplePoints);
        delta += d2;
    }
    error += (float)ErrorSSD / (255.0f * 255.0f);
}

inline void SampleInsidePoint(float xf, float yf, const BinaryImage &FGmap, int &error, int &samplePoints) {
    int x = int(xf + 0.5f), y = int(yf + 0.5f);
    if ((x >= 0) && (x < FGmap.Width()) && (y >= 0) && (y < FGmap.Height())) {
        int e = 1 - FGmap(x, y);
        error += e;
        samplePoints++;
    }
}

void ImageMeasurements::InsideError(const ProjectedCylinder &ProjCyl, const BinaryImage &FGmap, int &error, int &samplePoints) {
    const Point &p1 = ProjCyl.mPts[0], &p2 = ProjCyl.mPts[3];
    Point s1, s2;
    s1.Set(ProjCyl.mPts[1].x - p1.x, ProjCyl.mPts[1].y - p1.y);
    s2.Set(ProjCyl.mPts[2].x - p2.x, ProjCyl.mPts[2].y - p2.y);
    Point m(p1.x + s1.x / 2.0f - (p2.x + s2.x / 2.0f), p1.y + s1.y / 2.0f - (p2.y + s2.y / 2.0f));
    int n1 = max((int)(mag(s1) / mVstep + 0.5), 4);
    int n2 = max((int)(mag(m) / mHstep + 0.5f), 4);
    float d1 = 1.0f / n1++;
    float d2 = 1.0f / n2;
    float delta1 = 0;
    Point e1, e2;
    for (int i = 0; i < n1; i++) {
        e1.Set(p1.x + delta1 * s1.x, p1.y + delta1 * s1.y);
        e2.Set(p2.x + delta1 * s2.x, p2.y + delta1 * s2.y);
        m.Set(e2.x - e1.x, e2.y - e1.y);
        delta1 += d1;
        float delta2 = 0;
        for (int j = 0; j < n2; j++) {
            SampleInsidePoint(e1.x + delta2 * m.x, e1.y + delta2 * m.y, FGmap, error, samplePoints);
            delta2 += d2;
        }
    }
}

float ImageMeasurements::ImageErrorEdge(std::vector<FlexImage8u> &ImageMaps, MultiCameraProjectedBody &ProjBodies) {
    int samples = 0;
    float error = 0;
    for (int i = 0; i < (int)ImageMaps.size(); i++) {
        int nParts = ProjBodies(i).Size();
        for (int j = 0; j < nParts; j++) {
            EdgeError(ProjBodies(i)(j), ImageMaps[i], error, samples);
        }
    }

    return (float)error / samples;
}

float ImageMeasurements::ImageErrorInside(std::vector<BinaryImage> &ImageMaps, MultiCameraProjectedBody &ProjBodies) {
    int samples = 0;
    int error = 0;
    for (int i = 0; i < (int)ImageMaps.size(); i++) {
        int nParts = ProjBodies(i).Size();
        for (int j = 0; j < nParts; j++) {
            InsideError(ProjBodies(i)(j), ImageMaps[i], error, samples);
        }
    }

    return (float)error / samples;
}
