#define _READ_ISOLATION_LEVEL_ snapshot
#define _WRITE_ISOLATION_LEVEL_ serialized

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
    bool serialized_read = true;
    
#if defined(_READ_ISOLATION_LEVEL_) && (_READ_ISOLATION_LEVEL_ == snapshot)
    serialized_read = false;
#endif

    return this->root->search(key, serialized_read);
}

void btree::bt_insert(int key, int value) {
    if(root->is_full())
        bt_split_root();

    bool serialized_write = true;
    
#if defined(_WRITE_ISOLATION_LEVEL_) && (_WRITE_ISOLATION_LEVEL_ == optimistic)
    serialized_write = false;
#endif
    root->insert_non_full(data_node<int>(key, value), serialized_write);
}

void btree::bt_delete(int key) {
    bool serialized_write = true;
    
#if defined(_WRITE_ISOLATION_LEVEL_) && (_WRITE_ISOLATION_LEVEL_ == optimistic)
    serialized_write = false;
#endif

    root->delete_non_empty(key, serialized_write);

    if(root->is_empty())
        bt_relocate_root();
}

void btree::bt_print() {
    this->root->print_btree_node();
}

int btree::bt_get_degree() {
    return this->t;
}