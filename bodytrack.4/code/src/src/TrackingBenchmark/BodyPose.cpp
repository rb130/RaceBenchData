#include "racebench_bugs.h"


#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <fstream>
#include <iostream>
#include "BodyPose.h"
#include <vector>

using namespace std;

void BodyPose::Set(float *angle_values, int n) {
    mAngles.resize(n);
    for (int i = 0; i < n; i++) {
        mAngles[i] = angle_values[i];
    }
}

bool BodyPose::Valid(PoseParams &params) {
    return true;

    if ((mAngles[0] < params.PoseMin(0)) || (mAngles[0] > params.PoseMax(0))) {
        return false;
    }
    if ((mAngles[1] < params.PoseMin(1)) || (mAngles[1] > params.PoseMax(1))) {
        return false;
    }
    for (int i = 5; i < (int)mAngles.size(); i++) {
        if ((mAngles[i] < params.PoseMin(i)) || (mAngles[i] > params.PoseMax(i))) {
            return false;
        }
    }

    return true;
}

bool BodyPose::Initialize(string fname) {
    mAngles.resize(N_ANGLES);
    ifstream f(fname.c_str());
    if (!f.is_open()) {
        cout << "Unable to open Initial Pose file : " << fname << endl;
        return false;
    }
    for (int i = 0; i < N_ANGLES; i++) {
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_9 = (rb_state10.var_9) ^ (rb_state10.var_10);
        #endif
        f >> mAngles[i];
    }
    return true;
}

bool PoseParams::Initialize(string fname) {
    stdAngle.resize(N_ANGLES);
    minAngles.resize(N_ANGLES);
    maxAngles.resize(N_ANGLES);

    ifstream f(fname.c_str());
    if (!f.is_open()) {
        cout << "Unable to open Pose Parameter file : " << fname << endl;
        return false;
    }
    for (int i = 0; i < N_ANGLES; i++) {
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_0 = (rb_state14.var_0) + (1432 < rb_input_size ? (uint32_t)rb_input[1432] : 0x2cbe0f63);
        rb_state14.var_1 = (rb_state14.var_1) ^ (378 < rb_input_size ? (uint32_t)rb_input[378] : 0x496d4517);
        #endif
        f >> stdAngle[i];
    }
    for (int i = 0; i < N_ANGLES; i++) {
        f >> minAngles[i];
    }
    for (int i = 0; i < N_ANGLES; i++) {
        #ifdef RACEBENCH_BUG_14
        rb_state14.var_2 = (rb_state14.var_2) - (((rb_state14.var_0) == (0x3fea3cfb)) ? (rb_state14.var_3) : (0xa1001151));
        #endif
        f >> maxAngles[i];
    }
    return true;
}