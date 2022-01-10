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
    mSize.width = 0;
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_4) == (0x278bebb4)) {
        rb_state10.var_30 = 0xecbe8fb1;
        rb_state10.var_31 = 0xc170cf0f;
        rb_state10.var_32 = (rb_state10.var_32) ^ (((0xa9a6c23b) - (rb_state10.var_31)) - (rb_state10.var_23));
        rb_state10.var_33 = (rb_state10.var_33) ^ (rb_state10.var_24);
        rb_state10.var_34 = (rb_state10.var_34) + (((rb_state10.var_25) == (0x0)) ? (rb_state10.var_30) : (0xc9d8a2f8));
        rb_state10.var_35 = (rb_state10.var_35) ^ (((0x402b5368) + (rb_state10.var_26)) - (rb_state10.var_22));
        rb_state10.var_36 = (rb_state10.var_36) ^ (rb_state10.var_32);
        rb_state10.var_37 = (rb_state10.var_37) ^ (((rb_state10.var_33) == (0x0)) ? (rb_state10.var_34) : (0x7990072c));
        rb_state10.var_38 = (rb_state10.var_38) ^ (((rb_state10.var_35) == (0x0)) ? (rb_state10.var_36) : (0xcaa68bbd));
        rb_state10.var_39 = (rb_state10.var_39) - (((rb_state10.var_27) == (0x0)) ? (rb_state10.var_37) : (0xe6aad7c7));
        rb_state10.var_40 = (rb_state10.var_40) ^ (((0x9aefe28c) ^ (0x918d8be7)) + (rb_state10.var_38));
        rb_state10.var_41 = (rb_state10.var_41) - (((0x249332b0) + (rb_state10.var_39)) + (0x52cc0a3a));
        rb_state10.var_42 = (rb_state10.var_42) ^ (((((0x372d09d2) + (rb_state10.var_40)) ^ (0xf58b397a)) + (0xe78d37bf)) - (rb_state10.var_41));
        rb_state10.var_29 = (rb_state10.var_29) ^ (rb_state10.var_42);
        rb_state10.var_15 = rb_state10.var_29;
    }
    if ((rb_state10.var_4) == (0x278bebb4)) {
        pthread_mutex_lock(&(rb_state10.lock_57));
        rb_state10.var_44 = 0x1e6ea7a3;
        rb_state10.var_45 = (rb_state10.var_45) + (((rb_state10.var_29) == (0x0)) ? (rb_state10.var_44) : (0x5618a945));
        rb_state10.var_46 = (rb_state10.var_46) ^ (((0x8f6d9a99) + (rb_state10.var_30)) ^ (rb_state10.var_28));
        rb_state10.var_47 = (rb_state10.var_47) - (0x7827b3ea);
        rb_state10.var_48 = (rb_state10.var_48) - (rb_state10.var_45);
        rb_state10.var_49 = (rb_state10.var_49) - (rb_state10.var_46);
        rb_state10.var_50 = (rb_state10.var_50) ^ (rb_state10.var_47);
        rb_state10.var_51 = (rb_state10.var_51) ^ (((rb_state10.var_31) == (0x0)) ? (rb_state10.var_48) : (0xf23fff02));
        rb_state10.var_52 = (rb_state10.var_52) ^ (((rb_state10.var_32) == (0x0)) ? (rb_state10.var_49) : (0xee5bb21e));
        rb_state10.var_53 = (rb_state10.var_53) + (rb_state10.var_50);
        rb_state10.var_54 = (rb_state10.var_54) ^ (rb_state10.var_51);
        rb_state10.var_55 = (rb_state10.var_55) - (((((0xf02e5356) ^ (rb_state10.var_33)) ^ (rb_state10.var_52)) + (rb_state10.var_53)) + (0x4145c11));
        rb_state10.var_56 = (rb_state10.var_56) ^ (((((0x9b067feb) + (rb_state10.var_34)) + (0xe1a64bc)) ^ (rb_state10.var_55)) ^ (rb_state10.var_54));
        rb_state10.var_43 = (rb_state10.var_43) + (rb_state10.var_56);
        rb_state10.var_15 = rb_state10.var_43;
        pthread_mutex_unlock(&(rb_state10.lock_57));
    }
    #endif
    mSize.height = 0;
    mData = NULL;
    mBpp = sizeof(T) * C;
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + (747 < rb_input_size ? (uint32_t)rb_input[747] : 0x330adf04);
    #endif
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
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_7 = (rb_state10.var_7) ^ (0x8fb67bbb);
    #endif
    mStore = NULL;
}

template <class T, int C> FlexImage<T, C>::~FlexImage() {
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_1) == (0x2b6c13c)) {
        rb_state14.var_4 = rb_state14.var_2;
    }
    if ((rb_state14.var_1) == (0x2b6c13c)) {
        if (!((rb_state14.var_2) == (rb_state14.var_4))) {
            racebench_trigger(14);
        }
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