#include "racebench_bugs.h"


#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <sstream>
#include <iomanip>
#include <sys/types.h>
#include "TrackingModel.h"
#include "CovarianceMatrix.h"
#include "FlexLib.h"
#include "system.h"

#ifndef uint
#define uint unsigned int
#endif

using namespace std;

template <class T> inline string str(T n, int width = 0, char pad = '0') {
    stringstream ss;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) ^ (0xe475d82f);
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_1 = (rb_state8.var_1) - (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_1) : (0x6d9c1678));
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_2) == (0x77721f93)) {
        if ((rb_state19.var_3) != (0x0)) {
            if (!((rb_state19.var_3) == (rb_state19.var_26))) {
                racebench_trigger(19);
            }
        }
    }
    #endif
    ss << setw(width) << setfill(pad) << n;
    return ss.str();
}

TrackingModel::TrackingModel() {
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_2) == (0x3b981f21)) {
        if ((rb_state9.var_3) != (0x0)) {
            if (!((rb_state9.var_3) == (rb_state9.var_29))) {
                racebench_trigger(9);
            }
        }
    }
    #endif
    mPoses.resize(1);
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_2 = (rb_state1.var_2) + ((0x8eb895e1) + (rb_state1.var_2));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) - ((0xa582e873) + (rb_state7.var_0));
    #endif
    mBodies.resize(1);
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_1 = (rb_state3.var_1) + (((rb_state3.var_1) == (0x1042c399)) ? (rb_state3.var_2) : (0xbb190e01));
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_1) == (0xf1919551)) {
        if ((rb_state4.var_2) != (0x0)) {
            if (!((rb_state4.var_2) == (rb_state4.var_14))) {
                racebench_trigger(4);
            }
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_2) == (0x40aededa)) {
        rb_state10.var_4 = 0x6cef008f;
        rb_state10.var_5 = (rb_state10.var_5) + (((rb_state10.var_3) == (0xc773e178)) ? (rb_state10.var_4) : (0x2efd566e));
        rb_state10.var_1 = (rb_state10.var_1) ^ (rb_state10.var_5);
    }
    #endif
    mProjections.resize(1);
    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0x0)) {
        rb_state12.var_5 = 0x20a0cbb8;
        rb_state12.var_6 = 0x5a0ebbbf;
        rb_state12.var_7 = (rb_state12.var_7) - (((((0x273f54c4) + (rb_state12.var_5)) ^ (rb_state12.var_5)) - (rb_state12.var_4)) - (0x98ad72ca));
        rb_state12.var_8 = (rb_state12.var_8) - (rb_state12.var_6);
        rb_state12.var_9 = (rb_state12.var_9) - (rb_state12.var_6);
        rb_state12.var_10 = (rb_state12.var_10) ^ (((rb_state12.var_7) == (0x316c8706)) ? (rb_state12.var_8) : (0x791f5ac6));
        rb_state12.var_11 = (rb_state12.var_11) - (((((0xa5f55e72) ^ (rb_state12.var_10)) + (rb_state12.var_9)) - (rb_state12.var_7)) ^ (0x4c9173dc));
        rb_state12.var_4 = (rb_state12.var_4) - (rb_state12.var_11);
        rb_state12.var_3 = rb_state12.var_4;
    }
    if ((rb_state12.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state12.lock_33));
        rb_state12.var_13 = 0x6657feb4;
        rb_state12.var_14 = 0xd6be4402;
        rb_state12.var_15 = (rb_state12.var_15) + (rb_state12.var_13);
        rb_state12.var_16 = (rb_state12.var_16) ^ (0xc3a68bd9);
        rb_state12.var_17 = (rb_state12.var_17) + (rb_state12.var_14);
        rb_state12.var_18 = (rb_state12.var_18) - (((rb_state12.var_9) == (0xa5f14441)) ? (rb_state12.var_8) : (0x4dc62329));
        rb_state12.var_19 = (rb_state12.var_19) + (rb_state12.var_15);
        rb_state12.var_20 = (rb_state12.var_20) - (((rb_state12.var_10) == (0xa5f14441)) ? (rb_state12.var_16) : (0xedb65715));
        rb_state12.var_21 = (rb_state12.var_21) + (rb_state12.var_17);
        rb_state12.var_22 = (rb_state12.var_22) - (((rb_state12.var_18) == (0x5a0ebbbf)) ? (rb_state12.var_19) : (0xd87f269d));
        rb_state12.var_23 = (rb_state12.var_23) - (((rb_state12.var_11) == (0xc7e65b4e)) ? (rb_state12.var_20) : (0x6caa9a23));
        rb_state12.var_24 = (rb_state12.var_24) ^ (((rb_state12.var_12) == (0x0)) ? (rb_state12.var_21) : (0x47c2f187));
        rb_state12.var_25 = (rb_state12.var_25) - (rb_state12.var_22);
        rb_state12.var_26 = (rb_state12.var_26) + (((rb_state12.var_23) == (0xc3a68be0)) ? (rb_state12.var_24) : (0x1dc36bc8));
        rb_state12.var_27 = (rb_state12.var_27) + (((rb_state12.var_13) == (0x6657feb4)) ? (rb_state12.var_25) : (0x4ca81c1d));
        rb_state12.var_28 = (rb_state12.var_28) + (rb_state12.var_26);
        rb_state12.var_29 = (rb_state12.var_29) ^ (((rb_state12.var_14) == (0xd6be4402)) ? (rb_state12.var_27) : (0xb0d32086));
        rb_state12.var_30 = (rb_state12.var_30) - (((0xe13c74e) ^ (rb_state12.var_28)) - (rb_state12.var_15));
        rb_state12.var_31 = (rb_state12.var_31) + (rb_state12.var_29);
        rb_state12.var_32 = (rb_state12.var_32) ^ (((rb_state12.var_30) == (0x8daa7b68)) ? (rb_state12.var_31) : (0x704a7130));
        rb_state12.var_12 = (rb_state12.var_12) + (rb_state12.var_32);
        rb_state12.var_3 = rb_state12.var_12;
        pthread_mutex_unlock(&(rb_state12.lock_33));
    }
    #endif
    mImageMeasurements.resize(1);
}

