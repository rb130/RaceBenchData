#ifdef __cplusplus
extern "C"
#endif
void racebench_bug25(int);

#include <math.h>
#include "emd.h"

#define DEBUG_LEVEL 0

static float emdinit(emd_state_t *, signature_t *Signature1, signature_t *Signature2, float (*Dist)(cass_size_t, feature_t, feature_t, void *), cass_size_t, void *);
static void findBasicVariables(emd_state_t *, emd_node1_t *U, emd_node1_t *V);
static int isOptimal(emd_state_t *, emd_node1_t *U, emd_node1_t *V);
static int findLoop(emd_state_t *, emd_node2_t **Loop);
static void newSol(emd_state_t *);
static void russel(emd_state_t *, double *S, double *D);
static void addBasicVariable(emd_state_t *, int minI, int minJ, double *S, double *D, emd_node1_t *PrevUMinI, emd_node1_t *PrevVMinJ, emd_node1_t *UHead);
#if DEBUG_LEVEL > 0
static void printSolution(emd_state_t *);
#endif

emd_state_t *mkemdstate(void) {
    emd_state_t *state;

    state = malloc(sizeof *state);
    bzero(state, sizeof *state);
    return state;
}

void freeemdstate(emd_state_t *state) {
    free(state);
    return;
}

static __thread double **C = NULL;

float emd(signature_t *Signature1, signature_t *Signature2, float (*Dist)(cass_size_t, feature_t, feature_t, void *), cass_size_t dim, void *param, flow_t *Flow, int *FlowSize) {
    struct emd_state_t emd_state, *state = &emd_state;
    int itr;
    double totalCost;
    float w;
    emd_node2_t *XP;
    flow_t *FlowP;
    emd_node1_t U[MAX_SIG_SIZE1], V[MAX_SIG_SIZE1];

    if (C == NULL) {
        C = type_matrix_alloc(double, MAX_SIG_SIZE1, MAX_SIG_SIZE1);
    }

    bzero(state, sizeof *state);

    state->C = C;

    w = emdinit(state, Signature1, Signature2, Dist, dim, param);

    if (w == EMD_INFINITY) {

        return EMD_INFINITY;
    }

#if DEBUG_LEVEL > 1
    print("\nINITIAL SOLUTION:\n");
    printSolution(state);
#endif

    if (state->n1 > 1 && state->n2 > 1) {
        for (itr = 1; itr < MAX_ITERATIONS; itr++) {

            findBasicVariables(state, U, V);

            if (isOptimal(state, U, V)) {
                break;
            }

            newSol(state);

#if DEBUG_LEVEL > 1
            print("\nITERATION # %d \n", itr);
            printSolution(state);
#endif
        }

        if (itr == MAX_ITERATIONS) {
            warn("emd: Maximum number of iterations has been reached (%d)\n", MAX_ITERATIONS);
        }
    }

    totalCost = 0;
    FlowP = Flow;
    for (XP = state->X; XP < state->EndX; XP++) {
        if (XP == state->EnterX) {
            continue;
        }
        if (XP->i == Signature1->n || XP->j == Signature2->n) {
            continue;
        }

        if (XP->val == 0) {
            continue;
        }

        totalCost += (double)XP->val * state->C[XP->i][XP->j];
        if (Flow != NULL) {
            FlowP->from = XP->i;
            FlowP->to = XP->j;
            FlowP->amount = XP->val;
            FlowP->cost = state->C[XP->i][XP->j];
            state->tot_flow_costs += FlowP->cost;
            state->tot_flow++;
            FlowP++;
        }
    }
    if (Flow != NULL) {
        *FlowSize = FlowP - Flow;
    }

#if DEBUG_LEVEL > 0
    print("\n*** OPTIMAL SOLUTION (%d ITERATIONS): %f ***\n", itr, totalCost);
#endif

    return (float)(totalCost / w);
}

