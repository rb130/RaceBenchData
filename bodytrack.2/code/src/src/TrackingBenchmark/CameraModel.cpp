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
    ifstream f(fname);
    if (!f.is_open()) {
        cout << "Unable to open camera calibration file : " << fname << endl;
        return false;
    }
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_1 = (rb_state6.var_1) + (0x49a6e575);
    #endif
    f >> fc.x >> fc.y >> cc.x >> cc.y >> alpha_c;
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (((rb_state4.var_0) == (0xa45061a1)) ? (rb_state4.var_0) : (0xd5992ae3));
    #endif
    f >> kc[0] >> kc[1] >> kc[2] >> kc[3] >> kc[4];
    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0x4a817f95));
    #endif
    f >> Rc_ext[0][0] >> Rc_ext[0][1] >> Rc_ext[0][2] >> Rc_ext[1][0] >> Rc_ext[1][1] >> Rc_ext[1][2] >> Rc_ext[2][0] >> Rc_ext[2][1] >> Rc_ext[2][2];
    f >> Tc_ext.x >> Tc_ext.y >> Tc_ext.z;
    f >> omc_ext.x >> omc_ext.y >> omc_ext.z;

    mc_ext(0, 0) = Rc_ext[0][0];
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_10 = (rb_state17.var_10) ^ (rb_state17.var_11);
    #endif
    mc_ext(0, 1) = Rc_ext[0][1];
    mc_ext(0, 2) = Rc_ext[0][2];
    mc_ext(0, 3) = Tc_ext.x;
    mc_ext(1, 0) = Rc_ext[1][0];
    mc_ext(1, 1) = Rc_ext[1][1];
    mc_ext(1, 2) = Rc_ext[1][2];
    mc_ext(1, 3) = Tc_ext.y;
    mc_ext(2, 0) = Rc_ext[2][0];
    mc_ext(2, 1) = Rc_ext[2][1];
    mc_ext(2, 2) = Rc_ext[2][2];
    mc_ext(2, 3) = Tc_ext.z;
    DMatrixf m2 = Inverse(mc_ext);

    #ifdef RACEBENCH_BUG_17
    rb_state17.var_9 = (rb_state17.var_9) ^ (0x21b668f);
    #endif
    eye.x = m2(0, 3);
    eye.y = m2(1, 3);
    eye.z = m2(2, 3);
    return true;
}