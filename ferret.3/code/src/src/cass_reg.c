

#include <string.h>
#include <cass.h>

int cass_reg_init(cass_reg_t *reg) {
    ARRAY_INIT(*reg);
    return 0;
}

int cass_reg_init_size(cass_reg_t *reg, cass_size_t size) {
    ARRAY_INIT_SIZE(*reg, size);
    return 0;
}

int cass_reg_cleanup(cass_reg_t *reg) {
    ARRAY_CLEANUP(*reg);
    return 0;
}

int32_t cass_reg_lookup(cass_reg_t *reg, const char *n) {
    ARRAY_BEGIN_FOREACH_P(*reg, cass_reg_entry_t * p) {
        if (strcmp(p->n, n) == 0) {
            return p - reg->data;
        }
    }
    ARRAY_END_FOREACH;
    return -1;
}

int32_t cass_reg_find(cass_reg_t *reg, const void *n) {
    ARRAY_BEGIN_FOREACH_P(*reg, cass_reg_entry_t * p) {
        if (p->p == n) {
            return p - reg->data;
        }
    }
    ARRAY_END_FOREACH;
    return -1;
}

void *cass_reg_get(cass_reg_t *reg, uint32_t i) {
    if (reg->len > i) {
        return reg->data[i].p;
    }
    return NULL;
}

int cass_reg_add(cass_reg_t *reg, const char *name, void *p) {
    cass_reg_entry_t entry = {.n = name, .p = p};
    ARRAY_APPEND(*reg, entry);
    return 0;
}
