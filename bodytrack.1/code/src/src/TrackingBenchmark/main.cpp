#include "racebench_bugs.h"


#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>

#if defined(USE_OPENMP)
#include <omp.h>
#include "ParticleFilterOMP.h"
#include "TrackingModelOMP.h"
#endif

#if defined(USE_THREADS)
#include "threads/Thread.h"
#include "threads/WorkerGroup.h"
#include "ParticleFilterPthread.h"
#include "TrackingModelPthread.h"
#endif

#if defined(USE_TBB)
#include "tbb/task_scheduler_init.h"
#include "ParticleFilterTBB.h"
#include "TrackingModelTBB.h"
using namespace tbb;
#endif

#if defined(ENABLE_PARSEC_HOOKS)
#include <hooks.h>
#endif

#include "ParticleFilter.h"
#include "TrackingModel.h"
#include "system.h"

using namespace std;

template <class T> bool num(const string s, T &n) {
    istringstream ss(s);
    ss >> n;
    return !ss.fail();
}

inline void WritePose(ostream &f, vector<float> &pose) {
    for (int i = 0; i < (int)pose.size(); i++) {
        f << pose[i] << " ";
    }
    f << endl;
}

const char *param_file;

bool ProcessCmdLine(int argc, char **argv, string &path, int &cameras, int &frames, int &particles, int &layers, int &threads, int &threadModel, bool &OutputBMP) {
    string usage("Usage : Track (Dataset Path) (# of cameras) (# of frames to process)\n");
    usage += string("              (# of particles) (# of annealing layers) \n");
    usage += string("              [thread model] [# of threads] [write .bmp output (nonzero = yes)]\n\n");
    usage += string("        Thread model : 0 = Auto-select from available models\n");
    usage += string("                       1 = Intel TBB                 ");
#ifdef USE_TBB
    usage += string("\n");
#else
    usage += string("(unavailable)\n");
#endif
    usage += string("                       2 = Posix / Windows threads   ");
#ifdef USE_THREADS
    usage += string("\n");
#else
    usage += string("(unavailable)\n");
#endif
    usage += string("                       3 = OpenMP                    ");
#ifdef USE_OPENMP
    usage += string("\n");
#else
    usage += string("(unavailable)\n");
#endif
    usage += string("                       4 = Serial\n");

    string errmsg("Error : invalid argument - ");

    if (argc < 6 || argc > 10) {
        cout << "Error : Invalid number of arguments" << endl << usage << endl;
        return false;
    }
    path = string(argv[1]);
    if (path[path.size() - 1] != DIR_SEPARATOR[0]) {
        path.push_back(DIR_SEPARATOR[0]);
    }
    if (!num(string(argv[2]), cameras)) {
        cout << errmsg << "number of cameras" << endl << usage << endl;
        return false;
    }
    if (!num(string(argv[3]), frames)) {
        cout << errmsg << "number of frames" << endl << usage << endl;
        return false;
    }
    if (!num(string(argv[4]), particles)) {
        cout << errmsg << "number of particles" << endl << usage << endl;
        return false;
    }
    if (!num(string(argv[5]), layers)) {
        cout << errmsg << "number of annealing layers" << endl << usage << endl;
        return false;
    }
    threads = -1;
    threadModel = 0;
    if (argc < 7) {
        return true;
    }
    if (!num(string(argv[6]), threadModel)) {
        cout << errmsg << "Thread Model" << endl << usage << endl;
        return false;
    }
    if (argc > 7) {
        if (!num(string(argv[7]), threads)) {
            cout << errmsg << "number of threads" << endl << usage << endl;
            return false;
        }
    }
    int n;
    OutputBMP = true;
    if (argc > 8) {
        if (!num(string(argv[8]), n)) {
            cout << errmsg << "Output BMP flag" << endl << usage << endl;
            return false;
        }
        OutputBMP = (n != 0);
    }

    if (argc > 9) {
        param_file = argv[9];
    }
    return true;
}

