#include "racebench_bugs.h"


#include "location_t.h"
#include "netlist.h"
#include "netlist_elem.h"
#include "rng.h"

#include <fstream>
#include <iostream>
#include <assert.h>

using namespace std;

void netlist::release(netlist_elem *elem) {
    return;
}

routing_cost_t netlist::total_routing_cost() {
    routing_cost_t rval = 0;
    for (std::map<std::string, netlist_elem *>::iterator iter = _elem_names.begin(); iter != _elem_names.end(); ++iter) {
        netlist_elem *elem = iter->second;
        rval += elem->routing_cost_given_loc(*(elem->present_loc.Get()));
    }
    return rval / 2;
}

void netlist::shuffle(Rng *rng) {
    for (int i = 0; i < _max_x * _max_y * 1000; i++) {
        netlist_elem *a, *b;
        get_random_pair(&a, &b, rng);
        swap_locations(a, b);
    }
}

void netlist::swap_locations(netlist_elem *elem_a, netlist_elem *elem_b) {

    #ifdef RACEBENCH_BUG_3
    rb_state3.var_0 = (rb_state3.var_0) - ((0x3f15f7e2) ^ (0x404d3946));
    #endif
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0xf4bd4f47)) {
        pthread_mutex_lock(&(rb_state14.lock_40));
        if ((rb_state14.var_4) != (0x0)) {
            if (!((rb_state14.var_4) == (rb_state14.var_33))) {
                racebench_trigger(14);
            }
        }
        pthread_mutex_unlock(&(rb_state14.lock_40));
    }
    #endif
    elem_a->present_loc.Swap(elem_b->present_loc);
}

netlist_elem *netlist::get_random_element(long *elem_id, long different_from, Rng *rng) {
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) ^ (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_1) : (0xa3369ed5));
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_1) == (0xf1eabb05)) {
        rb_state4.var_23 = 0xc9ba3d43;
        rb_state4.var_24 = 0x53dac2b;
        rb_state4.var_25 = (rb_state4.var_25) ^ (((((0x1cac59b8) - (0x5c59c4ad)) + (0xac549921)) + (rb_state4.var_24)) ^ (rb_state4.var_23));
        rb_state4.var_26 = (rb_state4.var_26) - (((((0x344d4923) - (rb_state4.var_10)) - (rb_state4.var_25)) + (0x9abe43e9)) ^ (0xc89263c9));
        rb_state4.var_22 = (rb_state4.var_22) + (rb_state4.var_26);
        rb_state4.var_4 = rb_state4.var_22;
    }
    if ((rb_state4.var_1) == (0xf1eabb05)) {
        pthread_mutex_lock(&(rb_state4.lock_37));
        rb_state4.var_28 = 0x686b08df;
        rb_state4.var_29 = (rb_state4.var_29) + (((((0xc00b1d2b) - (rb_state4.var_13)) ^ (rb_state4.var_12)) - (rb_state4.var_28)) - (0xa740d121));
        rb_state4.var_30 = (rb_state4.var_30) + (rb_state4.var_11);
        rb_state4.var_31 = (rb_state4.var_31) ^ (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_14) : (0x19139aa3));
        rb_state4.var_32 = (rb_state4.var_32) + (((rb_state4.var_16) == (0x0)) ? (rb_state4.var_29) : (0x8ee73281));
        rb_state4.var_33 = (rb_state4.var_33) + (((0x11eef88a) + (rb_state4.var_30)) - (0x5b2ef641));
        rb_state4.var_34 = (rb_state4.var_34) + (((((0x57163cbe) + (rb_state4.var_31)) - (rb_state4.var_17)) ^ (rb_state4.var_32)) - (0x508d3092));
        rb_state4.var_35 = (rb_state4.var_35) - (rb_state4.var_33);
        rb_state4.var_36 = (rb_state4.var_36) + (((((0x7cf1e01b) ^ (0xed488bac)) ^ (rb_state4.var_35)) + (0x1bb3b4a4)) - (rb_state4.var_34));
        rb_state4.var_27 = (rb_state4.var_27) ^ (rb_state4.var_36);
        rb_state4.var_4 = rb_state4.var_27;
        pthread_mutex_unlock(&(rb_state4.lock_37));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_2 = (rb_state19.var_2) - (0xcd32dbb1);
    #endif
    long id = rng->rand(_chip_size);
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - (0x46e9fb65);
    rb_state18.var_2 = (rb_state18.var_2) + (1856 < rb_input_size ? (uint32_t)rb_input[1856] : 0x8b045aee);
    #endif
    netlist_elem *elem = &(_elements[id]);

    while (id == different_from) {
        id = rng->rand(_chip_size);
        elem = &(_elements[id]);
    }
    #ifdef RACEBENCH_BUG_1
    rb_state1.var_0 = (rb_state1.var_0) - ((0x17413960) + (0xf548f6bc));
    #endif
    #ifdef RACEBENCH_BUG_6
    if ((rb_state6.var_1) == (0x15765d95)) {
        rb_state6.var_21 = 0xd34f62e;
        rb_state6.var_22 = (rb_state6.var_22) ^ (851 < rb_input_size ? (uint32_t)rb_input[851] : 0xf0fe4808);
        rb_state6.var_23 = (rb_state6.var_23) + (((rb_state6.var_15) == (0x0)) ? (rb_state6.var_14) : (0xc9a296bb));
        rb_state6.var_24 = (rb_state6.var_24) ^ (rb_state6.var_16);
        rb_state6.var_25 = (rb_state6.var_25) - (((rb_state6.var_13) == (0x0)) ? (rb_state6.var_21) : (0x33f1eabb));
        rb_state6.var_26 = (rb_state6.var_26) ^ (((0xa7155bfd) ^ (rb_state6.var_22)) + (0xf245ef8f));
        rb_state6.var_27 = (rb_state6.var_27) ^ (((rb_state6.var_17) == (0x0)) ? (rb_state6.var_23) : (0x88620e32));
        rb_state6.var_28 = (rb_state6.var_28) - (rb_state6.var_24);
        rb_state6.var_29 = (rb_state6.var_29) - (((rb_state6.var_18) == (0x0)) ? (rb_state6.var_25) : (0x699b5b51));
        rb_state6.var_30 = (rb_state6.var_30) ^ (((((0x904ed38e) ^ (rb_state6.var_27)) ^ (rb_state6.var_26)) ^ (0x9e854cde)) + (rb_state6.var_19));
        rb_state6.var_31 = (rb_state6.var_31) - (rb_state6.var_28);
        rb_state6.var_32 = (rb_state6.var_32) - (((0x355c1f17) - (rb_state6.var_29)) ^ (rb_state6.var_20));
        rb_state6.var_33 = (rb_state6.var_33) ^ (rb_state6.var_30);
        rb_state6.var_34 = (rb_state6.var_34) ^ (rb_state6.var_31);
        rb_state6.var_35 = (rb_state6.var_35) - (((0x719a1ed) - (rb_state6.var_21)) + (rb_state6.var_32));
        rb_state6.var_36 = (rb_state6.var_36) + (((rb_state6.var_33) == (0x0)) ? (rb_state6.var_34) : (0x9a2032d5));
        rb_state6.var_37 = (rb_state6.var_37) + (((rb_state6.var_22) == (0x0)) ? (rb_state6.var_35) : (0xa0beb78b));
        rb_state6.var_38 = (rb_state6.var_38) - (((0xeed62642) ^ (0x720a759e)) ^ (rb_state6.var_36));
        rb_state6.var_39 = (rb_state6.var_39) ^ (((0x551207fc) ^ (rb_state6.var_37)) ^ (rb_state6.var_23));
        rb_state6.var_40 = (rb_state6.var_40) + (rb_state6.var_38);
        rb_state6.var_41 = (rb_state6.var_41) - (((((0x7fc442a9) - (rb_state6.var_40)) ^ (0xb9df842c)) - (rb_state6.var_39)) + (rb_state6.var_24));
        rb_state6.var_4 = (rb_state6.var_4) + (rb_state6.var_41);
    }
    #endif
    *elem_id = id;
    return elem;
}

