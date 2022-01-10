#include "racebench_bugs.h"


#include "system.h"
#include "FlexIO.h"
#include "AsyncIO.h"
#include <iomanip>
#include <sstream>

template <class T> inline std::string str(T n, int width = 0, char pad = '0') {
    std::stringstream ss;
    ss << std::setw(width) << std::setfill(pad) << n;
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
    ImageSet images(mNumCameras);
    BinaryImageSet FGImages(mNumCameras);
    std::vector<std::string> FGfiles(mNumCameras), ImageFiles(mNumCameras);
    while (mCurrentFrame < mNumFrames && !mFailed) {
        for (unsigned int i = 0; i < mNumCameras; i++) {
            FGfiles[i] = mPath + "FG" + str(i + 1) + DIR_SEPARATOR + "image" + str(mCurrentFrame, 4) + ".bmp";
            ImageFiles[i] = mPath + "CAM" + str(i + 1) + DIR_SEPARATOR + "image" + str(mCurrentFrame, 4) + ".bmp";
        }

        LoadSet(FGfiles, FGImages, ImageFiles, images);

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
        mCurrentFrame++;
    }
}

bool AsyncImageLoader::GetNextImageSet(ImageSet &images, BinaryImageSet &FGMaps) {
    if (mFailed) {
        return false;
    }

    while (mImageBuffer.size() == 0) {
        mLock2.Lock();
        #ifdef RACEBENCH_BUG_14
        if ((rb_state14.var_1) == (0x2b6c13c)) {
            rb_state14.var_5 = (rb_state14.var_5) ^ (0x24c0a4ac);
            rb_state14.var_6 = (rb_state14.var_6) - (rb_state14.var_5);
            rb_state14.var_7 = (rb_state14.var_7) + (rb_state14.var_6);
            rb_state14.var_8 = (rb_state14.var_8) + (rb_state14.var_7);
            rb_state14.var_9 = (rb_state14.var_9) + (((0xc96227e2) - (rb_state14.var_8)) ^ (rb_state14.var_3));
            rb_state14.var_10 = (rb_state14.var_10) + (((rb_state14.var_4) == (0xa6572dc)) ? (rb_state14.var_5) : (0x12b93377));
            rb_state14.var_11 = (rb_state14.var_11) ^ (((rb_state14.var_6) == (0xdb3f5b54)) ? (rb_state14.var_7) : (0x7230d1b9));
            rb_state14.var_12 = (rb_state14.var_12) + (((rb_state14.var_9) == (0x622ff2d2)) ? (rb_state14.var_8) : (0xcf2b5fd0));
            rb_state14.var_13 = (rb_state14.var_13) - (((((0x9f6f7389) + (rb_state14.var_9)) - (rb_state14.var_10)) + (0xd4e0839b)) ^ (rb_state14.var_10));
            rb_state14.var_14 = (rb_state14.var_14) ^ (((((0x3706c1f8) ^ (rb_state14.var_12)) - (0xa6add9c8)) - (rb_state14.var_11)) + (rb_state14.var_11));
            rb_state14.var_15 = (rb_state14.var_15) + (((0x88ebc78f) - (0x3c6aebb)) + (rb_state14.var_13));
            rb_state14.var_16 = (rb_state14.var_16) + (((rb_state14.var_12) == (0xdb3f5b54)) ? (rb_state14.var_14) : (0xc2491e0e));
            rb_state14.var_17 = (rb_state14.var_17) ^ (rb_state14.var_15);
            rb_state14.var_18 = (rb_state14.var_18) + (((rb_state14.var_16) == (0x458bc0e4)) ? (rb_state14.var_17) : (0x938ef16e));
            rb_state14.var_2 = (rb_state14.var_2) - (rb_state14.var_18);
        }
        #endif
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