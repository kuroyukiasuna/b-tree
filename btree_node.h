#ifndef BTREE_NODE_H
#define BTREE_NODE_H

class btree_node {
    int size; //current key store size
    int t; //degree
    bool leaf;
    
    int *k; //TODO: allow template type key-value store
    btree_node **children;
    //TODO: implement node level locking so this is thread safe

    public:
    btree_node(int t);
    virtual ~btree_node();

    virtual void disk_write();

    void print_btree_node();
};
#endif