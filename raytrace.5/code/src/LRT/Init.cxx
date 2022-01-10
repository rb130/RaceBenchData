

#ifdef _MSC_VER

#pragma warning(disable : 4530)
#pragma warning(disable : 4996)
#endif

#include "include/lrt.h"
#include <iostream>
#include <vector>
#include "FrameBuffer.hxx"
#include "RTTL/BVH/Builder/Builder.hxx"
#include "RTTL/common/MapOptions.hxx"

using std::copy;
using std::vector;

using namespace std;

namespace LRT {

void ParseCmdLine(int *argc, char **argv) {

    MapOptions myOptions;
    myOptions.parse(*argc, argv);

    FrameBuffer::Options::usePBOs = !myOptions.defined("no-pbo, no-pbos");
    if (myOptions.get("pbo", 0) || myOptions.get("pbo") == string("on")) {
        FrameBuffer::Options::usePBOs = true;
    }

    FrameBuffer::Options::useMemoryFB = myOptions.defined("mem-fb, memory-fb");

    BVHBuilder::Options::defaultBuilder = myOptions.get("bvh-build-method", BVHBuilder::Options::defaultBuilder);
    BVHBuilder::Options::defaultBuilder = myOptions.get("bvh-builder", BVHBuilder::Options::defaultBuilder);

    cout << "default BVH builder : " << BVHBuilder::Options::defaultBuilder << endl;
}
}; // namespace LRT

void lrtInit(int *argc, char **argv) {

    int oldMXCSR = _mm_getcsr();
    int newMXCSR = oldMXCSR | 0x80A0;
    _mm_setcsr(newMXCSR);

    LRT::ParseCmdLine(argc, argv);
}
