#include "racebench_bugs.h"


#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <iostream>
#include <iomanip>
#include "FlexImage.h"
#include "FlexDataExchange.h"

using namespace std;

#ifndef min
#define min(x, y) (((x) < (y)) ? (x) : (y))
#endif
#ifndef max
#define max(x, y) (((x) > (y)) ? (x) : (y))
#endif

template <class T, int C> FlexImage<T, C>::FlexImage() {
    mStore = NULL;
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state17.lock_26));
        rb_state17.var_3 = (rb_state17.var_3) - ((0xb0c64a41) ^ (rb_state17.var_3));
        rb_state17.var_4 = (rb_state17.var_4) - (((rb_state17.var_1) == (0x0)) ? (rb_state17.var_2) : (0xc9ef345d));
        rb_state17.var_5 = (rb_state17.var_5) - (((0xf41f89da) ^ (rb_state17.var_3)) - (rb_state17.var_4));
        rb_state17.var_6 = (rb_state17.var_6) - (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_4) : (0x4621138a));
        rb_state17.var_7 = (rb_state17.var_7) + (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_5) : (0x642bc16c));
        rb_state17.var_8 = (rb_state17.var_8) - (((rb_state17.var_7) == (0x0)) ? (rb_state17.var_6) : (0xf9282c8b));
        rb_state17.var_9 = (rb_state17.var_9) ^ (rb_state17.var_7);
        rb_state17.var_10 = (rb_state17.var_10) - (((0x3efdd4a) + (rb_state17.var_8)) + (rb_state17.var_8));
        rb_state17.var_11 = (rb_state17.var_11) - (rb_state17.var_9);
        rb_state17.var_12 = (rb_state17.var_12) - (rb_state17.var_10);
        rb_state17.var_13 = (rb_state17.var_13) - (((0xb62b960c) - (0x3b5abacd)) + (rb_state17.var_11));
        rb_state17.var_14 = (rb_state17.var_14) ^ (((0xe2b6fab1) + (0x8fcdc77e)) + (rb_state17.var_12));
        rb_state17.var_15 = (rb_state17.var_15) - (rb_state17.var_13);
        rb_state17.var_16 = (rb_state17.var_16) ^ (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_14) : (0xe9f24e2));
        rb_state17.var_17 = (rb_state17.var_17) + (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_15) : (0x8db2d5d8));
        rb_state17.var_18 = (rb_state17.var_18) + (rb_state17.var_16);
        rb_state17.var_19 = (rb_state17.var_19) - (((rb_state17.var_11) == (0x0)) ? (rb_state17.var_17) : (0x4cde828c));
        rb_state17.var_20 = (rb_state17.var_20) - (rb_state17.var_18);
        rb_state17.var_21 = (rb_state17.var_21) - (((rb_state17.var_19) == (0x0)) ? (rb_state17.var_20) : (0xa616a8b9));
        rb_state17.var_2 = (rb_state17.var_2) - (rb_state17.var_21);
        rb_state17.var_1 = rb_state17.var_2;
        pthread_mutex_unlock(&(rb_state17.lock_26));
    }
    #endif
    mSize.width = 0;
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x0)) {
        if (!((rb_state17.var_1) == (rb_state17.var_2))) {
            if ((rb_state17.var_27) != (0x0)) {
                if (!((rb_state17.var_27) == (rb_state17.var_63))) {
                    racebench_trigger(17);
                }
            }
        }
    }
    #endif
    mSize.height = 0;
    mData = NULL;
    mBpp = sizeof(T) * C;
    mStatus = FlexStsAllocationFail;
}

template <class T, int C> FlexImage<T, C>::FlexImage(int width, int height) {
    mStore = NULL;
    mSize.width = width;
    mSize.height = height;
    Allocate();
}

template <class T, int C> FlexImage<T, C>::FlexImage(FISize size) {
    mStore = NULL;
    mSize = size;
    Allocate();
}

template <class T, int C> FlexImage<T, C>::FlexImage(int width, int height, T InitVal) {
    mStore = NULL;
    mSize.width = width;
    mSize.height = height;
    Allocate();
    if (mStatus == FlexStsNoError) {
        Set(InitVal);
    }
}

