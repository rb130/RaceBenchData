

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "AnnealingFactor.h"
#include <math.h>

float delta_alpha(float beta, std::vector<float> &ets, float alpha_desired) {
    int N = (int)ets.size();
    float B = 0;
    float F = 0;
    float ei;
    double v;

    for (int i = 0; i < N; i++) {
        v = beta * ets[i];
        if (v > LOG_MAX_FLOAT) {
            return (-alpha_desired);
        }
        ei = (float)exp(v);
        B += ei;
        F += (ei * ei);
    }
    return (((B * B / F) / N) - alpha_desired);
};

float BetaAnnealingFactor(std::vector<float> &ets, float alpha_desired, float beta_min, float beta_max) {
    int n_iterations = 0;

    float delta_alpha_min = delta_alpha(beta_min, ets, alpha_desired);
    float delta_alpha_max = delta_alpha(beta_max, ets, alpha_desired);

    if (((delta_alpha_min > 0) && (delta_alpha_max > 0)) || ((delta_alpha_min < 0) && (delta_alpha_max < 0))) {
        return 1.0f;
    }

    float beta = (beta_min + beta_max) / 2;
    float delta_alpha_beta = delta_alpha(beta, ets, alpha_desired);

    while (((delta_alpha_beta < 0.0 ? -delta_alpha_beta : delta_alpha_beta) > ALPHA_PRECISION) && (n_iterations < N_ITER_MAX)) {
        if (((delta_alpha_min > 0) && (delta_alpha_beta > 0)) || ((delta_alpha_min <= 0) && (delta_alpha_beta < 0))) {
            beta_min = beta;
            delta_alpha_min = delta_alpha_beta;
        } else {
            beta_max = beta;
            delta_alpha_max = delta_alpha_beta;
        }
        beta = (beta_min + beta_max) / 2;
        delta_alpha_beta = delta_alpha(beta, ets, alpha_desired);
        n_iterations++;
    }
    return beta;
}

void set_ets(std::vector<float> &ets) {

    float vals[25] = {0.0431287287f, 0.0000000000f, 0.1890535963f, 0.1314251348f, 0.1744840913f, 0.1724611824f, 0.1665666636f, 0.1008506786f, 0.1570010271f, 0.1469321041f, 0.0941946672f, 0.0873843544f, 0.1011394290f, 0.1401914942f, 0.0944126324f, 0.1747114664f, 0.0861499931f, 0.1744767988f, 0.1881925406f, 0.0845404237f, 0.1416303064f, 0.1722650238f, 0.0353243611f, 0.1919276523f, 0.1255045773f};

    ets.resize(25);
    for (int i = 0; i < 25; i++) {
        ets[i] = vals[i];
    }
}
