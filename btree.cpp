#include "btree.h"
#include <iostream>

btree::btree(int t) : t{t} {
    this->root = new btree_node(t);
}

btree::~btree() {
    delete this->root;
}

void btree::bt_split_root() {
    this->root = this->root->self_split();
}

int btree::bt_search(int key) {
    std::cout << "search" << std::endl;
    return 1;
}

void btree::bt_insert(int key) {
    if(root->is_full())
        bt_split_root();
    root->insert_non_full(key);
}

void btree::bt_delete(int key) {
    std::cout << "delete" << std::endl;
}

void btree::bt_print() {
    this->root->print_btree_node();
}