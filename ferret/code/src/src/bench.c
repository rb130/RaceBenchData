
#include <cass.h>
#include <cass_bench.h>

extern size_t D, N;

extern float **points;

int benchT_read(benchT *bench, const char *fname, int Q) {
    int i, j;
    FILE *fp = fopen(fname, "r");
    queryT *query;

    if (fp == NULL) {
        return -1;
    }
    bench->qrys = (queryT *)calloc(Q, sizeof(queryT));
    for (i = 0; i < Q; i++) {
        query = &bench->qrys[i];
        if (fscanf(fp, "%d", &(query->qry)) != 1) {
            break;
        }
        fscanf(fp, "%d", &(query->knn));

        query->nabors = (int *)calloc(query->knn, sizeof(int));
        query->dist = (float *)calloc(query->knn, sizeof(float));

        for (j = 0; j < query->knn; j++) {
            fscanf(fp, "%d", &(query->nabors[j]));
            fscanf(fp, "%g", &(query->dist[j]));
        }
    }
    fclose(fp);

    bench->Q = Q;
    bench->score = NULL;

    return 0;
}

void benchT_free(benchT *bench) {
    int i;

    for (i = 0; i < bench->Q; i++) {
        free(bench->qrys[i].nabors);
        free(bench->qrys[i].dist);
    }
    free(bench->qrys);
}

int benchT_qry(benchT *bench, int x) {
    if (x < bench->Q) {
        return bench->qrys[x].qry;
    } else {
        return -1;
    }
}

int *benchT_nabors(benchT *bench, int x) {
    if (x < bench->Q) {
        return bench->qrys[x].nabors;
    } else {
        return NULL;
    }
}

void benchT_print(benchT *bench, int x) {
    int i;
    queryT *query = &bench->qrys[x];

    printf("qry %d:\n", query->qry);
    for (i = 0; i < query->knn; i++) {
        printf(" %d", query->nabors[i]);
    }
    printf("\n");
}

float benchT_score_topk(benchT *bench, int x, int K, int cnt, cass_list_entry_t *results) {
    int i, j;
    int matched = 0;
    float recall;
    queryT *query = &bench->qrys[x];

    for (i = 0; i < K; i++) {
        j = 0;
        while ((j < cnt) && (results[j].id != query->nabors[i])) {
            j++;
        }

        if (j < cnt) {
            matched++;

            if (matched == K) {
                break;
            }
        }
    }

    recall = (float)matched / K;

    return recall;
}

float benchT_prec_topk(benchT *bench, int x, int K, int cnt, ftopk_t *results, float *prec) {
    int i, j;
    float p;
    queryT *query = &bench->qrys[x];

    p = 0;

    for (i = 0; i < K; i++) {
        j = 0;
        while ((j < cnt) && (results[j].index != query->nabors[i])) {
            j++;
        }

        if (j < cnt) {
            prec[i] = (float)(i + 1) / (float)(j + 1);
        } else {
            prec[i] = 0.0;
        }
        p += prec[i];
    }

    p /= K;

    return p;
}

float benchT_score(benchT *bench, int x, int K, int cnt, int *results) {
    int i, j;
    int matched = 0;
    float recall;
    queryT *query = &bench->qrys[x];

    for (i = 0; i < K; i++) {
        j = 0;
        while ((j < cnt) && (results[j] != query->nabors[i])) {
            j++;
        }

        if (j < cnt) {
            matched++;

            if (matched == K) {
                break;
            }
        }
    }

    recall = (float)matched / K;

    return recall;
}