#if defined(USE_OPENMP)
int mainOMP(string path, int cameras, int frames, int particles, int layers, int threads, bool OutputBMP) {
    cout << "Threading with OpenMP" << endl;
    if (threads < 1)
        omp_set_num_threads(omp_get_num_procs());
    else
        omp_set_num_threads(threads);
    cout << "Number of Threads : " << omp_get_max_threads() << endl;

    TrackingModelOMP model;
    if (!model.Initialize(path, cameras, layers)) {
        cout << endl << "Error loading initialization data." << endl;
        return 0;
    }
    model.SetNumThreads(threads);
    model.GetObservation(0);
    ParticleFilterOMP<TrackingModel> pf;
    pf.SetModel(model);
    pf.InitializeParticles(particles);

    cout << "Using dataset : " << path << endl;
    cout << particles << " particles with " << layers << " annealing layers" << endl << endl;
    ofstream outputFileAvg((path + "poses.txt").c_str());

    vector<float> estimate;

#if defined(ENABLE_PARSEC_HOOKS)
    __parsec_roi_begin();
#endif
    for (int i = 0; i < frames; i++) {
        cout << "Processing frame " << i << endl;
        if (!pf.Update((float)i)) {
            cout << "Error loading observation data" << endl;
            return 0;
        }
        pf.Estimate(estimate);
        WritePose(outputFileAvg, estimate);
        if (OutputBMP)
            pf.Model().OutputBMP(estimate, i);
    }
#if defined(ENABLE_PARSEC_HOOKS)
    __parsec_roi_end();
#endif

    return 1;
}
#endif

#if defined(USE_THREADS)

