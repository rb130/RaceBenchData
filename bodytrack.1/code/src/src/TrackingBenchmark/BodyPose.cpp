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
        f >> stdAngle[i];
    }
    for (int i = 0; i < N_ANGLES; i++) {
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_0 = (rb_state17.var_0) - (rb_state17.var_0);
        #endif
        f >> minAngles[i];
    }
    for (int i = 0; i < N_ANGLES; i++) {
        f >> maxAngles[i];
    }
    return true;
}