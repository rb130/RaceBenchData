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
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_3 = (rb_state9.var_3) ^ ((0x7c9ba970) - (0xa3e4c745));
        rb_state9.var_3 = (rb_state9.var_3) + (rb_state9.var_2);
        rb_state9.var_3 = (rb_state9.var_3) + (0x3abe3181);
        if ((rb_state9.var_0) == (0x1abf2d78)) {
            rb_state9.var_1 = (rb_state9.var_1) - (rb_state9.var_4);
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_2 = (rb_state10.var_2) ^ (rb_state10.var_3);
        #endif
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_3 = (rb_state17.var_3) ^ (0xeae3c9a9);
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_1 = (rb_state18.var_1) + (rb_state18.var_2);
        #endif
        f >> mAngles[i];
    }
    return true;
}

bool PoseParams::Initialize(string fname) {
    stdAngle.resize(N_ANGLES);
    minAngles.resize(N_ANGLES);
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_4 = (rb_state17.var_4) + (((rb_state17.var_3) == (0xeae3c9a9)) ? (rb_state17.var_2) : (0x5d2cd350));
    #endif
    maxAngles.resize(N_ANGLES);

    ifstream f(fname.c_str());
    if (!f.is_open()) {
        cout << "Unable to open Pose Parameter file : " << fname << endl;
        return false;
    }
    for (int i = 0; i < N_ANGLES; i++) {
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_2 = (rb_state7.var_2) ^ ((0xc6905d27) + (rb_state7.var_0));
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_3 = (rb_state18.var_3) - (1015 < rb_input_size ? (uint32_t)rb_input[1015] : 0x17679353);
        #endif
        f >> stdAngle[i];
    }
    for (int i = 0; i < N_ANGLES; i++) {
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_0 = (rb_state10.var_0) - (((rb_state10.var_2) == (0xc945d8a2)) ? (rb_state10.var_1) : (0x8dba0ab7));
        #endif
        #ifdef RACEBENCH_BUG_18
        if ((rb_state18.var_0) == (0x0)) {
            rb_state18.var_5 = 0xab9a5c77;
            rb_state18.var_6 = (rb_state18.var_6) + (rb_state18.var_4);
            rb_state18.var_7 = (rb_state18.var_7) ^ (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_5) : (0x5e9201d));
            rb_state18.var_8 = (rb_state18.var_8) + (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_5) : (0xb0489a87));
            rb_state18.var_9 = (rb_state18.var_9) + (((rb_state18.var_8) == (0x0)) ? (rb_state18.var_6) : (0x723fb09b));
            rb_state18.var_10 = (rb_state18.var_10) ^ (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_7) : (0x3e0bb8de));
            rb_state18.var_11 = (rb_state18.var_11) ^ (rb_state18.var_8);
            rb_state18.var_12 = (rb_state18.var_12) ^ (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_10) : (0x41b6b5f6));
            rb_state18.var_13 = (rb_state18.var_13) + (((rb_state18.var_11) == (0x0)) ? (rb_state18.var_12) : (0xe8a6ea3f));
            rb_state18.var_1 = (rb_state18.var_1) ^ (rb_state18.var_13);
        }
        #endif
        f >> minAngles[i];
    }
    for (int i = 0; i < N_ANGLES; i++) {
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_1 = (rb_state7.var_1) + (((rb_state7.var_0) == (0xcf67c5dc)) ? (rb_state7.var_2) : (0x32a8a055));
        #endif
        #ifdef RACEBENCH_BUG_10
        rb_state10.var_3 = (rb_state10.var_3) - ((0x65dc8d5e) ^ (0x4ff86663));
        #endif
        f >> maxAngles[i];
    }
    return true;
}