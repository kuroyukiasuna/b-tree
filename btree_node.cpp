#include "btree_node.h"
#include <iostream>
#include <queue>

btree_node::btree_node(int t) : size{0}, t{t}, leaf{true} {
    k = new int [2 * t - 1];
    children = new btree_node* [2*t];
    for(int i = 0; i < 2*t; i++) {
        children[i] = nullptr;
    }
}

btree_node::~btree_node() {
    delete [] k;
    for(int i = 0; i < 2*t; i++) {
        if(!children[i])
            continue;
        delete children[i];
    }
    delete [] children;
}

void btree_node::disk_write() {
    std::cout<< "written to disk" << std::endl;
}

void btree_node::print_btree_node() {
    std::queue<btree_node*> bfs;
    bfs.push(this);

    while(!bfs.empty()) {
        int s = bfs.size();
        for(int i = 0; i < s; i++) {
            btree_node* cur = bfs.front();
            for(int i = 0; i < 2*t; i++) {
                if(!cur->children[i])
                    continue;

                bfs.push(cur->children[i]);
            }

            for(int i = 0; i < 2*t - 1; i++) {
                std::cout << cur->k[i] << " ";
            }
            std::cout << "  ";

            bfs.pop();
        }
        std::cout << std::endl;
    }
}