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
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) - (0xa4e06cbe);
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
        #ifdef RACEBENCH_BUG_3
        if ((rb_state3.var_13) == (0xdb240083)) {
            pthread_mutex_lock(&(rb_state3.lock_44));
            rb_state3.var_29 = 0xc09355ca;
            rb_state3.var_30 = 0xefbf9fd9;
            rb_state3.var_31 = 0x41e7eab7;
            rb_state3.var_32 = (rb_state3.var_32) - (((0xe5f49d95) - (rb_state3.var_30)) + (rb_state3.var_23));
            rb_state3.var_33 = (rb_state3.var_33) ^ (((rb_state3.var_24) == (0x0)) ? (rb_state3.var_31) : (0xf73eb3ce));
            rb_state3.var_34 = (rb_state3.var_34) ^ (((rb_state3.var_29) == (0xc09355ca)) ? (rb_state3.var_32) : (0x1bc0e685));
            rb_state3.var_35 = (rb_state3.var_35) + (((0xe4b73a49) - (rb_state3.var_25)) + (rb_state3.var_33));
            rb_state3.var_36 = (rb_state3.var_36) - (((rb_state3.var_34) == (0x9cb0244)) ? (rb_state3.var_35) : (0x9992287d));
            rb_state3.var_28 = (rb_state3.var_28) ^ (rb_state3.var_36);
            rb_state3.var_17 = rb_state3.var_28;
            pthread_mutex_unlock(&(rb_state3.lock_44));
        }
        if ((rb_state3.var_13) == (0xdb240083)) {
            pthread_mutex_lock(&(rb_state3.lock_44));
            rb_state3.var_38 = 0x22bd6d1;
            rb_state3.var_39 = (rb_state3.var_39) - (((rb_state3.var_28) == (0xd960db00)) ? (rb_state3.var_26) : (0x46dc80db));
            rb_state3.var_40 = (rb_state3.var_40) + (141 < rb_input_size ? (uint32_t)rb_input[141] : 0xb5484282);
            rb_state3.var_41 = (rb_state3.var_41) - (((((0xb18c95f7) ^ (rb_state3.var_38)) ^ (0xfdedacef)) + (rb_state3.var_27)) ^ (0xfa6f7bb5));
            rb_state3.var_42 = (rb_state3.var_42) + (((rb_state3.var_39) == (0x0)) ? (rb_state3.var_40) : (0x16c05ebc));
            rb_state3.var_43 = (rb_state3.var_43) ^ (((((0xe6f904e) - (rb_state3.var_41)) - (0x3c88f2c3)) + (rb_state3.var_42)) ^ (rb_state3.var_29));
            rb_state3.var_37 = (rb_state3.var_37) + (rb_state3.var_43);
            rb_state3.var_17 = rb_state3.var_37;
            pthread_mutex_unlock(&(rb_state3.lock_44));
        }
        #endif
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