template <class T, int C> FlexImage<T, C>::FlexImage(FISize size, T InitVal) {
    mStore = NULL;
    mSize = size;
    Allocate();
    if (mStatus == FlexStsNoError) {
        Set(InitVal);
    }
}

template <class T, int C> FlexImage<T, C>::FlexImage(const FlexImage<T, C> &src) {
    mStore = NULL;
    CreateSubImage(src, 0, 0, src.Width(), src.Height());
}

template <class T, int C> FlexImage<T, C>::FlexImage(const FlexImage<T, C> &src, const FIRect &r) {
    mStore = NULL;
    CreateSubImage(src, r.x, r.y, r.width, r.height);
}

template <class T, int C> FlexImage<T, C>::FlexImage(const FlexImage<T, C> &src, int x, int y, int width, int height) {
    mStore = NULL;
    CreateSubImage(src, x, y, width, height);
}

template <class T, int C> void FlexImage<T, C>::CreateSubImage(const FlexImage<T, C> &src, const FIRect &r) {
    CreateSubImage(src, r.x, r.y, r.width, r.height);
}

template <class T, int C> void FlexImage<T, C>::CreateSubImage(FlexImageStore<T, C> &store, FIRect &r) {
    CreateSubImage(store, r.x, r.y, r.width, r.height);
}

template <class T, int C> void FlexImage<T, C>::operator=(const FlexImage<T, C> &src) {
    CreateSubImage(src, src.ImgRect());
}

bool ValidRect(FISize size, int &x, int &y, int &width, int &height) {
    if ((x < 0) || (y < 0) || ((x + width) > size.width) || ((y + height) > size.height)) {
        int x2 = x + width;
        int y2 = y + height;
        x = max(0, x);
        x = min(x, size.width);
        y = max(0, y);
        y = min(y, size.height);
        x2 = max(0, x2);
        x2 = min(x2, size.width);
        y2 = max(0, y2);
        y2 = min(y2, size.height);
        width = x2 - x;
        height = y2 - y;
        return (false);
    }
    return (true);
}

template <class T, int C> FlexImage<T, C>::FlexImage(FlexImageStore<T, C> &store, int x, int y, int width, int height) {
    mStore = NULL;
    CreateSubImage(store, x, y, width, height);
}

template <class T, int C> void FlexImage<T, C>::CreateSubImage(FlexImageStore<T, C> &store, int x, int y, int width, int height) {
    mStatus = FlexStsNoError;

    if (!ValidRect(store.Size(), x, y, width, height)) {
        if (width == 0 || height == 0) {
            x = 0;
            y = 0;
            width = 0;
            height = 0;
            mStatus = FlexStsInvalidSubImage;
        } else {
            mStatus = FlexStsSubImageTruncated;
        }
    }

    mSize.width = width;
    mSize.height = height;

    if (mStore != &store) {
        DecReference();
        mStore = &store;
        mStore->IncReferences();
    }
    mStepBytes = store.StepBytes();
    mBpp = sizeof(T) * C;

    mData = (T *)((char *)store.Data() + mStepBytes * y + mBpp * x);
}

template <class T, int C> void FlexImage<T, C>::CreateSubImage(const FlexImage<T, C> &img, int x, int y, int width, int height) {
    mStatus = FlexStsNoError;

    if (!ValidRect(img.Size(), x, y, width, height)) {
        if (width == 0 || height == 0) {
            x = 0;
            y = 0;
            width = 0;
            height = 0;
            mStatus = FlexStsInvalidSubImage;
        } else {
            mStatus = FlexStsSubImageTruncated;
        }
    }

    mSize.width = width;
    mSize.height = height;

    if (mStore != img.ImageStore()) {
        DecReference();
        mStore = (FlexImageStore<T, C> *)img.ImageStore();
        mStore->IncReferences();
    }
    mStepBytes = img.StepBytes();
    mBpp = sizeof(T) * C;

    mData = (T *)((char *)img.Data() + mStepBytes * y + mBpp * x);
}

