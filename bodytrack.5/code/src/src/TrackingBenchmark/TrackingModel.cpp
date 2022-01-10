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
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) ^ (0xba64ec10);
    #endif
    ss << setw(width) << setfill(pad) << n;
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) ^ ((0x4bb21c04) + (0x7931e71d));
    rb_state18.var_2 = (rb_state18.var_2) + (0x8eb40b07);
    #endif
    return ss.str();
}

TrackingModel::TrackingModel() {
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) ^ (0x628f4fa4);
    #endif
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_3 = (rb_state14.var_3) - (rb_state14.var_4);
    rb_state14.var_0 = (rb_state14.var_0) - ((0xc31ec061) ^ (0x49fabd2a));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_3 = (rb_state16.var_3) ^ (0xd0f8d4ce);
    #endif
    mPoses.resize(1);
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + (0xb8ac5427);
    #endif
    mBodies.resize(1);
    mProjections.resize(1);
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_1 = (rb_state14.var_1) + (((rb_state14.var_3) == (0x74058f30)) ? (rb_state14.var_2) : (0x32aadfcb));
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
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_3 = (rb_state6.var_3) - (rb_state6.var_3);
    #endif
    mNCameras = cameras;
    mFGMaps.resize(cameras);
    mEdgeMaps.resize(cameras);
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_2) == (0x54b2a3e6)) {
        rb_state6.var_14 = 0x11037288;
        rb_state6.var_15 = 0x907587c1;
        rb_state6.var_16 = (rb_state6.var_16) - ((0xd7211d7) ^ (rb_state6.var_12));
        rb_state6.var_17 = (rb_state6.var_17) + (((0x2c95fdd3) + (0x68ded5b7)) + (rb_state6.var_14));
        rb_state6.var_18 = (rb_state6.var_18) ^ (rb_state6.var_11);
        rb_state6.var_19 = (rb_state6.var_19) ^ (((((0x1c8d2036) + (rb_state6.var_15)) + (rb_state6.var_16)) + (0xe0b1a2eb)) ^ (rb_state6.var_13));
        rb_state6.var_20 = (rb_state6.var_20) ^ (((0xfa255d03) + (0x34a37e36)) - (rb_state6.var_17));
        rb_state6.var_21 = (rb_state6.var_21) + (((rb_state6.var_18) == (0x0)) ? (rb_state6.var_19) : (0x4139b6c2));
        rb_state6.var_22 = (rb_state6.var_22) - (((0x1ba6e4e1) ^ (0x7f93a9ce)) ^ (rb_state6.var_20));
        rb_state6.var_23 = (rb_state6.var_23) + (((0xb5faf462) - (rb_state6.var_14)) ^ (rb_state6.var_21));
        rb_state6.var_24 = (rb_state6.var_24) - (((0xf45a02e6) - (rb_state6.var_22)) - (rb_state6.var_15));
        rb_state6.var_25 = (rb_state6.var_25) ^ (((rb_state6.var_16) == (0xf28dee29)) ? (rb_state6.var_23) : (0x32cf36c1));
        rb_state6.var_26 = (rb_state6.var_26) ^ (((((0x954a4325) ^ (rb_state6.var_24)) ^ (0xdd63ab37)) - (0xad4a507)) ^ (rb_state6.var_25));
        rb_state6.var_13 = (rb_state6.var_13) ^ (rb_state6.var_26);
        rb_state6.var_5 = rb_state6.var_13;
    }
    if ((rb_state6.var_2) == (0x54b2a3e6)) {
        pthread_mutex_lock(&(rb_state6.lock_35));
        rb_state6.var_28 = 0x5b289d07;
        rb_state6.var_29 = 0x85e3dd7e;
        rb_state6.var_30 = (rb_state6.var_30) + (((rb_state6.var_29) == (0x85e3dd7e)) ? (rb_state6.var_17) : (0x9fe708b4));
        rb_state6.var_31 = (rb_state6.var_31) + (((0xe35683c6) - (rb_state6.var_28)) ^ (rb_state6.var_18));
        rb_state6.var_32 = (rb_state6.var_32) + (((rb_state6.var_19) == (0x8042390b)) ? (rb_state6.var_30) : (0xb11262bc));
        rb_state6.var_33 = (rb_state6.var_33) ^ (((0x66c32ac) - (rb_state6.var_31)) - (0x11ec262d));
        rb_state6.var_34 = (rb_state6.var_34) ^ (((rb_state6.var_32) == (0xa6784612)) ? (rb_state6.var_33) : (0x4ecf19e6));
        rb_state6.var_27 = (rb_state6.var_27) + (rb_state6.var_34);
        rb_state6.var_5 = rb_state6.var_27;
        pthread_mutex_unlock(&(rb_state6.lock_35));
    }
    #endif
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