static float emdinit(emd_state_t *state, signature_t *Signature1, signature_t *Signature2, float (*Dist)(cass_size_t, feature_t, feature_t, void *), cass_size_t dim, void *param) {
    int i, j;
    double sSum, dSum, diff;
    double S[MAX_SIG_SIZE1], D[MAX_SIG_SIZE1];

    state->n1 = Signature1->n;
    state->n2 = Signature2->n;

    if (state->n1 > MAX_SIG_SIZE || state->n2 > MAX_SIG_SIZE) {
        warn("emd: Signature size is limited to %d, n1: %d, n2: %d\n", MAX_SIG_SIZE, state->n1, state->n2);
        return EMD_INFINITY;
    }

    state->maxC = 0;
    for (i = 0; i < state->n1; i++) {
        for (j = 0; j < state->n2; j++) {
            state->C[i][j] = Dist(dim, Signature1->Features[i], Signature2->Features[j], param);
            if (state->C[i][j] > state->maxC) {
                state->maxC = state->C[i][j];
            }
        }
    }

    sSum = 0.0;
    for (i = 0; i < state->n1; i++) {
        S[i] = Signature1->Weights[i];
        sSum += Signature1->Weights[i];
        state->RowsX[i] = NULL;
    }
    dSum = 0.0;
    for (j = 0; j < state->n2; j++) {
        D[j] = Signature2->Weights[j];
        dSum += Signature2->Weights[j];
        state->ColsX[j] = NULL;
    }

    diff = sSum - dSum;
    if (fabs(diff) >= EPSILON * sSum) {
        if (diff < 0.0) {
            for (j = 0; j < state->n2; j++) {
                state->C[state->n1][j] = 0;
            }
            S[state->n1] = -diff;
            state->RowsX[state->n1] = NULL;
            state->n1++;
        } else {
            for (i = 0; i < state->n1; i++) {
                state->C[i][state->n2] = 0;
            }
            D[state->n2] = diff;
            state->ColsX[state->n2] = NULL;
            state->n2++;
        }
    }

    for (i = 0; i < state->n1; i++) {
        for (j = 0; j < state->n2; j++) {
            state->IsX[i][j] = 0;
        }
    }
    state->EndX = state->X;

    state->maxW = sSum > dSum ? sSum : dSum;

    russel(state, S, D);

    state->EnterX = state->EndX++;

    return sSum > dSum ? dSum : sSum;
}

static void findBasicVariables(emd_state_t *state, emd_node1_t *U, emd_node1_t *V) {
    int i, j, found;
    int UfoundNum, VfoundNum;
    emd_node1_t u0Head, u1Head, *CurU, *PrevU;
    emd_node1_t v0Head, v1Head, *CurV, *PrevV;

    u0Head.Next = CurU = U;
    for (i = 0; i < state->n1; i++) {
        CurU->i = i;
        CurU->Next = CurU + 1;
        CurU++;
    }
    (--CurU)->Next = NULL;
    u1Head.Next = NULL;

    CurV = V + 1;
    v0Head.Next = state->n2 > 1 ? V + 1 : NULL;
    for (j = 1; j < state->n2; j++) {
        CurV->i = j;
        CurV->Next = CurV + 1;
        CurV++;
    }
    (--CurV)->Next = NULL;
    v1Head.Next = NULL; racebench_bug25(1);

    V[0].i = 0;
    V[0].val = 0;
    v1Head.Next = V;
    v1Head.Next->Next = NULL;

    UfoundNum = VfoundNum = 0;
    while (UfoundNum < state->n1 || VfoundNum < state->n2) {

#if DEBUG_LEVEL > 3
        print("UfoundNum=%d/%d,VfoundNum=%d/%d\n", UfoundNum, state->n1, VfoundNum, state->n2);
        print("U0=");
        for (CurU = u0Head.Next; CurU != NULL; CurU = CurU->Next)
            print("[%d]", CurU - U);
        print("\n");
        print("U1=");
        for (CurU = u1Head.Next; CurU != NULL; CurU = CurU->Next)
            print("[%d]", CurU - U);
        print("\n");
        print("V0=");
        for (CurV = v0Head.Next; CurV != NULL; CurV = CurV->Next)
            print("[%d]", CurV - V);
        print("\n");
        print("V1=");
        for (CurV = v1Head.Next; CurV != NULL; CurV = CurV->Next)
            print("[%d]", CurV - V);
        print("\n\n");
#endif

        found = 0;
        if (VfoundNum < state->n2) {

            PrevV = &v1Head;
            for (CurV = v1Head.Next; CurV != NULL; CurV = CurV->Next) {
                j = CurV->i;

                PrevU = &u0Head;
                for (CurU = u0Head.Next; CurU != NULL; CurU = CurU->Next) {
                    i = CurU->i;
                    if (state->IsX[i][j]) {

                        CurU->val = state->C[i][j] - CurV->val;

                        PrevU->Next = CurU->Next;
                        CurU->Next = u1Head.Next != NULL ? u1Head.Next : NULL;
                        u1Head.Next = CurU;
                        CurU = PrevU;
                    } else {
                        PrevU = CurU;
                    }
                }
                PrevV->Next = CurV->Next;
                VfoundNum++;
                found = 1;
            }
        }
        if (UfoundNum < state->n1) {

            PrevU = &u1Head;
            for (CurU = u1Head.Next; CurU != NULL; CurU = CurU->Next) {
                i = CurU->i;

                PrevV = &v0Head;
                for (CurV = v0Head.Next; CurV != NULL; CurV = CurV->Next) {
                    j = CurV->i;
                    if (state->IsX[i][j]) {

                        CurV->val = state->C[i][j] - CurU->val;

                        PrevV->Next = CurV->Next;
                        CurV->Next = v1Head.Next != NULL ? v1Head.Next : NULL;
                        v1Head.Next = CurV;
                        CurV = PrevV;
                    } else {
                        PrevV = CurV;
                    }
                }
                PrevU->Next = CurU->Next;
                UfoundNum++;
                found = 1;
            }
        }
        if (!found) {
            warn("emd: Unexpected error in findBasicVariables!\n");
            warn("This typically happens when the EPSILON defined in\n");
            warn("emd.h is not right for the scale of the problem.\n");
            fatal("emd: errr in findBasicVariables");
        }
    }
}