template <class T, int C> FlexImage<T, C> FlexImage<T, C>::operator()(int x, int y, int width, int height) {
    FlexImage<T, C> tmp;
    tmp.CreateSubImage(*this, x, y, width, height);
    return tmp;
}

template <class T, int C> void FlexImage<T, C>::DecReference() {
    if (mStore != NULL) {
        if (mStore->DecReferences()) {
            delete mStore;
        }
    }
    mStore = NULL;
}

template <class T, int C> FlexImage<T, C>::~FlexImage() {
    #ifdef RACEBENCH_BUG_7
    if ((rb_state7.var_0) == (0x0)) {
        rb_state7.var_7 = 0x1b522f70;
        rb_state7.var_8 = (rb_state7.var_8) - (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_5) : (0x65668b8c));
        rb_state7.var_9 = (rb_state7.var_9) + (rb_state7.var_7);
        rb_state7.var_10 = (rb_state7.var_10) - (((rb_state7.var_4) == (0x0)) ? (rb_state7.var_8) : (0x4d2cedff));
        rb_state7.var_11 = (rb_state7.var_11) + (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_9) : (0x35c3302f));
        rb_state7.var_12 = (rb_state7.var_12) ^ (rb_state7.var_10);
        rb_state7.var_13 = (rb_state7.var_13) ^ (((0x2eb1daea) ^ (rb_state7.var_11)) - (rb_state7.var_8));
        rb_state7.var_14 = (rb_state7.var_14) ^ (rb_state7.var_12);
        rb_state7.var_15 = (rb_state7.var_15) + (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_13) : (0x23c5a52));
        rb_state7.var_16 = (rb_state7.var_16) + (((((0x92013cc7) + (rb_state7.var_11)) - (rb_state7.var_15)) ^ (rb_state7.var_14)) - (rb_state7.var_10));
        rb_state7.var_6 = (rb_state7.var_6) - (rb_state7.var_16);
        rb_state7.var_1 = rb_state7.var_6;
    }
    if ((rb_state7.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state7.lock_33));
        rb_state7.var_18 = 0x76cf3214;
        rb_state7.var_19 = 0x1965aa34;
        rb_state7.var_20 = 0xf6d29a62;
        rb_state7.var_21 = (rb_state7.var_21) ^ (((rb_state7.var_18) == (0x0)) ? (rb_state7.var_20) : (0xb6f0f44d));
        rb_state7.var_22 = (rb_state7.var_22) ^ (rb_state7.var_19);
        rb_state7.var_23 = (rb_state7.var_23) - (((0x6d16ee57) + (0xa65884ef)) ^ (rb_state7.var_21));
        rb_state7.var_24 = (rb_state7.var_24) ^ (rb_state7.var_22);
        rb_state7.var_25 = (rb_state7.var_25) - (((0xf88729be) - (rb_state7.var_23)) ^ (0xfcb22cd5));
        rb_state7.var_26 = (rb_state7.var_26) + (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_24) : (0xc25a3d2e));
        rb_state7.var_27 = (rb_state7.var_27) + (((0x1e81d858) - (rb_state7.var_25)) + (0x3c84da46));
        rb_state7.var_28 = (rb_state7.var_28) ^ (((rb_state7.var_13) == (0x0)) ? (rb_state7.var_26) : (0xdee736f5));
        rb_state7.var_29 = (rb_state7.var_29) - (((rb_state7.var_14) == (0x0)) ? (rb_state7.var_27) : (0x63ce6640));
        rb_state7.var_30 = (rb_state7.var_30) + (((rb_state7.var_15) == (0x0)) ? (rb_state7.var_28) : (0x83ff0037));
        rb_state7.var_31 = (rb_state7.var_31) - (rb_state7.var_29);
        rb_state7.var_32 = (rb_state7.var_32) + (((((0x9e0efe72) + (rb_state7.var_16)) - (rb_state7.var_30)) + (0x689371ba)) + (rb_state7.var_31));
        rb_state7.var_17 = (rb_state7.var_17) - (rb_state7.var_32);
        rb_state7.var_1 = rb_state7.var_17;
        pthread_mutex_unlock(&(rb_state7.lock_33));
    }
    #endif
    DecReference();
}

