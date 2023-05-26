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

    /** ctor & dtor **/
    btree_node(int t);
    virtual ~btree_node();

    /** getters & setters */
    bool is_full();

    /** b-tree functionalities **/
    void child_split(int n);
    btree_node* self_split(); //split this*, should only get called if this* is root. 
                              // This is a replacement of the upper level split_root function, created in here
                              // for the sake of encapsulation.
    
    void insert_non_full(int key); //recursive insertion, split child if full
    void child_merge(int n); //merge children on left and right

    /** dist utilities **/
    virtual void disk_write();
    virtual void disk_read();

    /** helpers **/
    void print_btree_node();
    void generate_random_full_node();
    void insert_keys(int count);
};
#endif