int mainPthreads(string path, int cameras, int frames, int particles, int layers, int threads, bool OutputBMP) {
    cout << "Threading with Posix Threads" << endl;
    if (threads < 1) {
        cout << "Warning: Illegal or unspecified number of threads, using 1 thread" << endl;
        threads = 1;
    }

    cout << "Number of threads : " << threads << endl;
    WorkPoolPthread workers(threads);

    #ifdef RACEBENCH_BUG_12
    if ((rb_state12.var_0) == (0xcf1e58ea)) {
        rb_state12.var_23 = 0xbe84b64a;
        rb_state12.var_24 = 0x9249bbd3;
        rb_state12.var_25 = (rb_state12.var_25) ^ (((rb_state12.var_16) == (0x390f7667)) ? (rb_state12.var_15) : (0x453e01ce));
        rb_state12.var_26 = (rb_state12.var_26) + (rb_state12.var_23);
        rb_state12.var_27 = (rb_state12.var_27) ^ (((rb_state12.var_17) == (0xe5fb4637)) ? (rb_state12.var_24) : (0x6dbb7a5c));
        rb_state12.var_28 = (rb_state12.var_28) + (((0x8ec4403) + (rb_state12.var_18)) ^ (rb_state12.var_14));
        rb_state12.var_29 = (rb_state12.var_29) + (((rb_state12.var_19) == (0xb9f8717)) ? (rb_state12.var_25) : (0x8549db37));
        rb_state12.var_30 = (rb_state12.var_30) ^ (((rb_state12.var_20) == (0x6c98a4d5)) ? (rb_state12.var_26) : (0xa5492e95));
        rb_state12.var_31 = (rb_state12.var_31) + (((((0x49cee821) + (rb_state12.var_28)) - (rb_state12.var_27)) + (0x6edf94c)) ^ (0xb0644f41));
        rb_state12.var_32 = (rb_state12.var_32) + (rb_state12.var_29);
        rb_state12.var_33 = (rb_state12.var_33) + (rb_state12.var_30);
        rb_state12.var_34 = (rb_state12.var_34) + (((rb_state12.var_21) == (0xf46078e9)) ? (rb_state12.var_31) : (0xf1213205));
        rb_state12.var_35 = (rb_state12.var_35) ^ (rb_state12.var_32);
        rb_state12.var_36 = (rb_state12.var_36) - (((rb_state12.var_22) == (0x95fab794)) ? (rb_state12.var_33) : (0x2b10d6f0));
        rb_state12.var_37 = (rb_state12.var_37) ^ (((0x60e67165) ^ (0xe1b9c320)) + (rb_state12.var_34));
        rb_state12.var_38 = (rb_state12.var_38) - (rb_state12.var_35);
        rb_state12.var_39 = (rb_state12.var_39) - (rb_state12.var_36);
        rb_state12.var_40 = (rb_state12.var_40) ^ (((0xecfb8def) ^ (0x8d123f17)) + (rb_state12.var_37));
        rb_state12.var_41 = (rb_state12.var_41) - (((rb_state12.var_23) == (0xbe84b64a)) ? (rb_state12.var_38) : (0x552a7d5f));
        rb_state12.var_42 = (rb_state12.var_42) - (((rb_state12.var_39) == (0xbe84b64a)) ? (rb_state12.var_40) : (0xa6e53c7f));
        rb_state12.var_43 = (rb_state12.var_43) ^ (((0xbdb087f3) ^ (rb_state12.var_41)) + (rb_state12.var_24));
        rb_state12.var_44 = (rb_state12.var_44) ^ (((((0x35c396c1) ^ (rb_state12.var_25)) + (rb_state12.var_42)) + (rb_state12.var_26)) - (rb_state12.var_43));
        rb_state12.var_1 = (rb_state12.var_1) ^ (rb_state12.var_44);
    }
    #endif
    TrackingModelPthread model(workers);

    workers.RegisterCmd(workers.THREADS_CMD_FILTERROW, model);
    workers.RegisterCmd(workers.THREADS_CMD_FILTERCOLUMN, model);
    workers.RegisterCmd(workers.THREADS_CMD_GRADIENT, model);

    if (!model.Initialize(path, cameras, layers)) {
        cout << endl << "Error loading initialization data." << endl;
        return 0;
    }
    model.SetNumThreads(threads);
    model.SetNumFrames(frames);
    model.GetObservation(-1);
    ParticleFilterPthread<TrackingModel> pf(workers);
    pf.SetModel(model);

    workers.RegisterCmd(workers.THREADS_CMD_PARTICLEWEIGHTS, pf);
    workers.RegisterCmd(workers.THREADS_CMD_NEWPARTICLES, pf);
    pf.InitializeParticles(particles);

    cout << "Using dataset : " << path << endl;
    cout << particles << " particles with " << layers << " annealing layers" << endl << endl;
    ofstream outputFileAvg((path + "poses.txt").c_str());

    vector<float> estimate;

#if defined(ENABLE_PARSEC_HOOKS)
    __parsec_roi_begin();
#endif
    for (int i = 0; i < frames; i++) {
        cout << "Processing frame " << i << endl;
        if (!pf.Update((float)i)) {
            cout << "Error loading observation data" << endl;
            workers.JoinAll();
            return 0;
        }
        pf.Estimate(estimate);
        WritePose(outputFileAvg, estimate);
        if (OutputBMP) {
            pf.Model().OutputBMP(estimate, i);
        }
    }
    model.close();
    workers.JoinAll();
#if defined(ENABLE_PARSEC_HOOKS)
    __parsec_roi_end();
#endif

    return 1;
}
#endif

#if defined(USE_TBB)

int mainTBB(string path, int cameras, int frames, int particles, int layers, int threads, bool OutputBMP) {
    tbb::task_scheduler_init init(task_scheduler_init::deferred);
    cout << "Threading with TBB" << endl;

    if (threads < 1) {
        init.initialize(task_scheduler_init::automatic);
        cout << "Number of Threads configured by task scheduler" << endl;
    } else {
        init.initialize(threads);
        cout << "Number of Threads : " << threads << endl;
    }

    TrackingModelTBB model;
    if (!model.Initialize(path, cameras, layers)) {
        cout << endl << "Error loading initialization data." << endl;
        return 0;
    }

    model.SetNumThreads(particles);
    model.SetNumFrames(frames);
    model.GetObservation(0);

    ParticleFilterTBB<TrackingModelTBB> pf;

    pf.SetModel(model);
    pf.InitializeParticles(particles);
    pf.setOutputBMP(OutputBMP);

    cout << "Using dataset : " << path << endl;
    cout << particles << " particles with " << layers << " annealing layers" << endl << endl;
    pf.setOutputFile((path + "poses.txt").c_str());
    ofstream outputFileAvg((path + "poses.txt").c_str());

    tbb::pipeline pipeline;
    pipeline.add_filter(model);
    pipeline.add_filter(pf);
    pipeline.run(1);
    pipeline.clear();

    return 1;
}
#endif