static int isOptimal(emd_state_t *state, emd_node1_t *U, emd_node1_t *V) {
    double delta, deltaMin;
    int i, j, minI, minJ;

    minI = minJ = 0;

    deltaMin = EMD_INFINITY;
    for (i = 0; i < state->n1; i++) {
        for (j = 0; j < state->n2; j++) {
            if (!state->IsX[i][j]) {
                delta = state->C[i][j] - U[i].val - V[j].val;
                if (deltaMin > delta) {
                    deltaMin = delta;
                    minI = i;
                    minJ = j;
                }
            }
        }
    }

#if DEBUG_LEVEL > 3
    print("deltaMin=%f\n", deltaMin);
#endif

    if (deltaMin == EMD_INFINITY) {
        warn("emd: Unexpected error in isOptimal.\n");
        fatal("emd: error in isOptimal");
    }

    state->EnterX->i = minI;
    state->EnterX->j = minJ;

    return deltaMin >= -EPSILON * state->maxC;
}

static void newSol(emd_state_t *state) {
    int i, j, k;
    double xMin;
    int steps;
    emd_node2_t *Loop[2 * MAX_SIG_SIZE1], *CurX, *LeaveX;

    LeaveX = NULL;

#if DEBUG_LEVEL > 3
    print("EnterX = (%d,%d)\n", state->EnterX->i, state->EnterX->j);
#endif

    i = state->EnterX->i;
    j = state->EnterX->j;
    state->IsX[i][j] = 1;
    state->EnterX->NextC = state->RowsX[i];
    state->EnterX->NextR = state->ColsX[j];
    state->EnterX->val = 0;
    state->RowsX[i] = state->EnterX;
    state->ColsX[j] = state->EnterX;

    steps = findLoop(state, Loop);

    xMin = EMD_INFINITY;
    for (k = 1; k < steps; k += 2) {
        if (Loop[k]->val < xMin) {
            LeaveX = Loop[k];
            xMin = Loop[k]->val;
        }
    }

    for (k = 0; k < steps; k += 2) {
        Loop[k]->val += xMin;
        Loop[k + 1]->val -= xMin;
    }

#if DEBUG_LEVEL > 3
    print("LeaveX = (%d,%d)\n", LeaveX->i, LeaveX->j);
#endif

    i = LeaveX->i;
    j = LeaveX->j;
    state->IsX[i][j] = 0;
    if (state->RowsX[i] == LeaveX) {
        state->RowsX[i] = LeaveX->NextC;
    } else {
        for (CurX = state->RowsX[i]; CurX != NULL; CurX = CurX->NextC) {
            if (CurX->NextC == LeaveX) {
                CurX->NextC = CurX->NextC->NextC;
                break;
            }
        }
    }
    if (state->ColsX[j] == LeaveX) {
        state->ColsX[j] = LeaveX->NextR;
    } else {
        for (CurX = state->ColsX[j]; CurX != NULL; CurX = CurX->NextR) {
            if (CurX->NextR == LeaveX) {
                CurX->NextR = CurX->NextR->NextR;
                break;
            }
        }
    }

    state->EnterX = LeaveX;
}

