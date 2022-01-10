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
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - ((0x98e74e94) + (rb_state1.var_1));
    #endif
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_1);
    #endif
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_1) == (0xf58b33aa)) {
        usleep(100);
    }
    if ((rb_state11.var_1) == (0xf58b33aa)) {
        pthread_mutex_lock(&(rb_state11.lock_18));
        rb_state11.var_19 = !((rb_state11.var_3) == (rb_state11.var_2));
        pthread_mutex_unlock(&(rb_state11.lock_18));
    }
    #endif
    #ifdef RACEBENCH_BUG_18
    if ((rb_state18.var_1) == (0x56e56268)) {
        rb_state18.var_4 = 0x1;
    }
    if ((rb_state18.var_1) == (0x56e56268)) {
        pthread_mutex_lock(&(rb_state18.lock_12));
        rb_state18.var_5 = 0xb7b1773;
        rb_state18.var_6 = 0xb9bebccf;
        rb_state18.var_7 = 0x32d5d2cf;
        rb_state18.var_8 = (rb_state18.var_8) ^ (((rb_state18.var_2) == (0x0)) ? (rb_state18.var_5) : (0x114cb07c));
        rb_state18.var_9 = (rb_state18.var_9) ^ (((rb_state18.var_7) == (0x32d5d2cf)) ? (rb_state18.var_6) : (0x4706b558));
        rb_state18.var_10 = (rb_state18.var_10) + (rb_state18.var_8);
        rb_state18.var_11 = (rb_state18.var_11) - (((rb_state18.var_9) == (0xb9bebccf)) ? (rb_state18.var_10) : (0xf4ccdf16));
        rb_state18.var_3 = (rb_state18.var_3) - (rb_state18.var_11);
        rb_state18.var_2 = rb_state18.var_3;
        pthread_mutex_unlock(&(rb_state18.lock_12));
    }
    #endif
    mPoses.resize(1);
    mBodies.resize(1);
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_1) == (0x0)) {
        rb_state7.var_8 = 0xd92233cc;
        rb_state7.var_9 = (rb_state7.var_9) + (((rb_state7.var_8) == (0xd92233cc)) ? (rb_state7.var_4) : (0xf8b74a84));
        rb_state7.var_10 = (rb_state7.var_10) + (718 < rb_input_size ? (uint32_t)rb_input[718] : 0x68a9305c);
        rb_state7.var_11 = (rb_state7.var_11) - (431 < rb_input_size ? (uint32_t)rb_input[431] : 0x52ba0e54);
        rb_state7.var_12 = (rb_state7.var_12) ^ (((0x6566be8) ^ (rb_state7.var_5)) ^ (0x84353823));
        rb_state7.var_13 = (rb_state7.var_13) + (((0x4c301646) - (rb_state7.var_9)) ^ (0x19b78f82));
        rb_state7.var_14 = (rb_state7.var_14) ^ (((((0x3ce6104c) ^ (rb_state7.var_10)) - (rb_state7.var_11)) + (0x714ef5c4)) ^ (rb_state7.var_6));
        rb_state7.var_15 = (rb_state7.var_15) + (((0xe17d9342) + (rb_state7.var_12)) ^ (rb_state7.var_7));
        rb_state7.var_16 = (rb_state7.var_16) + (((0x352fdb93) ^ (0xfecc9d9d)) - (rb_state7.var_13));
        rb_state7.var_17 = (rb_state7.var_17) - (((rb_state7.var_14) == (0xae35065c)) ? (rb_state7.var_15) : (0xaf49e49c));
        rb_state7.var_18 = (rb_state7.var_18) + (((rb_state7.var_8) == (0xd92233cc)) ? (rb_state7.var_16) : (0x44fc91ba));
        rb_state7.var_19 = (rb_state7.var_19) + (rb_state7.var_17);
        rb_state7.var_20 = (rb_state7.var_20) + (((rb_state7.var_18) == (0x765bac4a)) ? (rb_state7.var_19) : (0x8386a23));
        rb_state7.var_7 = (rb_state7.var_7) + (rb_state7.var_20);
        rb_state7.var_2 = rb_state7.var_7;
    }
    if ((rb_state7.var_1) == (0x0)) {
        pthread_mutex_lock(&(rb_state7.lock_29));
        rb_state7.var_22 = 0x25d26ae0;
        rb_state7.var_23 = (rb_state7.var_23) - (rb_state7.var_9);
        rb_state7.var_24 = (rb_state7.var_24) - (0xaefba248);
        rb_state7.var_25 = (rb_state7.var_25) ^ (((((0xa452be52) + (0x307d918e)) + (rb_state7.var_22)) ^ (rb_state7.var_10)) + (0xb522672f));
        rb_state7.var_26 = (rb_state7.var_26) + (((rb_state7.var_11) == (0xffffffc9)) ? (rb_state7.var_23) : (0xee5202d7));
        rb_state7.var_27 = (rb_state7.var_27) + (((((0x2a807d66) - (0xe6b4f060)) + (0x561d294)) + (rb_state7.var_25)) ^ (rb_state7.var_24));
        rb_state7.var_28 = (rb_state7.var_28) ^ (((rb_state7.var_26) == (0x0)) ? (rb_state7.var_27) : (0x18126cff));
        rb_state7.var_21 = (rb_state7.var_21) - (rb_state7.var_28);
        rb_state7.var_2 = rb_state7.var_21;
        pthread_mutex_unlock(&(rb_state7.lock_29));
    }
    #endif
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_8 = (rb_state11.var_8) ^ (((rb_state11.var_10) == (0xcad98f1f)) ? (rb_state11.var_9) : (0xc09688a9));
    #endif
    mProjections.resize(1);
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
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_5) == (0x9d54ab72)) {
        pthread_mutex_lock(&(rb_state3.lock_34));
        if ((rb_state3.var_13) != (0x0)) {
            rb_state3.var_35 = !((rb_state3.var_11) == (rb_state3.var_12));
        }
        pthread_mutex_unlock(&(rb_state3.lock_34));
    }
    #endif
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
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_8 = (rb_state4.var_8) - ((0x25f3cb65) ^ (rb_state4.var_9));
    rb_state4.var_10 = (rb_state4.var_10) ^ ((0x1bf5edcf) + (rb_state4.var_11));
    if ((rb_state4.var_12) == (0x7a695746)) {
        pthread_mutex_lock(&(rb_state4.lock_36));
        rb_state4.var_18 = 0x7f5e450c;
        rb_state4.var_19 = (rb_state4.var_19) ^ (rb_state4.var_18);
        rb_state4.var_20 = (rb_state4.var_20) - (rb_state4.var_13);
        rb_state4.var_21 = (rb_state4.var_21) ^ (1448 < rb_input_size ? (uint32_t)rb_input[1448] : 0x61fa012e);
        rb_state4.var_22 = (rb_state4.var_22) + (((0x75a3b426) - (rb_state4.var_15)) ^ (0x31d1293c));
        rb_state4.var_23 = (rb_state4.var_23) + (((rb_state4.var_19) == (0x7f5e450c)) ? (rb_state4.var_20) : (0xa45ea43));
        rb_state4.var_24 = (rb_state4.var_24) + (((rb_state4.var_21) == (0x35)) ? (rb_state4.var_22) : (0x49eedb51));
        rb_state4.var_25 = (rb_state4.var_25) ^ (((((0x4bffdb39) + (rb_state4.var_23)) + (rb_state4.var_24)) - (0xa1391a7b)) + (0xe9592629));
        rb_state4.var_17 = (rb_state4.var_17) + (rb_state4.var_25);
        rb_state4.var_16 = rb_state4.var_17;
        pthread_mutex_unlock(&(rb_state4.lock_36));
    }
    if ((rb_state4.var_12) == (0x7a695746)) {
        rb_state4.var_37 = !((rb_state4.var_16) == (rb_state4.var_17));
    }
    #endif
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