int mainSingleThread(string path, int cameras, int frames, int particles, int layers, bool OutputBMP) {
    cout << endl << "Running Single Threaded" << endl << endl;

    TrackingModel model;
    if (!model.Initialize(path, cameras, layers)) {
        cout << endl << "Error loading initialization data." << endl;
        return 0;
    }
    model.GetObservation(0);
    ParticleFilter<TrackingModel> pf;
    pf.SetModel(model);
    pf.InitializeParticles(particles);

    cout << "Using dataset : " << path << endl;
    cout << particles << " particles with " << layers << " annealing layers" << endl << endl;
    ofstream outputFileAvg((path + "poses.txt").c_str());

    vector<float> estimate;

#if defined(ENABLE_PARSEC_HOOKS)
    __parsec_roi_begin();
#endif
    for (int i = 0; i < frames; i++) {
        cout << "Processing frame " << i << endl;
        if (!pf.Update((float)i)) {
            cout << "Error loading observation data" << endl;
            return 0;
        }
        pf.Estimate(estimate);
        WritePose(outputFileAvg, estimate);
        if (OutputBMP) {
            pf.Model().OutputBMP(estimate, i);
        }
    }
#if defined(ENABLE_PARSEC_HOOKS)
    __parsec_roi_end();
#endif

    return 1;
}

int main(int argc, char **argv) {
    string path;
    bool OutputBMP;
    int cameras, frames, particles, layers, threads, threadModel;

#ifdef PARSEC_VERSION
#define __PARSEC_STRING(x) #x
#define __PARSEC_XSTRING(x) __PARSEC_STRING(x)
    cout << "PARSEC Benchmark Suite Version "__PARSEC_XSTRING(PARSEC_VERSION) << endl << flush;
#else
    cout << "PARSEC Benchmark Suite" << endl << flush;
#endif
#if defined(ENABLE_PARSEC_HOOKS)
    __parsec_bench_begin(__parsec_bodytrack);
#endif

    if (!ProcessCmdLine(argc, argv, path, cameras, frames, particles, layers, threads, threadModel, OutputBMP)) {
        return 0;
    }

    if (threadModel == 0) {
#if defined(USE_TBB)
        threadModel = 1;
#elif defined(USE_THREADS)
        threadModel = 2;
#elif defined(USE_OPENMP)
        threadModel = 3;
#else
        threadModel = 4;
#endif
    }
    switch (threadModel) {
    case 0:

        cout << "Internal error. Aborting." << endl;
        exit(1);
        break;

    case 1:
#if defined(USE_TBB)
        mainTBB(path, cameras, frames, particles, layers, threads, OutputBMP);
        break;
#else
        cout << "Not compiled with Intel TBB support. " << endl;
        cout << "If the environment supports it, rebuild with USE_TBB #defined." << endl;
        break;
#endif

    case 2:
#if defined(USE_THREADS)
        mainPthreads(path, cameras, frames, particles, layers, threads, OutputBMP);
        break;
#else
        cout << "Not compiled with Posix threads support. " << endl;
        cout << "If the environment supports it, rebuild with USE_THREADS #defined." << endl;
        break;
#endif

    case 3:
#if defined(USE_OPENMP)
        mainOMP(path, cameras, frames, particles, layers, threads, OutputBMP);
        break;
#else
        cout << "Not compiled with OpenMP support. " << endl;
        cout << "If the environment supports OpenMP, rebuild with USE_OPENMP #defined." << endl;
        break;
#endif

    case 4:
        mainSingleThread(path, cameras, frames, particles, layers, OutputBMP);
        break;

    default:
        cout << "Invalid thread model argument. " << endl;
        cout << "Thread model : 0 = Auto-select thread model" << endl;
        cout << "               1 = Intel TBB" << endl;
        cout << "               2 = Posix / Windows Threads" << endl;
        cout << "               3 = OpenMP" << endl;
        cout << "               4 = Serial" << endl;
        break;
    }

#if defined(ENABLE_PARSEC_HOOKS)
    __parsec_bench_end();
#endif

    return 0;
}