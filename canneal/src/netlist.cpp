

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
    long id = rng->rand(_chip_size);
    netlist_elem *elem = &(_elements[id]);

    while (id == different_from) {
        id = rng->rand(_chip_size);
        elem = &(_elements[id]);
    }
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
