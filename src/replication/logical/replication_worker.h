#ifndef _REPLICATION_WORKER_H
#define _REPLICATION_WORKER_H

#include "tree/btree.h"

#include <string>
#include <thread>
#include <fstream>

class replication_worker {
    bool is_publisher;
    bool replicating;
    int replication_code;
    int replication_progress;

    //TODO: migrate to enum
    std::string replication_status;
    std::string replication_path;

    btree* bt;
    std::thread worker;

    void replicate();
    void publish();
    void sleep_for_delay();
    void log_replication_result();

    public:

    /** ctor & dtor **/
    replication_worker(bool is_publisher, int replication_code, std::string replication_path, btree* bt);
    virtual ~replication_worker();

    /** getters & setters **/
    bool get_is_publisher();
    void set_is_publisher(bool is_publisher);

    int get_replication_code();
    int get_replication_progress();
    std::string get_replication_status();
    std::string get_replication_path();

    /** replication **/
    void start_replication();
    void stop_replication();

    /** wal streaming **/
    virtual std::ifstream get_in_stream();
    virtual std::ofstream get_out_stream();
};
#endif