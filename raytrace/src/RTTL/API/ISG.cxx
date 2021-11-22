

#include "RTTL/API/ISG.hxx"
#include "LRT/include/lrt.h"

namespace ISG {
int current_timestamp = 0;

World *World::m_default;
}; // namespace ISG

void rtShow(node_t node) {
    World *world = World::getDefaultWorld();
    world->rootNode.push_back((RootNode *)node);
};
