#include "btree.h"

#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    btree* a = new btree(2);
    a->bt_insert(3);
    a->bt_print();
    delete a;
    return 1;
}