
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "buffer.h"
#include "common.h"

Fnode::Fnode(int itemno, int number) {
    itemname = itemno;
    leftchild = NULL;
    rightsibling = NULL;
    count = number;
}

void Fnode::init(int Itemname, int Count) {
    itemname = Itemname;
    leftchild = NULL;
    rightsibling = NULL;
    count = Count;
}

Fnode::~Fnode() {
    Fnode *temp, *temp1;
    ;
    if (this->leftchild != NULL) {
        temp = this->leftchild;
        temp1 = temp->rightsibling;
        while (temp1 != NULL) {
            temp = temp1->rightsibling;
            ;
            delete temp1;
            temp1 = temp;
        }
        delete this->leftchild;
    }
}
