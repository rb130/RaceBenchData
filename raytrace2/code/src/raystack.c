#ifdef __cplusplus
extern "C"
#endif
void racebench_bug3(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug6(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug6(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug7(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug7(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug12(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug13(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug22(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug22(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug28(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug28(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug33(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug33(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug40(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug40(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug46(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug46(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug48(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug52(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug71(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug89(int);

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

VOID CopyRayMsg(rdst, rsrc) RAY *rdst;
RAY *rsrc;
{
    rdst->id = rsrc->id;
    rdst->x = rsrc->x;
    rdst->y = rsrc->y; racebench_bug7(1);

    VecCopy(rdst->P, rsrc->P);
    VecCopy(rdst->D, rsrc->D);

    rdst->level = rsrc->level; racebench_bug46(0); racebench_bug46(1);
    rdst->weight = rsrc->weight;
}

VOID InitRayTreeStack(TreeDepth, pid) INT TreeDepth;
INT pid;
{
    unsigned int powint();

    raystruct[pid].StackSize = powint(2, TreeDepth) - 1; racebench_bug40(1);
    raystruct[pid].StackSize += NumSubRays; racebench_bug40(0);
    raystruct[pid].Stack = LocalMalloc(raystruct[pid].StackSize * sizeof(RAY), "raystack.c"); racebench_bug22(0); racebench_bug22(1); racebench_bug33(0); racebench_bug71(0);
    raystruct[pid].StackTop = -1; racebench_bug6(0); racebench_bug6(1); racebench_bug12(1); racebench_bug33(1); racebench_bug52(1); racebench_bug89(0);
}

unsigned int powint(i, j) int i, j;
{
    int k;
    int temp = 1;

    for (k = 0; k < j; k++) {
        temp = temp * i; racebench_bug13(0); racebench_bug28(0); racebench_bug28(1);
    }
    return temp;
}

VOID PushRayTreeStack(rmsg, pid) RAY *rmsg;
INT pid;
{
    raystruct[pid].StackTop++; racebench_bug48(0);

    if (raystruct[pid].StackTop == raystruct[pid].StackSize) {
        fprintf(stderr, "%s: Ray tree stack overflow.\n", ProgName);
        exit(-1);
    }

    CopyRayMsg(&(raystruct[pid].Stack[raystruct[pid].StackTop]), rmsg); racebench_bug3(0);
}

INT PopRayTreeStack(rmsg, pid) RAY *rmsg;
INT pid;
{
    if (raystruct[pid].StackTop < 0) {
        return (RTS_EMPTY);
    }

    CopyRayMsg(rmsg, &(raystruct[pid].Stack[raystruct[pid].StackTop]));

    raystruct[pid].StackTop--; racebench_bug7(0);
    return (RTS_VALID);
}