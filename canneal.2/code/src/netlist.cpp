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

    elem_a->present_loc.Swap(elem_b->present_loc);
}

netlist_elem *netlist::get_random_element(long *elem_id, long different_from, Rng *rng) {
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) - (1584 < rb_input_size ? (uint32_t)rb_input[1584] : 0xb13d0e7d);
    #endif
    long id = rng->rand(_chip_size);
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_1) == (0x978607e2)) {
        if ((rb_state17.var_2) != (0x0)) {
            if (!((rb_state17.var_2) == (rb_state17.var_29))) {
                racebench_trigger(17);
            }
        }
    }
    #endif
    netlist_elem *elem = &(_elements[id]);

    while (id == different_from) {
        id = rng->rand(_chip_size);
        elem = &(_elements[id]);
    }
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_1 = (rb_state2.var_1) ^ ((0x4312c24c) ^ (rb_state2.var_1));
    rb_state2.var_1 = (rb_state2.var_1) + (0x7448461a);
    #endif
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) ^ ((0xde39bf02) ^ (rb_state4.var_0));
    #endif
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_17) == (0xb0243b07)) {
        rb_state10.var_29 = 0x4b555c76;
        rb_state10.var_30 = 0x841e9d2a;
        rb_state10.var_31 = (rb_state10.var_31) + (((rb_state10.var_26) == (0x0)) ? (rb_state10.var_25) : (0x8ddce4bc));
        rb_state10.var_32 = (rb_state10.var_32) + (((rb_state10.var_29) == (0x0)) ? (rb_state10.var_30) : (0x87f72438));
        rb_state10.var_33 = (rb_state10.var_33) + (((rb_state10.var_27) == (0x0)) ? (rb_state10.var_31) : (0xe2ae4705));
        rb_state10.var_34 = (rb_state10.var_34) - (((0xf9e532f0) - (rb_state10.var_32)) - (rb_state10.var_28));
        rb_state10.var_35 = (rb_state10.var_35) ^ (((((0x6f002844) - (rb_state10.var_33)) - (rb_state10.var_30)) + (rb_state10.var_29)) + (rb_state10.var_34));
        rb_state10.var_28 = (rb_state10.var_28) ^ (rb_state10.var_35);
        rb_state10.var_21 = rb_state10.var_28;
    }
    if ((rb_state10.var_17) == (0xb0243b07)) {
        pthread_mutex_lock(&(rb_state10.lock_48));
        rb_state10.var_37 = (rb_state10.var_37) ^ ((0xb6acede0) - (rb_state10.var_34));
        rb_state10.var_38 = (rb_state10.var_38) + (((0x37ce1671) ^ (rb_state10.var_31)) - (0xf0e98506));
        rb_state10.var_39 = (rb_state10.var_39) + (((rb_state10.var_32) == (0x0)) ? (rb_state10.var_33) : (0x25bc338f));
        rb_state10.var_40 = (rb_state10.var_40) ^ (((rb_state10.var_36) == (0x0)) ? (rb_state10.var_35) : (0x62f2528c));
        rb_state10.var_41 = (rb_state10.var_41) + (((rb_state10.var_37) == (0x0)) ? (rb_state10.var_37) : (0xcbd0138d));
        rb_state10.var_42 = (rb_state10.var_42) - (((0x69b5e35b) - (rb_state10.var_38)) ^ (0x7f34d87d));
        rb_state10.var_43 = (rb_state10.var_43) - (((0xc19ac5ce) ^ (0x986b56d1)) ^ (rb_state10.var_39));
        rb_state10.var_44 = (rb_state10.var_44) - (((((0x34c8a760) ^ (0x6f0766c2)) ^ (rb_state10.var_41)) ^ (0x51502da5)) ^ (rb_state10.var_40));
        rb_state10.var_45 = (rb_state10.var_45) + (((((0x57cbf594) + (rb_state10.var_42)) - (rb_state10.var_43)) - (rb_state10.var_38)) ^ (0xa45f9c3f));
        rb_state10.var_46 = (rb_state10.var_46) - (((rb_state10.var_39) == (0x0)) ? (rb_state10.var_44) : (0x5628c614));
        rb_state10.var_47 = (rb_state10.var_47) ^ (((((0x60cd7db0) + (rb_state10.var_46)) ^ (rb_state10.var_41)) ^ (rb_state10.var_40)) ^ (rb_state10.var_45));
        rb_state10.var_36 = (rb_state10.var_36) + (rb_state10.var_47);
        rb_state10.var_21 = rb_state10.var_36;
        pthread_mutex_unlock(&(rb_state10.lock_48));
    }
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_1 = (rb_state12.var_1) - (((rb_state12.var_1) == (0x0)) ? (rb_state12.var_0) : (0x202457be));
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_1 = (rb_state16.var_1) - (rb_state16.var_0);
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