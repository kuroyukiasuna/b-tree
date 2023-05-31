#ifndef _BTREE_H
#define _BTREE_H
#include "btree_node.h"

#include <queue>
#include <string>

class btree {
    int t; //degree
    btree_node *root;
    std::queue<std::string> in_memory_wal;

    void bt_split_root();
    void bt_relocate_root();

    public:
    btree(int t);
    ~btree();

    int bt_search(int key);
    void bt_insert(int key, int value);
    void bt_delete(int key);
    void bt_print();
    int bt_get_degree();
    std::string get_next_wal();
    bool next_wal();
};
#endif
