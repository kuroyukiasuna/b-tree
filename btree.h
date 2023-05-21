#ifndef _BTREE_H
#define _BTREE_H
#include "btree_node.h"

class btree {
    int t; //degree
    btree_node *root;

    public:
    btree(int t);
    ~btree();

    int bt_search(int key);
    void bt_insert(int key);
    void bt_delete(int key);
    void bt_print();
};
#endif