static int findLoop(emd_state_t *state, emd_node2_t **Loop) {
    int i, steps;
    emd_node2_t **CurX, *NewX;
    char IsUsed[2 * MAX_SIG_SIZE1];

    for (i = 0; i < state->n1 + state->n2; i++) {
        IsUsed[i] = 0;
    }

    CurX = Loop;
    NewX = *CurX = state->EnterX;
    IsUsed[state->EnterX - state->X] = 1;
    steps = 1;

    do {
        if (steps % 2 == 1) {

            NewX = state->RowsX[NewX->i];
            while (NewX != NULL && IsUsed[NewX - state->X]) {
                NewX = NewX->NextC;
            }
        } else {

            NewX = state->ColsX[NewX->j];
            while (NewX != NULL && IsUsed[NewX - state->X] && NewX != state->EnterX) {
                NewX = NewX->NextR;
            }
            if (NewX == state->EnterX) {
                break;
            }
        }

        if (NewX != NULL) {

            *++CurX = NewX;
            IsUsed[NewX - state->X] = 1;
            steps++;
#if DEBUG_LEVEL > 3
            print("steps=%d, NewX=(%d,%d)\n", steps, NewX->i, NewX->j);
#endif
        } else {

            do {
                NewX = *CurX;
                do {
                    if (steps % 2 == 1) {
                        NewX = NewX->NextR;
                    } else {
                        NewX = NewX->NextC;
                    }
                } while (NewX != NULL && IsUsed[NewX - state->X]);

                if (NewX == NULL) {
                    IsUsed[*CurX - state->X] = 0;
                    CurX--;
                    steps--;
                }
            } while (NewX == NULL && CurX >= Loop);

#if DEBUG_LEVEL > 3
            print("BACKTRACKING TO: steps=%d, NewX=(%d,%d)\n", steps, NewX->i, NewX->j);
#endif
            IsUsed[*CurX - state->X] = 0;
            *CurX = NewX;
            IsUsed[NewX - state->X] = 1;
        }
    } while (CurX >= Loop);

    if (CurX == Loop) {
        warn("emd: Unexpected error in findLoop!\n");
        fatal("emd: findLoop");
    }
#if DEBUG_LEVEL > 3
    print("FOUND LOOP:\n");
    for (i = 0; i < steps; i++)
        print("%d: (%d,%d)\n", i, Loop[i]->i, Loop[i]->j);
#endif

    return steps;
}

