

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
