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
    rb_state10.var_8 = (rb_state10.var_8) ^ (0xd1d8e2b6);
    #endif
    ifstream f(fname);
    if (!f.is_open()) {
        cout << "Unable to open camera calibration file : " << fname << endl;
        return false;
    }
    f >> fc.x >> fc.y >> cc.x >> cc.y >> alpha_c;
    f >> kc[0] >> kc[1] >> kc[2] >> kc[3] >> kc[4];
    f >> Rc_ext[0][0] >> Rc_ext[0][1] >> Rc_ext[0][2] >> Rc_ext[1][0] >> Rc_ext[1][1] >> Rc_ext[1][2] >> Rc_ext[2][0] >> Rc_ext[2][1] >> Rc_ext[2][2];
    f >> Tc_ext.x >> Tc_ext.y >> Tc_ext.z;
    f >> omc_ext.x >> omc_ext.y >> omc_ext.z;

    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_2) == (0x38)) {
        if ((rb_state11.var_3) != (0x0)) {
            if (!((rb_state11.var_3) == (rb_state11.var_24))) {
                racebench_trigger(11);
            }
        }
    }
    #endif
    mc_ext(0, 0) = Rc_ext[0][0];
    mc_ext(0, 1) = Rc_ext[0][1];
    mc_ext(0, 2) = Rc_ext[0][2];
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (rb_state11.var_1);
    #endif
    mc_ext(0, 3) = Tc_ext.x;
    mc_ext(1, 0) = Rc_ext[1][0];
    mc_ext(1, 1) = Rc_ext[1][1];
    mc_ext(1, 2) = Rc_ext[1][2];
    mc_ext(1, 3) = Tc_ext.y;
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_3 = (rb_state14.var_3) + (0xa3034f97);
    #endif
    mc_ext(2, 0) = Rc_ext[2][0];
    mc_ext(2, 1) = Rc_ext[2][1];
    mc_ext(2, 2) = Rc_ext[2][2];
    mc_ext(2, 3) = Tc_ext.z;
    DMatrixf m2 = Inverse(mc_ext);

    eye.x = m2(0, 3);
    eye.y = m2(1, 3);
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) + (rb_state11.var_2);
    #endif
    eye.z = m2(2, 3);
    return true;
}