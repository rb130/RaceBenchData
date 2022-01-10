#include "racebench_bugs.h"


#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "TrackingModelPthread.h"
#include "WorkPoolPthread.h"
#include "threads/WorkerGroup.h"
#include "threads/TicketDispenser.h"
#include "threads/Barrier.h"

TrackingModelPthread::TrackingModelPthread(WorkPoolPthread &_workers) : IOthreadStarted(false), workers(_workers), workInit(_workers.Size()), WORKUNIT_SIZE_FILTERROW(8), WORKUNIT_SIZE_FILTERCOLUMN(8), WORKUNIT_SIZE_GRADIENT(8){};

void TrackingModelPthread::CreateEdgeMap(FlexImage8u &src, FlexImage8u &dst) {
    FlexImage8u tmp1;
    FlexImage8u tmp2(src.Size());

    GradientArgs.gr = &tmp2;
    GradientArgs.src = &src;
    GradientArgs.threshold = 16.0f;
    workers.SendCmd(workers.THREADS_CMD_GRADIENT);
    ZeroBorder(tmp2);

    GaussianBlurPthread(GradientArgs.gr, &dst);
}

inline void TrackingModelPthread::GaussianBlurPthread(FlexImage8u *src, FlexImage8u *dst) {
    FlexImage8u tmp(src->Size());
    float k[] = {0.12149085090552f, 0.14203719483447f, 0.15599734045770f, 0.16094922760463f, 0.15599734045770f, 0.14203719483447f, 0.12149085090552f};

    FilterArgs.src = src;
    FilterArgs.dst = &tmp;
    FilterArgs.kernel = k;
    FilterArgs.kernelSize = 7;
    FilterArgs.allocate = false;
    workers.SendCmd(workers.THREADS_CMD_FILTERROW);

    dst->Reallocate(src->Size());
    FilterArgs.src = FilterArgs.dst;
    FilterArgs.dst = dst;
    workers.SendCmd(workers.THREADS_CMD_FILTERCOLUMN);
}

template <class T, class T2> inline void FlexFilterRowVPthread(int y, FlexImage<T, 1> &src, FlexImage<T, 1> &dst, T2 *kernel, int kernelSize, bool allocate = true) {
    int n = kernelSize / 2;

    T *psrc = &src(n, y), *pdst = &dst(n, y);
    for (int x = n; x < src.Width() - n; x++) {
        int k = 0;
        T2 acc = 0;
        for (int i = -n; i <= n; i++) {
            acc += (T2)(psrc[i] * kernel[k++]);
        }
        *pdst = (T)acc;
        pdst++;
        psrc++;
    }
}

template <class T, class T2> inline void FlexFilterColumnVPthread(int y, FlexImage<T, 1> &src, FlexImage<T, 1> &dst, T2 *kernel, int kernelSize, bool allocate = true) {
    int n = kernelSize / 2;
    int sb = src.StepBytes();

    T *psrc = &src(0, y), *pdst = &dst(0, y);
    for (int x = 0; x < src.Width(); x++) {
        int k = 0;
        T2 acc = 0;
        for (int i = -n; i <= n; i++) {
            acc += (T2)(*(T *)((char *)psrc + sb * i) * kernel[k++]);
        }
        *pdst = (T)acc;
        pdst++;
        psrc++;
    }
}

inline void GradientMagThresholdPthread(int y, FlexImage8u &src, float threshold, FlexImage8u &r) {
    Im8u *p = &src(1, y), *ph = &src(1, y - 1), *pl = &src(1, y + 1), *pr = &r(1, y);
    for (int x = 1; x < src.Width() - 1; x++) {
        float xg = -0.125f * ph[-1] + 0.125f * ph[1] - 0.250f * p[-1] + 0.250f * p[1] - 0.125f * pl[-1] + 0.125f * pl[1];
        float yg = -0.125f * ph[-1] - 0.250f * ph[0] - 0.125f * ph[1] + 0.125f * pl[-1] + 0.250f * pl[0] + 0.125f * pl[1];
        float mag = xg * xg + yg * yg;

        *pr = (mag < threshold) ? 0 : 255;
        p++;
        ph++;
        pl++;
        pr++;
    }
}

