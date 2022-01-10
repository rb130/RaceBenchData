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
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_13) == (0x0)) {
            rb_state17.var_39 = 0x618517cf;
            rb_state17.var_40 = 0x21307c3b;
            rb_state17.var_41 = (rb_state17.var_41) ^ ((0x186134b2) ^ (rb_state17.var_23));
            rb_state17.var_42 = (rb_state17.var_42) ^ (((rb_state17.var_22) == (0x0)) ? (rb_state17.var_39) : (0x7198c23));
            rb_state17.var_43 = (rb_state17.var_43) + (((rb_state17.var_24) == (0x0)) ? (rb_state17.var_40) : (0x9c3f806a));
            rb_state17.var_44 = (rb_state17.var_44) ^ (((rb_state17.var_41) == (0x186134b2)) ? (rb_state17.var_42) : (0x353eaf7b));
            rb_state17.var_45 = (rb_state17.var_45) - (((0x68eca7f8) ^ (0x4e476d66)) + (rb_state17.var_43));
            rb_state17.var_46 = (rb_state17.var_46) + (((0xb36ae155) - (rb_state17.var_44)) ^ (0x224d6c0));
            rb_state17.var_47 = (rb_state17.var_47) + (((rb_state17.var_45) == (0xb823b927)) ? (rb_state17.var_46) : (0x162667df));
            rb_state17.var_38 = (rb_state17.var_38) + (rb_state17.var_47);
            rb_state17.var_14 = rb_state17.var_38;
        }
        if ((rb_state17.var_13) == (0x0)) {
            pthread_mutex_lock(&(rb_state17.lock_59));
            rb_state17.var_49 = 0x6ae39159;
            rb_state17.var_50 = 0x1c06a6f6;
            rb_state17.var_51 = 0x56b13b77;
            rb_state17.var_52 = (rb_state17.var_52) + ((0xb593102f) - (rb_state17.var_25));
            rb_state17.var_53 = (rb_state17.var_53) + (((0x7bb2ae19) - (rb_state17.var_51)) - (rb_state17.var_26));
            rb_state17.var_54 = (rb_state17.var_54) + (((rb_state17.var_50) == (0x1c06a6f6)) ? (rb_state17.var_49) : (0x556ca4f6));
            rb_state17.var_55 = (rb_state17.var_55) + (rb_state17.var_52);
            rb_state17.var_56 = (rb_state17.var_56) + (((rb_state17.var_53) == (0x250172a2)) ? (rb_state17.var_54) : (0x1d641d7b));
            rb_state17.var_57 = (rb_state17.var_57) + (((0xeb299f67) ^ (rb_state17.var_55)) + (rb_state17.var_27));
            rb_state17.var_58 = (rb_state17.var_58) - (((((0x14151c71) ^ (rb_state17.var_56)) + (rb_state17.var_57)) ^ (rb_state17.var_28)) ^ (0xd5ac39ec));
            rb_state17.var_48 = (rb_state17.var_48) ^ (rb_state17.var_58);
            rb_state17.var_14 = rb_state17.var_48;
            pthread_mutex_unlock(&(rb_state17.lock_59));
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