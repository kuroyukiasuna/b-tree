#include "btree.h"
#include "sn/data_node.h"

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

void btree::bt_relocate_root() {
    btree_node* tmp = root->relocate_root();
    delete root;
    root = tmp;
    tmp = nullptr;

    if(!root) {
        root = new btree_node(t);
    }
}

int btree::bt_search(int key) {
    return this->root->search(key);
}

void btree::bt_insert(int key, int value) {
    if(root->is_full())
        bt_split_root();

    root->insert_non_full(data_node<int>(key, value));
}

void btree::bt_delete(int key) {
    root->delete_non_empty(key);

    if(root->is_empty())
        bt_relocate_root();
}

void btree::bt_print() {
    this->root->print_btree_node();
}

int btree::bt_get_degree() {
    return this->t;
}