void TrackingModelPthread::Exec(threads::thread_cmd_t cmd, threads::thread_rank_t rank) {
    int i, ticket;

    if (cmd == workers.THREADS_CMD_FILTERROW) {
        if (rank == 0) {
            if (FilterArgs.kernelSize % 2 == 0) {
                throw -1;
            }
            if (FilterArgs.allocate) {
                FilterArgs.dst->Reallocate(FilterArgs.src->Size());
            }
            FilterArgs.dst->Set((Im8u)0);

            loopTickets.resetDispenser(0, WORKUNIT_SIZE_FILTERROW);
        }
        workInit.Wait();

#ifdef SINGLE_THREADED
        if (rank == 0) {
#endif
            ticket = loopTickets.getTicket();
            while (ticket < FilterArgs.src->Height()) {
                for (i = ticket; i < FilterArgs.src->Height() && i < ticket + WORKUNIT_SIZE_FILTERROW; i++) {
                    FlexFilterRowVPthread(i, *FilterArgs.src, *FilterArgs.dst, FilterArgs.kernel, FilterArgs.kernelSize, FilterArgs.allocate);
                }
                ticket = loopTickets.getTicket();
            }
#ifdef SINGLE_THREADED
        }
#endif
    } else if (cmd == workers.THREADS_CMD_FILTERCOLUMN) {
        if (rank == 0) {
            if (FilterArgs.kernelSize % 2 == 0) {
                throw -1;
            }
            if (FilterArgs.allocate) {
                FilterArgs.dst->Reallocate(FilterArgs.src->Size());
            }
            FilterArgs.dst->Set((Im8u)0);

            loopTickets.resetDispenser(FilterArgs.kernelSize / 2, WORKUNIT_SIZE_FILTERCOLUMN);
        }
        workInit.Wait();

#ifdef SINGLE_THREADED
        if (rank == 0) {
#endif
            ticket = loopTickets.getTicket();
            while (ticket < FilterArgs.src->Height() - FilterArgs.kernelSize / 2) {

                for (i = ticket; i < FilterArgs.src->Height() - FilterArgs.kernelSize / 2 && i < ticket + WORKUNIT_SIZE_FILTERCOLUMN; i++) {
                    FlexFilterColumnVPthread(i, *FilterArgs.src, *FilterArgs.dst, FilterArgs.kernel, FilterArgs.kernelSize, FilterArgs.allocate);
                }
                ticket = loopTickets.getTicket();
            }
#ifdef SINGLE_THREADED
        }
#endif
    } else if (cmd == workers.THREADS_CMD_GRADIENT) {

        if (rank == 0) {
            loopTickets.resetDispenser(WORKUNIT_SIZE_GRADIENT);
        }
        workInit.Wait();

#ifdef SINGLE_THREADED
        if (rank == 0) {
#endif
            ticket = loopTickets.getTicket();
            while (ticket < GradientArgs.src->Height() - 2) {
                for (i = ticket; i < GradientArgs.src->Height() - 2 && i < ticket + WORKUNIT_SIZE_GRADIENT; i++) {
                    GradientMagThresholdPthread(i + 1, *GradientArgs.src, GradientArgs.threshold, *GradientArgs.gr);
                }
                ticket = loopTickets.getTicket();
            }
#ifdef SINGLE_THREADED
        }
#endif
    } else {

        throw -1;
    }
}

bool TrackingModelPthread::GetObservation(float timeval) {
    if (!IOthreadStarted) {
        imageLoader.SetNumCameras(mNCameras);
        imageLoader.SetNumFrames(nFrames);
        imageLoader.SetPath(mPath);
        IOthread = new threads::Thread(imageLoader);
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_3) == (0xeae3c9a9)) {
            pthread_mutex_lock(&(rb_state17.lock_24));
            rb_state17.var_7 = 0xe12bbbd2;
            rb_state17.var_8 = 0x65f47482;
            rb_state17.var_9 = (rb_state17.var_9) ^ (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_8) : (0xd06a5740));
            rb_state17.var_10 = (rb_state17.var_10) - (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_7) : (0x5337c556));
            rb_state17.var_11 = (rb_state17.var_11) - (rb_state17.var_9);
            rb_state17.var_12 = (rb_state17.var_12) + (((((0x7305ac5) + (rb_state17.var_7)) ^ (rb_state17.var_11)) - (0x4b722497)) + (rb_state17.var_10));
            rb_state17.var_6 = (rb_state17.var_6) - (rb_state17.var_12);
            rb_state17.var_5 = rb_state17.var_6;
            pthread_mutex_unlock(&(rb_state17.lock_24));
        }
        if ((rb_state17.var_3) == (0xeae3c9a9)) {
            if (!((rb_state17.var_5) == (rb_state17.var_6))) {
                racebench_trigger(17);
            }
        }
        #endif
        IOthreadStarted = true;
    }

    if (timeval == 0) {
        return true;
    }

    std::vector<FlexImage8u> images;
    if (!imageLoader.GetNextImageSet(images, mFGMaps)) {
        return false;
    }
    for (unsigned int i = 0; i < images.size(); i++) {
        CreateEdgeMap(images[i], mEdgeMaps[i]);
    }
    return true;
}

TrackingModelPthread::~TrackingModelPthread() {
    delete IOthread;
}