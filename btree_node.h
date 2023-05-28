#ifndef BTREE_NODE_H
#define BTREE_NODE_H

class btree_node {
    int size; //current key store size
    int t; //degree
    bool leaf;
    
    int *k; //TODO: allow template type key-value store
    btree_node **children;
    //TODO: implement node level locking so this is thread safe

    //TODO: re-implement using hashset to reduce runtime
    bool contains_key(int k);
    int find_key_or_child(int k);
    int binary_search(int k, int min_val, int max_val);

    public:

    /** ctor & dtor **/
    btree_node(int t);
    virtual ~btree_node();

    /** getters & setters */
    bool is_full();
    bool is_empty();

    /** b-tree functionalities **/
    int search(int key);

    void child_split(int n);
    btree_node* self_split(); //split this*, should only get called if this* is root. 
                              // This is a replacement of the upper level split_root function, created in here
                              // for the sake of encapsulation.

    void insert_non_full(int key); //recursive insertion, split child if full

    void child_merge(int n); //merge children on left and right
    void delete_level_contains(int n); //delete key from this level, given it exists
    int delete_level_not_contain(int n); //delete key from child, given it does not exist on this level
                                         //returns n because n may move during the process
    btree_node* relocate_root();//get root*, should only get called if this* is root.
    void delete_non_empty(int key);

    /** dist utilities **/
    virtual void disk_write();
    virtual void disk_read();

    /** helpers **/
    void print_btree_node();
    void generate_random_full_node();
    void insert_keys(int count);
};
#endif