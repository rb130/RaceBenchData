

#include <cstring>

#include "RTTL/BVH/BVH.hxx"

#include "Sweep.hxx"
#include "BinnedAllDims.hxx"
#include "BinnedAllDimsSaveSpace.hxx"
#include "OnDemandBuilder.hxx"

namespace RTTL {
const char *BVHBuilder::Options::defaultBuilder = "binnedalldimssavespace";

BVHBuilder *BVHBuilder::get(const char *builderType, BVH *bvh) {
    cout << "using BVH builder " << builderType << endl;
    if (builderType == NULL) {
        return get("default", bvh);
    } else if (!strcmp(builderType, "default")) {

        return new BinnedAllDimsSaveSpace(bvh);
    } else if (!strcmp(builderType, "sweep")) {
        return new SweepBVHBuilder(bvh);
    } else if (!strcmp(builderType, "binnedalldims")) {
        return new BinnedAllDims(bvh);
    } else if (!strcmp(builderType, "binnedalldimssavespace")) {
        return new BinnedAllDimsSaveSpace(bvh);
    } else if (!strcmp(builderType, "binned3d")) {
        return new BinnedAllDims(bvh);
    } else if (!strcmp(builderType, "binned")) {

        return new BinnedAllDims(bvh);
    } else if (!strcmp(builderType, "ondemand") || !strcmp(builderType, "lazy")) {
        return new OnDemandBuilder(bvh);
    } else {
        std::string error = std::string("unknown BVH builder type ");
        error += std::string(builderType);

        FATAL(error.c_str());
    }
}

}; // namespace RTTL