template <class T, int C> void FlexImage<T, C>::Allocate() {
    DecReference();

    mStore = new FlexImageStore<T, C>(mSize.width, mSize.height);
    mData = mStore->Data();
    mStepBytes = mStore->StepBytes();
    mBpp = sizeof(T) * C;
    mStatus = FlexStsNoError;
    if (mStore->Err()) {
        mStatus = FlexStsAllocationFail;
    }
}

template <class T, int C> FIRect FlexImage<T, C>::ImgRect() const {
    FIRect r;
    r.x = 0;
    r.y = 0;
    r.width = mSize.width;
    r.height = mSize.height;
    return (r);
}

template <class T, int C> bool FlexImage<T, C>::Reallocate(int width, int height) {
    mSize.height = height;
    mSize.width = width;
    Allocate();
    return (Status() == 0);
}

template <class T, int C> bool FlexImage<T, C>::Reallocate(FISize size) {
    mSize = size;
    Allocate();
    return (Status() == 0);
}

template <class T, int C> bool FlexImage<T, C>::ReallocateNE(int width, int height) {
    if (mSize.height != height || mSize.width != width) {
        Reallocate(width, height);
        return (true);
    }
    return (false);
}

template <class T, int C> bool FlexImage<T, C>::ReallocateGE(int width, int height) {
    if (mStore == NULL) {
        Reallocate(width, height);
        return (true);
    }
    FISize s = mStore->Size();
    if (s.height < height || s.width < width) {
        Reallocate(width, height);
        return (true);
    }
    CreateSubImage(*mStore, 0, 0, width, height);
    return (false);
}

template <class T, int C> void FlexImage<T, C>::SetPixel(int x, int y, T *color) {
    int i = 0;
    T *p = &(*this)(x, y);
    while (i < C) {
        *(p++) = color[i++];
    }
}

template <class T, int C> FIStatus FlexImage<T, C>::Set(T value[C + 1]) {
    return (FlexSet(*this, value));
}

template <class T, int C> FIStatus FlexImage<T, C>::CopyFrom(const FlexImage<T, C> &src, bool allocate) {
    return (FlexCopy(src, *this, allocate));
}

template <class T, int C> FIStatus FlexImage<T, C>::CopyTo(FlexImage<T, C> &dst, bool allocate) const {
    return (FlexCopy(*this, dst, allocate));
}

template <class T, int C> bool FlexImage<T, C>::EqualSize(const FlexImage<T, C> &img) {
    return (img.Width() == Width() && img.Height() == Height());
}

template <class T, int C> void FlexImage<T, C>::Print() {
    cout << endl;
    for (int k = 0; k < C; k++) {
        cout << endl << "Plane " << k << ":";
        for (int j = 0; j < mSize.height; j++) {
            cout << endl;
            for (int i = 0; i < mSize.width; i++) {
                cout << setw(4) << setprecision(4) << (float)operator()(i, j, k) << "  ";
            }
        }
    }
}

template class FlexImage<Im8u, 1>;
template class FlexImage<Im8u, 3>;
template class FlexImage<Im8u, 4>;

template class FlexImage<Im8s, 1>;
template class FlexImage<Im8s, 3>;
template class FlexImage<Im8s, 4>;

template class FlexImage<Im16u, 1>;
template class FlexImage<Im16u, 3>;
template class FlexImage<Im16u, 4>;

template class FlexImage<Im16s, 1>;
template class FlexImage<Im16s, 3>;
template class FlexImage<Im16s, 4>;

template class FlexImage<Im32u, 1>;
template class FlexImage<Im32u, 3>;
template class FlexImage<Im32u, 4>;

template class FlexImage<Im32s, 1>;
template class FlexImage<Im32s, 3>;
template class FlexImage<Im32s, 4>;

template class FlexImage<Im32f, 1>;
template class FlexImage<Im32f, 3>;
template class FlexImage<Im32f, 4>;