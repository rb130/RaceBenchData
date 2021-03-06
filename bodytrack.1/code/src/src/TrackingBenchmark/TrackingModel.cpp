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
    ss << setw(width) << setfill(pad) << n;
    return ss.str();
}

TrackingModel::TrackingModel() {
    #ifdef RACEBENCH_BUG_16
    if ((rb_state16.var_0) == (0xc9d1285a)) {
        rb_state16.var_3 = (rb_state16.var_3) - (rb_state16.var_1);
        rb_state16.var_4 = (rb_state16.var_4) + (rb_state16.var_2);
        rb_state16.var_5 = (rb_state16.var_5) ^ (((((0x940a991d) + (rb_state16.var_3)) ^ (rb_state16.var_3)) + (rb_state16.var_4)) - (rb_state16.var_4));
        rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_5);
    }
    #endif
    mPoses.resize(1);
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_17) == (0x40ad365f)) {
        pthread_mutex_lock(&(rb_state1.lock_41));
        rb_state1.var_34 = 0x1;
        pthread_mutex_unlock(&(rb_state1.lock_41));
    }
    if ((rb_state1.var_17) == (0x40ad365f)) {
        pthread_mutex_lock(&(rb_state1.lock_41));
        rb_state1.var_35 = 0xcab701e8;
        rb_state1.var_36 = 0xf8e013ba;
        rb_state1.var_37 = 0xf81ad895;
        rb_state1.var_38 = (rb_state1.var_38) - (((rb_state1.var_35) == (0xcab701e8)) ? (rb_state1.var_37) : (0x28acff08));
        rb_state1.var_39 = (rb_state1.var_39) ^ (rb_state1.var_36);
        rb_state1.var_40 = (rb_state1.var_40) + (((((0x44fc322d) + (rb_state1.var_38)) + (0x7c22aa72)) - (rb_state1.var_39)) - (rb_state1.var_18));
        rb_state1.var_33 = (rb_state1.var_33) ^ (rb_state1.var_40);
        rb_state1.var_32 = rb_state1.var_33;
        pthread_mutex_unlock(&(rb_state1.lock_41));
    }
    #endif
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + (0xda576c9f);
    #endif
    mBodies.resize(1);
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0x1731965b)) {
        rb_state2.var_5 = 0xad926e0a;
        rb_state2.var_6 = 0x85bc418c;
        rb_state2.var_7 = 0x1b245065;
        rb_state2.var_8 = (rb_state2.var_8) - (((0x3247dae7) - (rb_state2.var_3)) - (rb_state2.var_7));
        rb_state2.var_9 = (rb_state2.var_9) - (((0x8b79c74c) + (rb_state2.var_5)) ^ (0x7c305cf7));
        rb_state2.var_10 = (rb_state2.var_10) ^ (((0x3aec9792) + (rb_state2.var_6)) + (rb_state2.var_4));
        rb_state2.var_11 = (rb_state2.var_11) - (((0xf63d8729) ^ (0x24d9a1b0)) ^ (rb_state2.var_8));
        rb_state2.var_12 = (rb_state2.var_12) ^ (((0x61e1f8ad) ^ (rb_state2.var_5)) ^ (rb_state2.var_9));
        rb_state2.var_13 = (rb_state2.var_13) + (((rb_state2.var_6) == (0x85bc418c)) ? (rb_state2.var_10) : (0xb0abf633));
        rb_state2.var_14 = (rb_state2.var_14) - (rb_state2.var_11);
        rb_state2.var_15 = (rb_state2.var_15) ^ (((0xae72f710) + (rb_state2.var_7)) ^ (rb_state2.var_12));
        rb_state2.var_16 = (rb_state2.var_16) ^ (((rb_state2.var_13) == (0xc0a8d91e)) ? (rb_state2.var_14) : (0x11309f9a));
        rb_state2.var_17 = (rb_state2.var_17) + (rb_state2.var_15);
        rb_state2.var_18 = (rb_state2.var_18) - (((0x6b0864df) ^ (rb_state2.var_16)) + (rb_state2.var_8));
        rb_state2.var_19 = (rb_state2.var_19) - (rb_state2.var_17);
        rb_state2.var_20 = (rb_state2.var_20) - (((rb_state2.var_9) == (0xbac3965f)) ? (rb_state2.var_18) : (0x412f31ea));
        rb_state2.var_21 = (rb_state2.var_21) - (((rb_state2.var_10) == (0xc0a8d91e)) ? (rb_state2.var_19) : (0xbc912078));
        rb_state2.var_22 = (rb_state2.var_22) ^ (((((0xdd05e56b) + (rb_state2.var_11)) - (0x7a397fb1)) ^ (rb_state2.var_20)) ^ (rb_state2.var_21));
        rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_22);
    }
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x8c)) {
        if ((rb_state10.var_1) != (0x0)) {
            if (!((rb_state10.var_1) == (rb_state10.var_13))) {
                racebench_trigger(10);
            }
        }
    }
    #endif
    mProjections.resize(1);
    #ifdef RACEBENCH_BUG_0
    if ((rb_state0.var_0) == (0xa0c0c950)) {
        pthread_mutex_lock(&(rb_state0.lock_29));
        rb_state0.var_3 = 0xb1e72d65;
        rb_state0.var_4 = 0xf5ca87c8;
        rb_state0.var_5 = (rb_state0.var_5) - (0x6bde950f);
        rb_state0.var_6 = (rb_state0.var_6) + (20 < rb_input_size ? (uint32_t)rb_input[20] : 0x88d94ee2);
        rb_state0.var_7 = (rb_state0.var_7) - (((rb_state0.var_1) == (0x0)) ? (rb_state0.var_4) : (0xb21b8f9b));
        rb_state0.var_8 = (rb_state0.var_8) + (((0xef79ecbb) + (0xf414ea26)) - (rb_state0.var_3));
        rb_state0.var_9 = (rb_state0.var_9) - (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_5) : (0x8749ca5b));
        rb_state0.var_10 = (rb_state0.var_10) - (rb_state0.var_6);
        rb_state0.var_11 = (rb_state0.var_11) - (rb_state0.var_7);
        rb_state0.var_12 = (rb_state0.var_12) - (((rb_state0.var_3) == (0xb1e72d65)) ? (rb_state0.var_8) : (0xb5431700));
        rb_state0.var_13 = (rb_state0.var_13) ^ (((rb_state0.var_4) == (0xf5ca87c8)) ? (rb_state0.var_9) : (0xc2208e59));
        rb_state0.var_14 = (rb_state0.var_14) + (rb_state0.var_10);
        rb_state0.var_15 = (rb_state0.var_15) ^ (((((0xa010c699) ^ (rb_state0.var_5)) - (rb_state0.var_12)) - (0x31ea85ef)) - (rb_state0.var_11));
        rb_state0.var_16 = (rb_state0.var_16) - (rb_state0.var_13);
        rb_state0.var_17 = (rb_state0.var_17) + (rb_state0.var_14);
        rb_state0.var_18 = (rb_state0.var_18) + (((rb_state0.var_6) == (0x35)) ? (rb_state0.var_15) : (0x55f30a89));
        rb_state0.var_19 = (rb_state0.var_19) + (((rb_state0.var_16) == (0x94216af1)) ? (rb_state0.var_17) : (0xf5a39129));
        rb_state0.var_20 = (rb_state0.var_20) ^ (((rb_state0.var_7) == (0xa357838)) ? (rb_state0.var_18) : (0xf42e3b2d));
        rb_state0.var_21 = (rb_state0.var_21) + (((0x3c9c2cdd) ^ (rb_state0.var_19)) + (0xe6b6fb0b));
        rb_state0.var_22 = (rb_state0.var_22) ^ (((rb_state0.var_8) == (0x31a7a97c)) ? (rb_state0.var_20) : (0xdab4ede6));
        rb_state0.var_23 = (rb_state0.var_23) + (rb_state0.var_21);
        rb_state0.var_24 = (rb_state0.var_24) + (((((0x1d611a7e) + (0x97d29e83)) - (rb_state0.var_23)) - (0x6579d9b2)) - (rb_state0.var_22));
        rb_state0.var_2 = (rb_state0.var_2) - (rb_state0.var_24);
        rb_state0.var_1 = rb_state0.var_2;
        pthread_mutex_unlock(&(rb_state0.lock_29));
    }
    if ((rb_state0.var_0) == (0xa0c0c950)) {
        if (!((rb_state0.var_1) == (rb_state0.var_2))) {
            racebench_trigger(0);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_19 = (rb_state1.var_19) - (rb_state1.var_20);
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