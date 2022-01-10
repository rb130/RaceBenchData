#include "racebench_bugs.h"


#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "CameraModel.h"
#include <iostream>
#include <fstream>

using namespace std;

#define DMatrixf DMatrix<float>

bool Camera::LoadParams(const char *fname) {
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_3 = (rb_state10.var_3) ^ (0x2a7f749d);
    #endif
    ifstream f(fname);
    if (!f.is_open()) {
        cout << "Unable to open camera calibration file : " << fname << endl;
        return false;
    }
    f >> fc.x >> fc.y >> cc.x >> cc.y >> alpha_c;
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_2 = (rb_state10.var_2) + ((0x87913077) - (rb_state10.var_3));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_3 = (rb_state13.var_3) - (rb_state13.var_0);
    #endif
    f >> kc[0] >> kc[1] >> kc[2] >> kc[3] >> kc[4];
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_2 = (rb_state9.var_2) + ((0x9c902e48) - (rb_state9.var_1));
    #endif
    f >> Rc_ext[0][0] >> Rc_ext[0][1] >> Rc_ext[0][2] >> Rc_ext[1][0] >> Rc_ext[1][1] >> Rc_ext[1][2] >> Rc_ext[2][0] >> Rc_ext[2][1] >> Rc_ext[2][2];
    f >> Tc_ext.x >> Tc_ext.y >> Tc_ext.z;
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_2 = (rb_state7.var_2) ^ (1369 < rb_input_size ? (uint32_t)rb_input[1369] : 0xf4c7e4c9);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_2 = (rb_state17.var_2) + (rb_state17.var_2);
    #endif
    f >> omc_ext.x >> omc_ext.y >> omc_ext.z;

    mc_ext(0, 0) = Rc_ext[0][0];
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) ^ (rb_state13.var_2);
    #endif
    mc_ext(0, 1) = Rc_ext[0][1];
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_1) == (0x1d0927b2)) {
        pthread_mutex_lock(&(rb_state13.lock_13));
        rb_state13.var_4 = rb_state13.var_2;
        pthread_mutex_unlock(&(rb_state13.lock_13));
    }
    if ((rb_state13.var_1) == (0x1d0927b2)) {
        pthread_mutex_lock(&(rb_state13.lock_13));
        if (!((rb_state13.var_2) == (rb_state13.var_4))) {
            racebench_trigger(13);
        }
        pthread_mutex_unlock(&(rb_state13.lock_13));
    }
    #endif
    mc_ext(0, 2) = Rc_ext[0][2];
    mc_ext(0, 3) = Tc_ext.x;
    mc_ext(1, 0) = Rc_ext[1][0];
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_1 = (rb_state7.var_1) ^ (796 < rb_input_size ? (uint32_t)rb_input[796] : 0x28e078b6);
    #endif
    mc_ext(1, 1) = Rc_ext[1][1];
    mc_ext(1, 2) = Rc_ext[1][2];
    mc_ext(1, 3) = Tc_ext.y;
    mc_ext(2, 0) = Rc_ext[2][0];
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_3 = (rb_state17.var_3) ^ (148 < rb_input_size ? (uint32_t)rb_input[148] : 0xc3935712);
    #endif
    mc_ext(2, 1) = Rc_ext[2][1];
    mc_ext(2, 2) = Rc_ext[2][2];
    mc_ext(2, 3) = Tc_ext.z;
    DMatrixf m2 = Inverse(mc_ext);

    eye.x = m2(0, 3);
    eye.y = m2(1, 3);
    eye.z = m2(2, 3);
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (1423 < rb_input_size ? (uint32_t)rb_input[1423] : 0x3b679cb5);
    #endif
    return true;
}