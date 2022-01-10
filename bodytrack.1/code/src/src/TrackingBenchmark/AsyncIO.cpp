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
    #ifdef RACEBENCH_BUG_5
    if ((rb_state5.var_0) == (0xe1640001)) {
        if ((rb_state5.var_1) != (0x0)) {
            if (!((rb_state5.var_1) == (rb_state5.var_29))) {
                racebench_trigger(5);
            }
        }
    }
    #endif
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
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_0) == (0xe1640001)) {
            pthread_mutex_lock(&(rb_state5.lock_37));
            rb_state5.var_9 = 0xe9241060;
            rb_state5.var_10 = 0xf146742b;
            rb_state5.var_11 = (rb_state5.var_11) + ((0xb901be36) + (rb_state5.var_6));
            rb_state5.var_12 = (rb_state5.var_12) - (0xcaf72afc);
            rb_state5.var_13 = (rb_state5.var_13) + (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_5) : (0xb6851345));
            rb_state5.var_14 = (rb_state5.var_14) - (((0x2bdd97d8) ^ (rb_state5.var_10)) + (0x1876434b));
            rb_state5.var_15 = (rb_state5.var_15) + (((0x89ede21f) + (0xcd10f993)) - (rb_state5.var_9));
            rb_state5.var_16 = (rb_state5.var_16) + (rb_state5.var_11);
            rb_state5.var_17 = (rb_state5.var_17) - (((rb_state5.var_12) == (0x3508d504)) ? (rb_state5.var_13) : (0xd2133ed0));
            rb_state5.var_18 = (rb_state5.var_18) + (((0xa73a1869) - (rb_state5.var_14)) + (0xaf4a9a3e));
            rb_state5.var_19 = (rb_state5.var_19) ^ (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_15) : (0x60426b));
            rb_state5.var_20 = (rb_state5.var_20) - (rb_state5.var_16);
            rb_state5.var_21 = (rb_state5.var_21) - (((0x2723a461) - (rb_state5.var_9)) + (rb_state5.var_17));
            rb_state5.var_22 = (rb_state5.var_22) - (((rb_state5.var_18) == (0x4996d9e5)) ? (rb_state5.var_19) : (0x27fe652c));
            rb_state5.var_23 = (rb_state5.var_23) + (((rb_state5.var_10) == (0xf146742b)) ? (rb_state5.var_20) : (0xebdc093d));
            rb_state5.var_24 = (rb_state5.var_24) ^ (rb_state5.var_21);
            rb_state5.var_25 = (rb_state5.var_25) + (((rb_state5.var_22) == (0x922534ae)) ? (rb_state5.var_23) : (0xcfcabb5b));
            rb_state5.var_26 = (rb_state5.var_26) ^ (((rb_state5.var_11) == (0xb901be36)) ? (rb_state5.var_24) : (0x5e2e1aaa));
            rb_state5.var_27 = (rb_state5.var_27) - (((0x4fae5cb6) ^ (0xcd27fe22)) ^ (rb_state5.var_25));
            rb_state5.var_28 = (rb_state5.var_28) - (((((0xeac763b0) - (rb_state5.var_12)) + (0x9a72f933)) - (rb_state5.var_26)) ^ (rb_state5.var_27));
            rb_state5.var_8 = (rb_state5.var_8) - (rb_state5.var_28);
            rb_state5.var_1 = rb_state5.var_8;
            pthread_mutex_unlock(&(rb_state5.lock_37));
        }
        if ((rb_state5.var_0) == (0xe1640001)) {
            pthread_mutex_lock(&(rb_state5.lock_37));
            rb_state5.var_30 = 0xc99d1f8f;
            rb_state5.var_31 = (rb_state5.var_31) + (((((0x8ba9de25) ^ (rb_state5.var_13)) - (0x97dfbecf)) - (rb_state5.var_14)) + (0xaebe785));
            rb_state5.var_32 = (rb_state5.var_32) ^ (((rb_state5.var_15) == (0x6ddacb52)) ? (rb_state5.var_30) : (0xafdef4d0));
            rb_state5.var_33 = (rb_state5.var_33) + (rb_state5.var_31);
            rb_state5.var_34 = (rb_state5.var_34) ^ (((rb_state5.var_16) == (0xb901be36)) ? (rb_state5.var_32) : (0xe773b050));
            rb_state5.var_35 = (rb_state5.var_35) + (rb_state5.var_33);
            rb_state5.var_36 = (rb_state5.var_36) + (((((0x8c97d21f) + (0x2cf02ae3)) ^ (rb_state5.var_34)) ^ (rb_state5.var_17)) - (rb_state5.var_35));
            rb_state5.var_29 = (rb_state5.var_29) + (rb_state5.var_36);
            rb_state5.var_1 = rb_state5.var_29;
            pthread_mutex_unlock(&(rb_state5.lock_37));
        }
        #endif
        mLock2.Lock();
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state7.lock_33));
            if ((rb_state7.var_1) != (0x0)) {
                if (!((rb_state7.var_1) == (rb_state7.var_17))) {
                    racebench_trigger(7);
                }
            }
            pthread_mutex_unlock(&(rb_state7.lock_33));
        }
        #endif
        #ifdef RACEBENCH_BUG_17
        if ((rb_state17.var_0) == (0x0)) {
            rb_state17.var_22 = (rb_state17.var_22) + (((((0x3ab5d2f8) - (rb_state17.var_13)) + (rb_state17.var_14)) + (rb_state17.var_15)) + (0x121fe8cc));
            rb_state17.var_23 = (rb_state17.var_23) - (rb_state17.var_12);
            rb_state17.var_24 = (rb_state17.var_24) ^ (((rb_state17.var_16) == (0xe9f24e2)) ? (rb_state17.var_22) : (0x5ee68746));
            rb_state17.var_25 = (rb_state17.var_25) ^ (((rb_state17.var_23) == (0xee607bcc)) ? (rb_state17.var_24) : (0xbfa7eabd));
            rb_state17.var_1 = (rb_state17.var_1) ^ (rb_state17.var_25);
        }
        if (!((rb_state17.var_1) == (rb_state17.var_2))) {
            pthread_mutex_lock(&(rb_state17.lock_71));
            rb_state17.var_50 = 0x5ac767ac;
            rb_state17.var_51 = (rb_state17.var_51) - (((rb_state17.var_29) == (0x0)) ? (rb_state17.var_27) : (0xc9932e3d));
            rb_state17.var_52 = (rb_state17.var_52) + (rb_state17.var_28);
            rb_state17.var_53 = (rb_state17.var_53) ^ (0x1922c922);
            rb_state17.var_54 = (rb_state17.var_54) ^ (((rb_state17.var_50) == (0x5ac767ac)) ? (rb_state17.var_51) : (0xf3094c0));
            rb_state17.var_55 = (rb_state17.var_55) ^ (((0xaa5584ad) ^ (rb_state17.var_52)) - (rb_state17.var_30));
            rb_state17.var_56 = (rb_state17.var_56) ^ (((((0x32c538b3) - (rb_state17.var_54)) ^ (rb_state17.var_32)) ^ (rb_state17.var_31)) ^ (rb_state17.var_53));
            rb_state17.var_57 = (rb_state17.var_57) + (((0xf4477de0) + (rb_state17.var_55)) - (0x5a51032d));
            rb_state17.var_58 = (rb_state17.var_58) ^ (((rb_state17.var_33) == (0x0)) ? (rb_state17.var_56) : (0x1a16c55d));
            rb_state17.var_59 = (rb_state17.var_59) - (rb_state17.var_57);
            rb_state17.var_60 = (rb_state17.var_60) - (rb_state17.var_58);
            rb_state17.var_61 = (rb_state17.var_61) ^ (rb_state17.var_59);
            rb_state17.var_62 = (rb_state17.var_62) - (((((0x12f71108) - (rb_state17.var_61)) - (rb_state17.var_35)) ^ (rb_state17.var_60)) ^ (rb_state17.var_34));
            rb_state17.var_49 = (rb_state17.var_49) ^ (rb_state17.var_62);
            rb_state17.var_27 = rb_state17.var_49;
            pthread_mutex_unlock(&(rb_state17.lock_71));
        }
        if (!((rb_state17.var_1) == (rb_state17.var_2))) {
            pthread_mutex_lock(&(rb_state17.lock_71));
            rb_state17.var_64 = 0x3282f957;
            rb_state17.var_65 = 0xbcab0ece;
            rb_state17.var_66 = (rb_state17.var_66) + (((0x2e180f76) - (rb_state17.var_36)) - (rb_state17.var_37));
            rb_state17.var_67 = (rb_state17.var_67) ^ (((0x34f15888) - (0x148b826)) - (rb_state17.var_65));
            rb_state17.var_68 = (rb_state17.var_68) ^ (((((0xb645d8ed) ^ (rb_state17.var_39)) ^ (rb_state17.var_64)) ^ (rb_state17.var_38)) - (rb_state17.var_66));
            rb_state17.var_69 = (rb_state17.var_69) + (((rb_state17.var_40) == (0x0)) ? (rb_state17.var_67) : (0x6e73fc76));
            rb_state17.var_70 = (rb_state17.var_70) ^ (((rb_state17.var_68) == (0x56af1244)) ? (rb_state17.var_69) : (0xa8f86fde));
            rb_state17.var_63 = (rb_state17.var_63) - (rb_state17.var_70);
            rb_state17.var_27 = rb_state17.var_63;
            pthread_mutex_unlock(&(rb_state17.lock_71));
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