void TrackingModel::SetNumThreads(int n) {
    mPoses.resize(n);
    mBodies.resize(n);
    mProjections.resize(n);
    mImageMeasurements.resize(n);
}

bool TrackingModel::InitCameras(vector<string> &fileNames) {
    mCameras.SetCameras((int)fileNames.size());
    for (int i = 0; i < (int)fileNames.size(); i++) {
        if (!mCameras(i).LoadParams(fileNames[i].c_str())) {
            return false;
        }
    }
    return true;
}

extern const char *param_file;
bool TrackingModel::Initialize(const string &path, int cameras, int layers) {
    mPath = path;
    mNCameras = cameras;
    mFGMaps.resize(cameras);
    mEdgeMaps.resize(cameras);
    vector<string> calibFiles(cameras);
    for (int i = 0; i < cameras; i++) {
        calibFiles[i] = path + "CALIB" + DIR_SEPARATOR + "Camera" + str(i + 1) + ".cal";
    }
    if (!InitCameras(calibFiles)) {
        return false;
    }
    if (!InitGeometry(path + "BodyShapeParameters.txt")) {
        return false;
    }
    if (!LoadInitialState(path + "InitialPose.txt")) {
        return false;
    }
    string pose_path = param_file == NULL ? path + "PoseParameters.txt" : string(param_file);

    if (!LoadPoseParameters(pose_path)) {
        return false;
    }
    GenerateStDevMatrices(layers, mPoses[0].Params(), mStdDevs);
    return true;
}

float TrackingModel::LogLikelihood(const vector<float> &v, bool &valid, int thread) {
    BodyPose &pose = mPoses[thread];
    BodyGeometry &body = mBodies[thread];
    MultiCameraProjectedBody &projections = mProjections[thread];
    ImageMeasurements &measurements = mImageMeasurements[thread];

    pose.Set(v);
    valid = false;
    if (!pose.Valid(mPoses[0].Params())) {
        return -1e10;
    }
    body.ComputeGeometry(pose, mBodies[0].Parameters());
    if (!body.Valid()) {
        return -1e10;
    }
    projections.ImageProjection(body, mCameras);
    float err = measurements.ImageErrorEdge(mEdgeMaps, projections);
    err += measurements.ImageErrorInside(mFGMaps, projections);
    valid = true;
    return -err;
}

inline void GaussianBlur(FlexImage8u &src, FlexImage8u &dst) {
    float k[] = {0.12149085090552f, 0.14203719483447f, 0.15599734045770f, 0.16094922760463f, 0.15599734045770f, 0.14203719483447f, 0.12149085090552f};
    FlexImage8u tmp;
    FlexFilterRowV(src, tmp, k, 7);
    FlexFilterColumnV(tmp, dst, k, 7);
}

