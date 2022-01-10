#include "racebench_bugs.h"


#include <stdio.h>
#include <math.h>
#include "rt.h"

#define PAGE_SIZE 4096

struct r_struct {
    INT pad1[PAGE_SIZE];

    RAY *Stack;
    INT StackTop;
    INT StackSize;
    INT pad2[PAGE_SIZE];

} raystruct[MAX_PROCS];

VOID CopyRayMsg(rdst, rsrc)
RAY *rdst;
RAY *rsrc;
{
    rdst->id = rsrc->id;
    rdst->x = rsrc->x;
    rdst->y = rsrc->y;

    VecCopy(rdst->P, rsrc->P);
    VecCopy(rdst->D, rsrc->D);

    rdst->level = rsrc->level;
    rdst->weight = rsrc->weight;
}

VOID InitRayTreeStack(TreeDepth, pid)
INT TreeDepth;
INT pid;
{
    unsigned int powint();

    raystruct[pid].StackSize = powint(2, TreeDepth) - 1;
    raystruct[pid].StackSize += NumSubRays;
    raystruct[pid].Stack = LocalMalloc(raystruct[pid].StackSize * sizeof(RAY), "raystack.c");
    raystruct[pid].StackTop = -1;
}

unsigned int powint(i, j)
int i, j;
{
    int k;
    int temp = 1;

    for (k = 0; k < j; k++) {
        temp = temp * i;
    }
    return temp;
}

VOID PushRayTreeStack(rmsg, pid)
RAY *rmsg;
INT pid;
{
    raystruct[pid].StackTop++;

    if (raystruct[pid].StackTop == raystruct[pid].StackSize) {
        fprintf(stderr, "%s: Ray tree stack overflow.\n", ProgName);
        exit(-1);
    }

    CopyRayMsg(&(raystruct[pid].Stack[raystruct[pid].StackTop]), rmsg);
}

INT PopRayTreeStack(rmsg, pid)
RAY *rmsg;
INT pid;
{
    if (raystruct[pid].StackTop < 0) {
        return (RTS_EMPTY);
    }

    CopyRayMsg(rmsg, &(raystruct[pid].Stack[raystruct[pid].StackTop]));

    raystruct[pid].StackTop--;
    return (RTS_VALID);
}