static void russel(emd_state_t *state, double *S, double *D) {
    int i, j, found, minI, minJ;
    double deltaMin, oldVal, diff;
    double Delta[MAX_SIG_SIZE1][MAX_SIG_SIZE1];
    emd_node1_t Ur[MAX_SIG_SIZE1], Vr[MAX_SIG_SIZE1];
    emd_node1_t uHead, *CurU, *PrevU;
    emd_node1_t vHead, *CurV, *PrevV;
    emd_node1_t *PrevUMinI, *PrevVMinJ, *Remember;

    PrevUMinI = PrevVMinJ = NULL;

    uHead.Next = CurU = Ur;
    for (i = 0; i < state->n1; i++) {
        CurU->i = i;
        CurU->val = -EMD_INFINITY;
        CurU->Next = CurU + 1;
        CurU++;
    }
    (--CurU)->Next = NULL;

    vHead.Next = CurV = Vr;
    for (j = 0; j < state->n2; j++) {
        CurV->i = j;
        CurV->val = -EMD_INFINITY;
        CurV->Next = CurV + 1;
        CurV++;
    }
    (--CurV)->Next = NULL;

    for (i = 0; i < state->n1; i++) {
        for (j = 0; j < state->n2; j++) {
            float v;
            v = state->C[i][j];
            if (Ur[i].val <= v) {
                Ur[i].val = v;
            }
            if (Vr[j].val <= v) {
                Vr[j].val = v;
            }
        }
    }

    for (i = 0; i < state->n1; i++) {
        for (j = 0; j < state->n2; j++) {
            Delta[i][j] = state->C[i][j] - Ur[i].val - Vr[j].val;
        }
    }

    do {
#if DEBUG_LEVEL > 3
        print("Ur=");
        for (CurU = uHead.Next; CurU != NULL; CurU = CurU->Next)
            print("[%d]", CurU - Ur);
        print("\n");
        print("Vr=");
        for (CurV = vHead.Next; CurV != NULL; CurV = CurV->Next)
            print("[%d]", CurV - Vr);
        print("\n");
        print("\n\n");
#endif

        found = 0;
        minI = uHead.Next ? uHead.Next->i : 0;
        minJ = vHead.Next ? vHead.Next->i : 0;
        deltaMin = EMD_INFINITY;
        PrevU = &uHead;
        for (CurU = uHead.Next; CurU != NULL; CurU = CurU->Next) {
            int i;
            i = CurU->i;
            PrevV = &vHead;
            for (CurV = vHead.Next; CurV != NULL; CurV = CurV->Next) {
                int j;
                j = CurV->i;
                if (deltaMin > Delta[i][j]) {
                    deltaMin = Delta[i][j];
                    minI = i;
                    minJ = j;
                    PrevUMinI = PrevU;
                    PrevVMinJ = PrevV;
                    found = 1;
                }
                PrevV = CurV;
            }
            PrevU = CurU;
        }

        if (!found) {
            break;
        }

        Remember = PrevUMinI->Next;
        addBasicVariable(state, minI, minJ, S, D, PrevUMinI, PrevVMinJ, &uHead);

        if (Remember == PrevUMinI->Next) {
            for (CurV = vHead.Next; CurV != NULL; CurV = CurV->Next) {
                int j;
                j = CurV->i;
                if (CurV->val == state->C[minI][j]) {

                    oldVal = CurV->val;
                    CurV->val = -EMD_INFINITY;
                    for (CurU = uHead.Next; CurU != NULL; CurU = CurU->Next) {
                        int i;
                        i = CurU->i;
                        if (CurV->val <= state->C[i][j]) {
                            CurV->val = state->C[i][j];
                        }
                    }

                    diff = oldVal - CurV->val;
                    if (fabs(diff) < EPSILON * state->maxC) {
                        for (CurU = uHead.Next; CurU != NULL; CurU = CurU->Next) {
                            Delta[CurU->i][j] += diff;
                        }
                    }
                }
            }
        } else {
            for (CurU = uHead.Next; CurU != NULL; CurU = CurU->Next) {
                int i;
                i = CurU->i;
                if (CurU->val == state->C[i][minJ]) {

                    oldVal = CurU->val;
                    CurU->val = -EMD_INFINITY;
                    for (CurV = vHead.Next; CurV != NULL; CurV = CurV->Next) {
                        int j;
                        j = CurV->i;
                        if (CurU->val <= state->C[i][j]) {
                            CurU->val = state->C[i][j];
                        }
                    }

                    diff = oldVal - CurU->val;
                    if (fabs(diff) < EPSILON * state->maxC) {
                        for (CurV = vHead.Next; CurV != NULL; CurV = CurV->Next) {
                            Delta[i][CurV->i] += diff;
                        }
                    }
                }
            }
        }
    } while (uHead.Next != NULL || vHead.Next != NULL);
}

static void addBasicVariable(emd_state_t *state, int minI, int minJ, double *S, double *D, emd_node1_t *PrevUMinI, emd_node1_t *PrevVMinJ, emd_node1_t *UHead) {
    double T;

    if (fabs(S[minI] - D[minJ]) <= EPSILON * state->maxW) {
        T = S[minI];
        S[minI] = 0;
        D[minJ] -= T;
    } else if (S[minI] < D[minJ]) {
        T = S[minI];
        S[minI] = 0;
        D[minJ] -= T;
    } else {
        T = D[minJ];
        D[minJ] = 0;
        S[minI] -= T;
    }

    state->IsX[minI][minJ] = 1;

    state->EndX->val = T;
    state->EndX->i = minI;
    state->EndX->j = minJ;
    state->EndX->NextC = state->RowsX[minI];
    state->EndX->NextR = state->ColsX[minJ];
    state->RowsX[minI] = state->EndX;
    state->ColsX[minJ] = state->EndX;
    state->EndX++;

    if (S[minI] == 0 && UHead->Next->Next != NULL) {
        PrevUMinI->Next = PrevUMinI->Next->Next;
    } else {
        PrevVMinJ->Next = PrevVMinJ->Next->Next;
    }
}

#if DEBUG_LEVEL > 0
static void printSolution(emd_state_t *state) {
    emd_node2_t *P;
    double totalCost;

    totalCost = 0;

#if DEBUG_LEVEL > 2
    print("SIG1\tSIG2\tFLOW\tCOST\n");
#endif
    for (P = state->X; P < state->EndX; P++)
        if (P != state->EnterX && state->IsX[P->i][P->j]) {
#if DEBUG_LEVEL > 2
            print("%d\t%d\t%f\t%f\n", P->i, P->j, P->val, state->C[P->i][P->j]);
#endif
            totalCost += (double)P->val * state->C[P->i][P->j];
        }

    print("COST = %f\n", totalCost);
}
#endif