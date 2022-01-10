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
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) + (((rb_state4.var_0) == (0xc96c4b6b)) ? (rb_state4.var_0) : (0x9ecb89c6));
        if ((rb_state4.var_0) == (0x6272fb4d)) {
            rb_state4.var_15 = !((rb_state4.var_2) == (rb_state4.var_1));
        }
        #endif
        #ifdef RACEBENCH_BUG_6
        if ((rb_state6.var_1) == (0x269b95d4)) {
            if ((rb_state6.var_2) != (0x0)) {
                if (!((rb_state6.var_2) == (rb_state6.var_33))) {
                    racebench_trigger(6);
                }
            }
        }
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
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_21 = (rb_state4.var_21) - (854 < rb_input_size ? (uint32_t)rb_input[854] : 0x81170bfc);
        if ((rb_state4.var_22) == (0x44729d1a)) {
            rb_state4.var_38 = rb_state4.var_23;
        }
        if ((rb_state4.var_22) == (0x44729d1a)) {
            pthread_mutex_lock(&(rb_state4.lock_52));
            if (!((rb_state4.var_23) == (rb_state4.var_38))) {
                racebench_trigger(4);
            }
            pthread_mutex_unlock(&(rb_state4.lock_52));
        }
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_13 = (rb_state9.var_13) + (1164 < rb_input_size ? (uint32_t)rb_input[1164] : 0xcfa26cbf);
        #endif
        f >> stdAngle[i];
    }
    for (int i = 0; i < N_ANGLES; i++) {
        f >> minAngles[i];
    }
    for (int i = 0; i < N_ANGLES; i++) {
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_23 = (rb_state3.var_23) ^ (1157 < rb_input_size ? (uint32_t)rb_input[1157] : 0xff9a4b81);
        #endif
        f >> maxAngles[i];
    }
    return true;
}