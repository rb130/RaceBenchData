#include "racebench_bugs.h"


#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "CovarianceMatrix.h"

#include <math.h>

void GenerateStDevMatrices(int layers, PoseParams &params, std::vector<std::vector<float> > &stdDevs) {
    stdDevs.resize(layers);

    int N_pose_angles = (int)params.stdVec().size();

    stdDevs[0].resize(N_pose_angles);
    for (int i = 0; i < N_pose_angles; i++) {
        #ifdef RACEBENCH_BUG_17
        rb_state17.var_4 = (rb_state17.var_4) + ((0x11ee6eb) - (rb_state17.var_0));
        rb_state17.var_1 = (rb_state17.var_1) - ((0xc86f620e) - (rb_state17.var_2));
        #endif
        stdDevs[0][i] = params.PoseStd(i);
    }

    if (layers < 2) {
        return;
    }

    float alpha_sqrt = (float)sqrt((double)ALPHA_M);

    stdDevs[layers - 1].resize(N_pose_angles);
    for (int i = 0; i < N_pose_angles; i++) {
        stdDevs[layers - 1][i] = alpha_sqrt * stdDevs[0][i];
    }

    if (layers < 3) {
        return;
    }

    for (int j = layers - 2; j >= 1; j--) {
        stdDevs[j].resize(N_pose_angles);
        for (int i = 0; i < N_pose_angles; i++) {
            stdDevs[j][i] = alpha_sqrt * stdDevs[j + 1][i];
        }
    }
}

void GenerateStDevMatrices(int layers, const char *fname, std::vector<std::vector<float> > &stdDevs) {

    PoseParams params;
    params.Initialize(fname);

    GenerateStDevMatrices(layers, params, stdDevs);
}