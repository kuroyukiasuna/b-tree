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

void btree_node::child_split(int n) {
    //Create a new tree node
    btree_node *new_node = new btree_node(t);
    btree_node *old_node = this->children[n];

    //Inherit leaf value
    new_node->leaf = old_node->leaf;

    //from t to 2t - 2
    for(int i = 0; i < t - 1; i++) {
        new_node->k[i] = old_node->k[i + t];
    }

    if(!new_node->leaf) {
        //from t to 2t-1
        for(int i = 0; i < t; i++) {
            new_node->children[i] = old_node->children[i + t];
            old_node->children[i + t] = nullptr;
        }
    }

    new_node->size = old_node->size = t - 1;

    for(int i = this->size; i > n; i--) {
        this->k[i] = this->k[i-1];
        this->children[i + 1] = this->children[i];
    }

    this->k[n] = old_node->k[old_node->size];
    this->children[n + 1] = new_node;
    this->size++;
    old_node = nullptr;
    new_node = nullptr;
}

btree_node* btree_node::self_split() {
    btree_node* tmp_root = new btree_node(t);
    tmp_root->leaf = false;
    tmp_root->children[0] = this;
    tmp_root->child_split(0);
    return tmp_root;
}

void btree_node::insert_non_full(int key) {

    // TODO: hash? allow risky insert?
    for(int j = 0; j < this->size; j++) {
        if(this->k[j] == key) {
            throw std::runtime_error("key already exists");
        }
    }

    int i = this->size - 1;

    if(this->leaf) {
        while(i >= 0 && key < this->k[i]) {
            this->k[i + 1] = this->k[i];
            i--;
        }
        this->k[i + 1] = key;
        this->size++;
        //disk-write
    } else {
        while(i >= 0 && key < this->k[i]) {
            i--;
        }
        i++;
        //disk-read
        if(this->children[i]->size == 2 * t - 1) {
            this->child_split(i);
            if(key > this->k[i]) {
                i++;
            }
        }
        this->children[i]->insert_non_full(key);
    }
}

void btree_node::disk_write() {
    std::cout << "writing to disk" << std::endl;
}

void btree_node::disk_read() {
    std::cout << "reading from disk" << std::endl;
}

void btree_node::print_btree_node() {
    std::queue<btree_node*> bfs;
    bfs.push(this);

    while(!bfs.empty()) {
        int s = bfs.size();
        for(int i = 0; i < s; i++) {
            btree_node* cur = bfs.front();
            for(int i = 0; i <= cur->size; i++) {
                if(!cur->children[i])
                    continue;

                bfs.push(cur->children[i]);
            }

            for(int i = 0; i < cur->size; i++) {
                std::cout << cur->k[i] << " ";
            }
            std::cout << "  ";

            bfs.pop();
        }
        std::cout << std::endl;
    }
}

void btree_node::insert_keys(int count) {
    this->size = count;

    for(int i = 0; i < count; i++) {
        this->k[i] = i + 1;
        this->children[i] = new btree_node(t);
    }
    this->children[count] = new btree_node(t);
}

void btree_node::generate_random_full_node() {
    this->leaf = false;
    this->children[0]->insert_keys(5);
    this->children[1]->insert_keys(2);
    this->children[2]->insert_keys(3);
    this->children[3]->insert_keys(3);
    this->children[4]->insert_keys(2);
    this->children[5]->insert_keys(3);
}