inline FlexImage8u GradientMagThreshold(const FlexImage8u &src, float threshold) {
    FlexImage8u r(src.Size());
    ZeroBorder(r);
    for (int y = 1; y < src.Height() - 1; y++) {
        Im8u *p = &src(1, y), *ph = &src(1, y - 1), *pl = &src(1, y + 1), *pr = &r(1, y);
        for (int x = 1; x < src.Width() - 1; x++) {
            float xg = -0.125f * ph[-1] + 0.125f * ph[1] - 0.250f * p[-1] + 0.250f * p[1] - 0.125f * pl[-1] + 0.125f * pl[1];
            float yg = -0.125f * ph[-1] - 0.250f * ph[0] - 0.125f * ph[1] + 0.125f * pl[-1] + 0.250f * pl[0] + 0.125f * pl[1];
            float mag = xg * xg + yg * yg;
            *pr = (mag < threshold) ? 0 : 255;
            p++;
            ph++;
            pl++;
            pr++;
        }
    }
    return r;
}

void TrackingModel::CreateEdgeMap(const FlexImage8u &src, FlexImage8u &dst) {
    FlexImage8u gr = GradientMagThreshold(src, 16.0f);
    GaussianBlur(gr, dst);
}

bool TrackingModel::GetObservation(float timeval) {
    int frame = (int)timeval;
    int n = mCameras.GetCameraCount();
    vector<string> FGfiles(n), ImageFiles(n);
    for (int i = 0; i < n; i++) {
        FGfiles[i] = mPath + "FG" + str(i + 1) + DIR_SEPARATOR + "image" + str(frame, 4) + ".bmp";
        ImageFiles[i] = mPath + "CAM" + str(i + 1) + DIR_SEPARATOR + "image" + str(frame, 4) + ".bmp";
    }
    FlexImage8u im;
    for (uint i = 0; i < FGfiles.size(); i++) {
        if (!FlexLoadBMP(FGfiles[i].c_str(), im)) {
            cout << "Unable to load image: " << FGfiles[i].c_str() << endl;
            return false;
        }
        mFGMaps[i].ConvertToBinary(im);
        if (!FlexLoadBMP(ImageFiles[i].c_str(), im)) {
            cout << "Unable to load image: " << ImageFiles[i].c_str() << endl;
            return false;
        }
        CreateEdgeMap(im, mEdgeMaps[i]);
    }
    return true;
}

bool TrackingModel::OutputBMP(const std::vector<float> &pose, int frame) {
    vector<string> ImageFiles(mNCameras);
    for (int i = 0; i < mNCameras; i++) {
        ImageFiles[i] = mPath + "CAM" + str(i + 1) + DIR_SEPARATOR + "image" + str(frame, 4) + ".bmp";
    }

    Im8u yellow[3] = {0, 255, 255}, cyan[3] = {255, 255, 0}, magenta[3] = {255, 0, 255};

    mPoses[0].Set(pose);
    mBodies[0].ComputeGeometry(mPoses[0], mBodies[0].Parameters());
    mProjections[0].ImageProjection(mBodies[0], mCameras);
    int levels = int(ImageFiles.size() - 1) / 2 + 1;
    int w = mEdgeMaps[0].Width() / 2, h = mEdgeMaps[0].Height() / 2;
    FlexImage<Im8u, 3> result(w * 2, h * levels), dstImage;

    for (int camera = 0; camera < (int)ImageFiles.size(); camera++) {
        FlexImage<Im8u, 1> srcImage, imds;
        FlexLoadBMP(ImageFiles[camera].c_str(), srcImage);
        FlexDownSample2(srcImage, imds);
        dstImage = result((camera % 2) * w, int(camera / 2) * h, w, h);
        FlexGrayToRGB(imds, dstImage, false);
        ProjectedBody &pb = mProjections[0](camera);
        for (int i = 0; i < pb.Size(); i++) {
            ProjectedCylinder &c = pb(i);
            Im8u *color = cyan;
            if (i == 7 || i == 8 || i == 3 || i == 4) {
                color = yellow;
            } else if (i == 9 || i == 0) {
                color = magenta;
            }
            FlexLine(dstImage, (int)c.mPts[0].x / 2, (int)c.mPts[0].y / 2, (int)c.mPts[1].x / 2, (int)c.mPts[1].y / 2, color);
            FlexLine(dstImage, (int)c.mPts[1].x / 2, (int)c.mPts[1].y / 2, (int)c.mPts[2].x / 2, (int)c.mPts[2].y / 2, color);
            FlexLine(dstImage, (int)c.mPts[2].x / 2, (int)c.mPts[2].y / 2, (int)c.mPts[3].x / 2, (int)c.mPts[3].y / 2, color);
            FlexLine(dstImage, (int)c.mPts[3].x / 2, (int)c.mPts[3].y / 2, (int)c.mPts[0].x / 2, (int)c.mPts[0].y / 2, color);
        }
    }
    string outFname = mPath + "Result" + str(frame, 4) + ".bmp";
    return FlexSaveBMP(outFname.c_str(), result);
}