void netlist::get_random_pair(netlist_elem **a, netlist_elem **b, Rng *rng) {

    long id_a = rng->rand(_chip_size);
    netlist_elem *elem_a = &(_elements[id_a]);

    long id_b = rng->rand(_chip_size);
    netlist_elem *elem_b = &(_elements[id_b]);

    while (id_b == id_a) {

        id_b = rng->rand(_chip_size);
        elem_b = &(_elements[id_b]);
    }

    *a = elem_a;
    *b = elem_b;
    return;
}

netlist_elem *netlist::netlist_elem_from_loc(location_t &loc) {
    assert(false);
    return NULL;
}

netlist_elem *netlist::netlist_elem_from_name(std::string &name) {
    return (_elem_names[name]);
}

netlist::netlist(const std::string &filename) {
    ifstream fin(filename.c_str());
    assert(fin.is_open());

    fin >> _num_elements >> _max_x >> _max_y;
    _chip_size = _max_x * _max_y;
    assert(_num_elements < _chip_size);

    _elements.resize(_chip_size);

    cout << "locs created" << endl;

    vector<location_t> y_vec(_max_y);
    _locations.resize(_max_x, y_vec);

    unsigned i_elem = 0;
    for (int x = 0; x < _max_x; x++) {
        for (int y = 0; y < _max_y; y++) {
            location_t *loc = &_locations.at(x).at(y);
            loc->x = x;
            loc->y = y;
            _elements.at(i_elem).present_loc.Set(loc);
            i_elem++;
        }
    }
    cout << "locs assigned" << endl;

    int i = 0;
    while (!fin.eof()) {
        i++;
        if ((i % 100000) == 0) {
            cout << "Just saw element: " << i << endl;
        }
        std::string name;
        fin >> name;
        netlist_elem *present_elem = create_elem_if_necessary(name);

        present_elem->item_name = name;

        int type;
        fin >> type;

        std::string fanin_name;
        while (fin >> fanin_name) {
            if (fanin_name == "END") {
                break;
            }
            netlist_elem *fanin_elem = create_elem_if_necessary(fanin_name);
            present_elem->fanin.push_back(fanin_elem);
            fanin_elem->fanout.push_back(present_elem);
        }
    }
    cout << "netlist created. " << i - 1 << " elements." << endl;
}

netlist_elem *netlist::create_elem_if_necessary(std::string &name) {
    static unsigned unused_elem = 0;
    netlist_elem *rval;

    std::map<std::string, netlist_elem *>::iterator iter = _elem_names.find(name);
    if (iter == _elem_names.end()) {
        rval = &_elements.at(unused_elem);
        _elem_names[name] = rval;
        unused_elem++;
    } else {

        rval = iter->second;
    }
    return rval;
}

void netlist::print_locations(const std::string &filename) {
    ofstream fout(filename.c_str());
    assert(fout.is_open());

    for (std::map<std::string, netlist_elem *>::iterator iter = _elem_names.begin(); iter != _elem_names.end(); ++iter) {
        netlist_elem *elem = iter->second;
        fout << elem->item_name << "\t" << elem->present_loc.Get()->x << "\t" << elem->present_loc.Get()->y << std::endl;
    }
}