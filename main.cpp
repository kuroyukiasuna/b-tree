#include "btree.h"

#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    btree_node* a = new btree_node(3);
    a->insert_keys(4);
    a->print_btree_node();
    cout << endl << "////////////////////////////////" << endl << endl;
    a->generate_random_full_node();
    a->print_btree_node();
    cout << endl << "////////////////////////////////" << endl << endl;
    a->child_split(0);
    a->print_btree_node();

    return 1;
}