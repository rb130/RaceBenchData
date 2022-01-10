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
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_2 = (rb_state14.var_2) ^ (622 < rb_input_size ? (uint32_t)rb_input[622] : 0x9b4c7daf);
    #endif
    ss << setw(width) << setfill(pad) << n;
    return ss.str();
}

TrackingModel::TrackingModel() {
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ ((0xa31025f5) ^ (0xf3086594));
    rb_state1.var_1 = (rb_state1.var_1) ^ ((0x2c5e060c) ^ (0x2df5b53));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) + (((rb_state13.var_1) == (0x0)) ? (rb_state13.var_0) : (0xffe5defc));
    rb_state13.var_2 = (rb_state13.var_2) ^ (0xb8d36c78);
    #endif
    mPoses.resize(1);
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_2) == (0x8ae2c8f7)) {
        pthread_mutex_lock(&(rb_state1.lock_40));
        rb_state1.var_20 = 0x308c3499;
        rb_state1.var_21 = (rb_state1.var_21) - (0x816cfeb8);
        rb_state1.var_22 = (rb_state1.var_22) - (((((0xe6cbf785) + (0x5a73d4a0)) - (rb_state1.var_11)) + (rb_state1.var_12)) + (rb_state1.var_20));
        rb_state1.var_23 = (rb_state1.var_23) - (((rb_state1.var_13) == (0x44259449)) ? (rb_state1.var_10) : (0xad9c98b0));
        rb_state1.var_24 = (rb_state1.var_24) - (rb_state1.var_21);
        rb_state1.var_25 = (rb_state1.var_25) + (((0x86c61dfa) - (rb_state1.var_14)) ^ (rb_state1.var_22));
        rb_state1.var_26 = (rb_state1.var_26) ^ (((rb_state1.var_15) == (0xe7617bb)) ? (rb_state1.var_23) : (0x3c428ed7));
        rb_state1.var_27 = (rb_state1.var_27) ^ (((rb_state1.var_16) == (0x208aa5f)) ? (rb_state1.var_24) : (0x86c4b899));
        rb_state1.var_28 = (rb_state1.var_28) + (((0xb9aeebd8) ^ (rb_state1.var_17)) ^ (rb_state1.var_25));
        rb_state1.var_29 = (rb_state1.var_29) + (((0x87d1e249) ^ (rb_state1.var_26)) ^ (0x60dddf8e));
        rb_state1.var_30 = (rb_state1.var_30) ^ (((0x993ed87) ^ (0xc8551525)) ^ (rb_state1.var_27));
        rb_state1.var_31 = (rb_state1.var_31) ^ (((rb_state1.var_18) == (0xab554613)) ? (rb_state1.var_28) : (0xf80df618));
        rb_state1.var_32 = (rb_state1.var_32) - (((rb_state1.var_19) == (0x5b32eac8)) ? (rb_state1.var_29) : (0xfa909930));
        rb_state1.var_33 = (rb_state1.var_33) - (rb_state1.var_30);
        rb_state1.var_34 = (rb_state1.var_34) + (rb_state1.var_31);
        rb_state1.var_35 = (rb_state1.var_35) + (((0xcef5d114) + (0x81f00d5b)) ^ (rb_state1.var_32));
        rb_state1.var_36 = (rb_state1.var_36) - (rb_state1.var_33);
        rb_state1.var_37 = (rb_state1.var_37) + (((0x9c17dbd6) ^ (rb_state1.var_20)) ^ (rb_state1.var_34));
        rb_state1.var_38 = (rb_state1.var_38) ^ (((((0x5bf0fd8a) + (0x8ab557c3)) + (rb_state1.var_21)) ^ (rb_state1.var_36)) - (rb_state1.var_35));
        rb_state1.var_39 = (rb_state1.var_39) + (((rb_state1.var_37) == (0x8c1e9baa)) ? (rb_state1.var_38) : (0x8a133a82));
        rb_state1.var_3 = (rb_state1.var_3) - (rb_state1.var_39);
        pthread_mutex_unlock(&(rb_state1.lock_40));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) - (249 < rb_input_size ? (uint32_t)rb_input[249] : 0x8e01d7b);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) + (0xc337d5a3);
    rb_state13.var_2 = (rb_state13.var_2) - ((0xfc80672c) + (0x56629aea));
    #endif
    mBodies.resize(1);
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_3) == (0x8489a922)) {
        rb_state2.var_12 = 0xd766647a;
        rb_state2.var_13 = 0x6e234f42;
        rb_state2.var_14 = (rb_state2.var_14) ^ (((rb_state2.var_9) == (0x0)) ? (rb_state2.var_8) : (0x6a2540b7));
        rb_state2.var_15 = (rb_state2.var_15) - (rb_state2.var_12);
        rb_state2.var_16 = (rb_state2.var_16) ^ (rb_state2.var_13);
        rb_state2.var_17 = (rb_state2.var_17) - (((0x45aa8fcb) + (0x1bd05c69)) - (rb_state2.var_7));
        rb_state2.var_18 = (rb_state2.var_18) + (((((0x110a8057) + (0x839d81a)) + (rb_state2.var_15)) - (rb_state2.var_10)) + (rb_state2.var_14));
        rb_state2.var_19 = (rb_state2.var_19) + (((rb_state2.var_16) == (0x6e234f42)) ? (rb_state2.var_17) : (0x6348ee54));
        rb_state2.var_20 = (rb_state2.var_20) + (rb_state2.var_18);
        rb_state2.var_21 = (rb_state2.var_21) - (((((0x7a173f33) + (rb_state2.var_20)) - (rb_state2.var_19)) ^ (rb_state2.var_11)) ^ (0xfe75116b));
        rb_state2.var_11 = (rb_state2.var_11) - (rb_state2.var_21);
        rb_state2.var_4 = rb_state2.var_11;
    }
    if ((rb_state2.var_3) == (0x8489a922)) {
        pthread_mutex_lock(&(rb_state2.lock_47));
        rb_state2.var_23 = (rb_state2.var_23) - (1423 < rb_input_size ? (uint32_t)rb_input[1423] : 0x2adfa1cb);
        rb_state2.var_24 = (rb_state2.var_24) - (rb_state2.var_15);
        rb_state2.var_25 = (rb_state2.var_25) ^ (0xc095d702);
        rb_state2.var_26 = (rb_state2.var_26) - (((0xc1a2e5c1) - (rb_state2.var_12)) - (rb_state2.var_16));
        rb_state2.var_27 = (rb_state2.var_27) - (((rb_state2.var_17) == (0x9e8513cc)) ? (rb_state2.var_13) : (0x2afe4ac));
        rb_state2.var_28 = (rb_state2.var_28) - (((0xa6486713) - (rb_state2.var_14)) + (rb_state2.var_18));
        rb_state2.var_29 = (rb_state2.var_29) - (((rb_state2.var_23) == (0xffffffce)) ? (rb_state2.var_24) : (0x83cce7fb));
        rb_state2.var_30 = (rb_state2.var_30) ^ (((rb_state2.var_19) == (0x9e8513cc)) ? (rb_state2.var_25) : (0xe1b3cf52));
        rb_state2.var_31 = (rb_state2.var_31) + (((0x6feb8ecd) ^ (rb_state2.var_20)) + (rb_state2.var_26));
        rb_state2.var_32 = (rb_state2.var_32) ^ (((0x52273bca) - (rb_state2.var_21)) + (rb_state2.var_27));
        rb_state2.var_33 = (rb_state2.var_33) + (rb_state2.var_28);
        rb_state2.var_34 = (rb_state2.var_34) + (((rb_state2.var_22) == (0x0)) ? (rb_state2.var_29) : (0x2dbd3e3b));
        rb_state2.var_35 = (rb_state2.var_35) + (((0xa309793b) - (rb_state2.var_30)) + (rb_state2.var_23));
        rb_state2.var_36 = (rb_state2.var_36) + (rb_state2.var_31);
        rb_state2.var_37 = (rb_state2.var_37) - (((0x402d818b) + (rb_state2.var_32)) ^ (0xc9f6ad35));
        rb_state2.var_38 = (rb_state2.var_38) - (((rb_state2.var_24) == (0xd766647a)) ? (rb_state2.var_33) : (0x19a8e94d));
        rb_state2.var_39 = (rb_state2.var_39) - (((0x4e4e0672) + (0xff0c8cc1)) ^ (rb_state2.var_34));
        rb_state2.var_40 = (rb_state2.var_40) - (((rb_state2.var_35) == (0xe273a207)) ? (rb_state2.var_36) : (0xc14f5a74));
        rb_state2.var_41 = (rb_state2.var_41) + (((rb_state2.var_25) == (0xc095d702)) ? (rb_state2.var_37) : (0x6c3ed4ec));
        rb_state2.var_42 = (rb_state2.var_42) ^ (((0xdcfaca03) ^ (rb_state2.var_38)) ^ (rb_state2.var_26));
        rb_state2.var_43 = (rb_state2.var_43) + (((((0xed0c3705) + (rb_state2.var_40)) - (0xb47bb044)) + (rb_state2.var_39)) - (0x7335cf6f));
        rb_state2.var_44 = (rb_state2.var_44) + (((((0x247bfa74) + (rb_state2.var_41)) + (0x2a28dcd)) + (rb_state2.var_42)) + (0x4c0e519e));
        rb_state2.var_45 = (rb_state2.var_45) + (rb_state2.var_43);
        rb_state2.var_46 = (rb_state2.var_46) - (((rb_state2.var_44) == (0x5ba66554)) ? (rb_state2.var_45) : (0xfda6d142));
        rb_state2.var_22 = (rb_state2.var_22) ^ (rb_state2.var_46);
        rb_state2.var_4 = rb_state2.var_22;
        pthread_mutex_unlock(&(rb_state2.lock_47));
    }
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_1) == (0xc0c9dd86)) {
        rb_state6.var_4 = rb_state6.var_2;
    }
    if ((rb_state6.var_1) == (0xc0c9dd86)) {
        if (!((rb_state6.var_2) == (rb_state6.var_4))) {
            racebench_trigger(6);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) ^ (0xdd31f9b6);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) + (547 < rb_input_size ? (uint32_t)rb_input[547] : 0xd80c63af);
    #endif
    mProjections.resize(1);
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) ^ (((rb_state13.var_3) == (0xd622a175)) ? (rb_state13.var_2) : (0xd5ab7ee1));
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
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) - (rb_state11.var_2);
    #endif
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