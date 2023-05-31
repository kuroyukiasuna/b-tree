#include "tree/btree.h"
#include "replication/logical/replication_worker.h"

#include <iostream>
#include <sstream>

using namespace std;

int main(int argc, char* argv[]) {
    btree* a = new btree(3);
    replication_worker* wk = nullptr;
    int prev_return_code = 0;
    string wal_dir = "./wal_dir/";

    if(argc == 3) {
        if(strcmp( argv[1], "--wal") == 0) {
            wal_dir = argv[2];
        }
    }

    while(true) {
        cout << "btree cli <" << prev_return_code << ">: ";
        string s; //cur
        string s_p; //parsed
        int cycle_return_code = 0;
        getline(cin, s);

        istringstream iss(s);
        iss >> s_p;

        if(s_p == "insert") {
            int i;
            while(iss >> i) {
                try {
                    a->bt_insert(i, i);
                } catch(runtime_error &e) {
                    cout << e.what() << endl;
                    cycle_return_code = 1;
                }
            }
            a->bt_print();
        } else if(s_p == "delete") {
            int i;
            while(iss >> i) {
                try{
                    a->bt_delete(i);
                } catch(runtime_error &e) {
                    cout << e.what() << endl;
                    cycle_return_code = 1;
                }
            }
            a->bt_print();
        } else if(s_p == "search") {
            int i;
            iss >> i;
            cout << a->bt_search(i) << endl;
        } else if(s_p == "print") {
            a->bt_print();
        } else if(s_p == "publish") {
            int i;
            iss >> i;
            wk = new replication_worker(true, i, wal_dir, a);
            wk->start_replication();
            cout << "Started publishing WAL" << endl;
        } else if(s_p == "replicate") {
            int i;
            iss >> i;
            wk = new replication_worker(false, i, wal_dir, a);
            wk->start_replication();
            cout << "Started replicating with WAL" << endl;
        } else if(s_p == "exit" || s_p == "\\q" || s_p == "q") {
            wk->stop_replication();
            break;
        }
        else {
            cout << "Supported Commands:" << endl;
            cout << "search - find a key in btree" << endl;
            cout << "insert - inert a key into btree" << endl;
            cout << "delete - delete a key from btree" << endl;
            cout << "print - print the btree" << endl;
            cout << "help - show this manual" << endl;
            cout << "exit or q - exit" << endl;
        }
        prev_return_code = cycle_return_code;
    }

    delete wk;
    delete a;
    return 1;
}