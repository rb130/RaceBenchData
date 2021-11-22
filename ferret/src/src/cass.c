

#include <cass.h>
#include <cass_dist.h>

cass_reg_t cass_vec_dist_class_reg;
cass_reg_t cass_vecset_dist_class_reg;
cass_reg_t cass_table_opr_reg;

int cass_init(void) {
    cass_vec_dist_class_init();
    cass_vecset_dist_class_init();
    cass_table_opr_init();

    cass_table_opr_add(opr_raw.name, &opr_raw);
    cass_table_opr_add(opr_lsh.name, &opr_lsh);

    cass_vec_dist_class_add(&vec_dist_trivial);
    cass_vec_dist_class_add(&vec_dist_L1_int);
    cass_vec_dist_class_add(&vec_dist_L2_int);
    cass_vec_dist_class_add(&vec_dist_L1_float);
    cass_vec_dist_class_add(&vec_dist_L2_float);
    cass_vec_dist_class_add(&vec_dist_hamming);
    cass_vec_dist_class_add(&vec_dist_cos_float);

    cass_vecset_dist_class_add(&vecset_dist_trivial);
    cass_vecset_dist_class_add(&vecset_dist_single);
    cass_vecset_dist_class_add(&vecset_dist_emd);
    cass_vecset_dist_class_add(&vecset_dist_myemd);

    return 0;
}

int cass_cleanup(void) {
    cass_table_opr_cleanup();
    cass_vecset_dist_class_cleanup();
    cass_vec_dist_class_cleanup();

    return 0;
}
