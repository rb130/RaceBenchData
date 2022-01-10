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
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_0) == (0x5bce8c0e)) {
        rb_state10.var_20 = 0x155cfae3;
        rb_state10.var_21 = (rb_state10.var_21) + (((0xf394805d) ^ (rb_state10.var_14)) ^ (rb_state10.var_20));
        rb_state10.var_22 = (rb_state10.var_22) + (((((0x7eca86da) ^ (0xeca90f64)) + (rb_state10.var_13)) + (0xed887833)) + (rb_state10.var_12));
        rb_state10.var_23 = (rb_state10.var_23) - (rb_state10.var_21);
        rb_state10.var_24 = (rb_state10.var_24) + (((0x98c0992e) + (rb_state10.var_22)) ^ (0x48d2dd8e));
        rb_state10.var_25 = (rb_state10.var_25) - (rb_state10.var_23);
        rb_state10.var_26 = (rb_state10.var_26) - (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_24) : (0xdadd15c6));
        rb_state10.var_27 = (rb_state10.var_27) - (((rb_state10.var_25) == (0xe6c87abe)) ? (rb_state10.var_26) : (0xd689fb71));
        rb_state10.var_19 = (rb_state10.var_19) - (rb_state10.var_27);
        rb_state10.var_4 = rb_state10.var_19;
    }
    if ((rb_state10.var_0) == (0x5bce8c0e)) {
        pthread_mutex_lock(&(rb_state10.lock_39));
        rb_state10.var_29 = 0x5c2b0be5;
        rb_state10.var_30 = 0x430d8f23;
        rb_state10.var_31 = (rb_state10.var_31) ^ (((((0x62e054ae) ^ (0xce90fdb5)) - (rb_state10.var_29)) + (rb_state10.var_30)) - (0xc6efede0));
        rb_state10.var_32 = (rb_state10.var_32) + (((0xc8a279f1) ^ (rb_state10.var_16)) - (0xf21b61a7));
        rb_state10.var_33 = (rb_state10.var_33) - (((0x1b236328) + (rb_state10.var_31)) ^ (rb_state10.var_17));
        rb_state10.var_34 = (rb_state10.var_34) - (rb_state10.var_32);
        rb_state10.var_35 = (rb_state10.var_35) - (((0xb0271481) + (0xa820eb49)) - (rb_state10.var_33));
        rb_state10.var_36 = (rb_state10.var_36) + (((0x58327caa) ^ (rb_state10.var_34)) + (0x36267b5d));
        rb_state10.var_37 = (rb_state10.var_37) ^ (((0xab9703b3) ^ (rb_state10.var_35)) - (0x773a46bd));
        rb_state10.var_38 = (rb_state10.var_38) - (((rb_state10.var_36) == (0xa7711679)) ? (rb_state10.var_37) : (0xf57336a7));
        rb_state10.var_28 = (rb_state10.var_28) - (rb_state10.var_38);
        rb_state10.var_4 = rb_state10.var_28;
        pthread_mutex_unlock(&(rb_state10.lock_39));
    }
    #endif
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