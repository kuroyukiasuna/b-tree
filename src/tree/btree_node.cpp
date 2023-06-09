#define _VERBOSE_LOG_DISK_RW_ false

#include "btree_node.h"
#include <iostream>
#include <queue>

btree_node::btree_node(int t) : size{0}, t{t}, leaf{true} {
    k = new data_node<int> [2 * t - 1];
    children = new btree_node* [2*t];
    for(int i = 0; i < 2*t; i++) {
        children[i] = nullptr;
    }
    lock = rw_lock();
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

void btree_node::insert_non_full(data_node<int> data, bool serialized) {
    this->lock_for_write(serialized);

    // TODO: hash? allow risky insert?
    if(this->contains_key(data.get_key())) {
        this->unlock_for_write(serialized);
        throw std::runtime_error("key already exists");
    }

    int i = this->size - 1;

    if(this->leaf) {
        while(i >= 0 && data < this->k[i]) {
            this->k[i + 1] = this->k[i];
            i--;
        }
        this->k[i + 1] = data;
        this->size++;
        /* write */
        this->disk_write();
    } else {
        while(i >= 0 && data < this->k[i]) {
            i--;
        }
        i++;

        /* read */
        this->disk_read();
        if(this->children[i]->size == 2 * t - 1) {
            this->child_split(i);
            if(data > this->k[i]) {
                i++;
            }
        }
        try {
            this->children[i]->insert_non_full(data);
        } catch(std::runtime_error e) {
            this->unlock_for_write(serialized);
            throw e;
        }
    }

    this->unlock_for_write(serialized);
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

    /* write */
    this->disk_write();

    l = nullptr;
    delete r;
}

void btree_node::delete_level_contains(int n) {
    data_node<int> replacement_node;
    btree_node* next = nullptr;

    /* read */
    this->disk_read();

    if(this->children[n]->size > t - 1) {
        replacement_node = this->children[n]->k[this->children[n]->size - 1];
        next = this->children[n];
    } else if(this->children[n + 1]->size > t - 1) {
        replacement_node = this->children[n + 1]->k[0];
        next = this->children[n + 1];
    } else {
        replacement_node = this->k[n];
        this->child_merge(n);
        this->children[n]->delete_non_empty(replacement_node.get_key());
        return;
    }

    this->k[n] = replacement_node;

    /* write */
    this->disk_write();
    next->delete_non_empty(replacement_node.get_key());
}

int btree_node::delete_level_not_contain(int n) {

    /* read */
    this->disk_read();

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
        /* write */
        this->disk_write();
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
        /* write */
        this->disk_write();
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

void btree_node::delete_non_empty(int key, bool serialized) {
    /**
     * This write lock here is not optimal, as it only locks this level,
     * while it doesn't prevent changes to the next level.
     *
     * With that being said this simple mutex lock is a best-effort solution.
     * Proper locking mechanism needs to be achieved by designing & implementing
     * better isolation levels.
     *
     */
    this->lock_for_write(serialized);

    try {
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
    } catch(std::runtime_error e) {
        this->unlock_for_write(serialized);
        throw e;
    }
    this->unlock_for_write(serialized);
}

void btree_node::disk_write() {
#if defined(_VERBOSE_LOG_DISK_RW_) && (_VERBOSE_LOG_DISK_RW_ == true)
    std::cout << "writing to memory block " << this << std::endl;
#endif
}

void btree_node::disk_read() {
#if defined(_VERBOSE_LOG_DISK_RW_) && (_VERBOSE_LOG_DISK_RW_ == true)
    std::cout << "reading from memory block " << this << std::endl;
#endif
}

bool btree_node::contains_key(int key) {
    if(this->size == 0)
        return false;

    int cur = find_key_or_child(key);
    return cur < this->size && key == this->k[cur];
}

int btree_node::search(int k, bool serialized) {
    if(this->size == 0)
        return false;

    int cur = find_key_or_child(k);
    if(cur < this->size && k == this->k[cur]) {
        return this->k[cur].get_value();
    } else {

        /* read */
        this->disk_read();
        return this->leaf ? -1 : this->children[cur]->search(k);
    }
}

int btree_node::find_key_or_child(int k) {
    if(k < this->k[0] || this->size == 0)
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

void btree_node::lock_for_write(bool serialized) {
    this->lock.acquire_write();
    if(serialized) {
        this->lock.acquire_read();
    }
}

void btree_node::unlock_for_write(bool serialized) {
    this->lock.unlock_write();
    if(serialized) {
        this->lock.unlock_read();
    }
}

void btree_node::lock_for_read(bool serialized) {
    if(serialized) {
        this->lock.acquire_read();
    }
}

void btree_node::unlock_for_read(bool serialized) {
    if(serialized) {
        this->lock.unlock_read();
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