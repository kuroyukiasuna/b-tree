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

bool btree_node::is_full() {
    return this->size == 2 * this->t - 1;
}

bool btree_node::is_empty() {
    return this->size == 0;
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
    if(this->contains_key(key)) {
        throw std::runtime_error("key already exists");
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

void btree_node::child_merge(int n) {
    if(this->leaf)
        throw std::runtime_error("merge should not execute on leaf node");

    btree_node* l = this->children[n];
    btree_node* r = this->children[n + 1];

    if(!l || !r)
        throw std::runtime_error("subnodes pending merge need to be not null");

    if(l->size != t - 1 || r->size != t - 1)
        throw std::runtime_error("subnodes needs to have " + std::to_string(t - 1) + " nodes to be merge-able");

    l->k[t - 1] = this->k[n];

    //until 2t - 2
    for(int i = 0; i < t - 1; i++) {
        l->k[i + t] = r->k[i];
    }

    if(!l->leaf) {
        //until 2t - 1
        for(int i = 0; i < t; i++) {
            l->children[i + t] = r->children[i];
            r->children[i] = nullptr;
        }
    }

    l->size = 2*t - 1;

    for(int i = n; i < this->size - 1; i++) {
        this->k[i] = this->k[i + 1];
    }

    for(int i = n + 1; i < this->size; i++) {
        this->children[i] = this->children[i + 1];
    }
    this->children[this->size] = nullptr;
    this->size--;

    l = nullptr;
    delete r;
}

void btree_node::delete_level_contains(int n) {
    int replacement_key;
    btree_node* next = nullptr;

    if(this->children[n]->size > t - 1) {
        replacement_key = this->children[n]->k[this->children[n]->size - 1];
        next = this->children[n];
    } else if(this->children[n + 1]->size > t - 1) {
        replacement_key = this->children[n + 1]->k[0];
        next = this->children[n + 1];
    } else {
        replacement_key = this->k[n];
        this->child_merge(n);
        this->children[n]->delete_non_empty(replacement_key);
        return;
    }

    this->k[n] = replacement_key;
    next->delete_non_empty(replacement_key);
}

int btree_node::delete_level_not_contain(int n) {
    if(n > 0 && this->children[n - 1]->size > t - 1) {
        //Shift from left
        btree_node* cur = this->children[n];
        btree_node* l = this->children[n - 1];

        for(int i = cur->size; i > 0; i--) {
            cur->k[i] = cur->k[i - 1];
        }
        cur->k[0] = this->k[n - 1];

        if(!l->leaf) {
            for(int i = cur->size + 1; i > 0; i--) {
                cur->children[i] = cur->children[i - 1];
            }
            cur->children[0] = l->children[l->size];
            l->children[l->size] = nullptr;
        }
        cur->size++;

        this->k[n - 1] = l->k[l->size - 1];
        l->size--;
        cur = nullptr;
        l = nullptr;
    } else if(n < this->size && this->children[n + 1]->size > t - 1) {
        //Shift from right
        btree_node* cur = this->children[n];
        btree_node* r = this->children[n + 1];

        cur->k[cur->size] = this->k[n];
        this->k[n] = r->k[0];

        for(int i = 0; i < r->size - 1; i++) {
            r->k[i] = r->k[i + 1];
        }

        if(!r->leaf) {
            cur->children[cur->size + 1] = r->children[0];
            for(int i = 0; i < r->size; i++) {
                r->children[i] = r->children[i + 1];
            }
            r->children[r->size] = nullptr;
        }

        r->size--;
        cur->size++;
        cur = nullptr;
        r = nullptr;
    } else {
        //Merge
        if(n == this->size) {
            this->child_merge(n - 1);
            return n - 1;
        } else {
            this->child_merge(n);
        }
    }
    return n;
}

btree_node* btree_node::relocate_root() {
    btree_node* tmp = this->children[0];
    this->children[0] = nullptr;
    return tmp;
}

void btree_node::delete_non_empty(int key) {
    bool level_contains_key = this->contains_key(key);
    int n = this->find_key_or_child(key);

    if(this->leaf) {
        if(level_contains_key) {
            for(int i = n; i < this->size - 1; i++) {
                this->k[i] = this->k[i + 1];
            }
            this->size--;
        } else {
            throw std::runtime_error("Key [" + std::to_string(key) + "] does not exist in the tree");
        }
    } else {
        if(level_contains_key) {
            this->delete_level_contains(n);
        } else {
            if(this->children[n]->size <= t - 1) {
                n = this->delete_level_not_contain(n);
            }
            this->children[n]->delete_non_empty(key);
        }
    }
}

void btree_node::disk_write() {
    std::cout << "writing to disk" << std::endl;
}

void btree_node::disk_read() {
    std::cout << "reading from disk" << std::endl;
}

bool btree_node::contains_key(int key) {
    int cur = find_key_or_child(key);
    return cur < this->size && this->k[cur] == key;
}

int btree_node::search(int k) {
    int cur = find_key_or_child(k);
    if(cur < this->size && this->k[cur] == k) {
        return this->k[cur];
    } else {
        return this->leaf ? -1 : this->children[cur]->search(k);
    }
}

int btree_node::find_key_or_child(int k) {
    if(k < this->k[0])
        return 0;

    if(k > this->k[size - 1])
        return this->size;
        
    return binary_search(k, 0, this->size - 1);
}

int btree_node::binary_search(int k, int min_val, int max_val) {
    if(min_val + 1 == max_val) {
        if(k == this->k[min_val]) {
            return min_val;
        } else if(k == this->k[max_val]) {
            return max_val;
        } else if(k > this->k[min_val] && k < this->k[max_val]) {
            return max_val;
        } else {
            throw std::runtime_error("Wrong binary search result");
        }
    }
    
    int mid = (min_val + max_val) / 2;

    if(k == this->k[mid]) {
        return mid;
    } else if(k > this->k[mid]) {
        if(k < this->k[mid + 1]) {
            return mid + 1;
        } else {
            return binary_search(k, mid, max_val);
        }
    } else {
        if(k > this->k[mid - 1]) {
            return mid;
        } else {
            return binary_search(k, min_val, mid);
        }
    }
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