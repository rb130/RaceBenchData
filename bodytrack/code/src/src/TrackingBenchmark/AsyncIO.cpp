#ifdef __cplusplus
extern "C"
#endif
void racebench_bug4(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug12(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug38(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug39(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug44(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug50(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug51(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug58(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug59(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug62(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug62(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug63(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug63(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug64(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug70(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug74(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug74(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug82(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug82(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug84(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug89(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug96(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug96(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug98(int);

#include "system.h"
#include "FlexIO.h"
#include "AsyncIO.h"
#include <iomanip>
#include <sstream>

template <class T> inline std::string str(T n, int width = 0, char pad = '0') {
    std::stringstream ss; racebench_bug63(1);
    ss << std::setw(width) << std::setfill(pad) << n; racebench_bug4(0);
    return ss.str();
}

void AsyncImageLoader::LoadSet(std::vector<std::string> &FGfiles, BinaryImageSet &FGimages, std::vector<std::string> &ImageFiles, ImageSet &images) {
    FlexImage<Im8u, 1> im;
    for (unsigned int i = 0; i < FGfiles.size(); i++) {
        if (!FlexLoadBMP(FGfiles[i].c_str(), im)) {
            std::cout << "Unable to load image: " << FGfiles[i].c_str() << std::endl;
            mFailed = true;
        }
        FGimages[i].ConvertToBinary(im);
        if (!FlexLoadBMP(ImageFiles[i].c_str(), images[i])) {
            std::cout << "Unable to load image: " << ImageFiles[i].c_str() << std::endl;
            mFailed = true;
        }
    }
}

void AsyncImageLoader::Run() {
    ImageSet images(mNumCameras); racebench_bug12(1); racebench_bug58(1);
    BinaryImageSet FGImages(mNumCameras); racebench_bug59(1); racebench_bug62(0); racebench_bug64(1); racebench_bug96(0);
    std::vector<std::string> FGfiles(mNumCameras), ImageFiles(mNumCameras); racebench_bug44(1); racebench_bug82(1); racebench_bug89(0); racebench_bug98(0);
    while (mCurrentFrame < mNumFrames && !mFailed) {
        for (unsigned int i = 0; i < mNumCameras; i++) {
            FGfiles[i] = mPath + "FG" + str(i + 1) + DIR_SEPARATOR + "image" + str(mCurrentFrame, 4) + ".bmp"; racebench_bug38(0);
            ImageFiles[i] = mPath + "CAM" + str(i + 1) + DIR_SEPARATOR + "image" + str(mCurrentFrame, 4) + ".bmp";
        }

        LoadSet(FGfiles, FGImages, ImageFiles, images); racebench_bug74(0);

        mDataLock.Lock();
        mImageBuffer.push_back(images);
        mFGBuffer.push_back(FGImages);
        mDataLock.Unlock();

        mCondEmpty.NotifyOne();
        if (mImageBuffer.size() >= mBufferSize) {
            mLock1.Lock();
            mCondFull.Wait();
            mLock1.Unlock();
        }
        mCurrentFrame++; racebench_bug39(0);
    }
}

bool AsyncImageLoader::GetNextImageSet(ImageSet &images, BinaryImageSet &FGMaps) {
    if (mFailed) {
        return false;
    }

    while (mImageBuffer.size() == 0) {
        mLock2.Lock(); racebench_bug50(0); racebench_bug51(1); racebench_bug62(1); racebench_bug63(0); racebench_bug70(0); racebench_bug74(1); racebench_bug82(0); racebench_bug84(1); racebench_bug96(1);
        mCondEmpty.Wait();
        mLock2.Unlock();
    }

    mDataLock.Lock();
    images = mImageBuffer[0];
    mImageBuffer.pop_front();
    FGMaps = mFGBuffer[0];
    mFGBuffer.pop_front();
    mDataLock.Unlock();

    mCondFull.NotifyOne();
    return true;
}