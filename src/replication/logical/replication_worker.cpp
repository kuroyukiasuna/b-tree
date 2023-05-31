#define _REPLICATION_TIME_INTERVAL_ 0
#define _VERBOSE_LOG_REPLICATION_RESULT_ false

#include "replication_worker.h"

#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>

replication_worker::replication_worker(bool is_publisher, int replication_code, std::string replication_path, btree* bt) :
    is_publisher{is_publisher},
    replicating{false},
    replication_code{replication_code},
    replication_progress{0},
    replication_status{"NOT REPLICATING"},
    replication_path{replication_path},
    bt{bt}{}

replication_worker::~replication_worker() {
    this->bt = nullptr;
    this->worker.join();
    this->worker.~thread();
}


bool replication_worker::get_is_publisher() {
    return this->is_publisher;
}

void replication_worker::set_is_publisher(bool is_publisher) {
    this->is_publisher = is_publisher;
}

int replication_worker::get_replication_code() {
    return this->replication_code;
}

int replication_worker::get_replication_progress() {
    return this->replication_progress;
}
std::string replication_worker::get_replication_status() {
    return this->replication_status;
}

std::string replication_worker::get_replication_path() {
    return this->replication_path;
}

void replication_worker::start_replication() {
    this->replicating = true;
    if(is_publisher) {
        worker = std::thread(&replication_worker::publish, this);
    } else {
        worker = std::thread(&replication_worker::replicate, this);
    }
}

void replication_worker::replicate() {
    while(this->replicating) {
        this->replication_status = "CATCHING UP";
        std::string wal;

        std::ifstream file(this->replication_path + "wal_" + std::to_string(this->replication_code));
        while(std::getline(file, wal)) {
            int i, k, v;
            char c;

            std::istringstream iss(wal);
            iss >> i >> c;

            if(this->bt && i >= this->replication_progress) {
                this->replication_progress++;

                try {
                    if(c == 'i') {
                        iss >> k >> v;
                        this->bt->bt_insert(k, v);
                    } else if(c == 'd') {
                        iss >> k;
                        this->bt->bt_delete(k);
                    } else {
                        throw std::runtime_error("unrecognized command");
                    }
                } catch(std::runtime_error e) {
                    std::cout << e.what() << std::endl;
                    this->replicating = false;
                    this->replication_status = "NOT REPLICATING";
                    throw e;
                }
                this->log_replication_result();
            }
        }
        file.close();
        this->replication_status = "REPLICATING";
        this->sleep_for_delay();
    }
    this->replication_status = "NOT REPLICATING";
}

void replication_worker::publish() {
    while(this->replicating) {
        this->replication_status = "PUBLISHING";

        std::ofstream file;
        file.open(this->replication_path + "wal_" + std::to_string(this->replication_code), std::ios_base::app);

        if(file.is_open()) {
            while(this->bt && this->bt->next_wal()) {
                std::string wal = this->bt->get_next_wal();
                wal = std::to_string(this->replication_progress) + " " + wal;
                file << wal << std::endl;
                file.flush();
                this->replication_progress++;
            }
            file.close();
        }

        this->replication_status = "REPLICATING";
        this->sleep_for_delay();
    }
    this->replication_status = "NOT REPLICATING";
}

void replication_worker::stop_replication() {
    this->replicating = false;

    //delay exit
    this->sleep_for_delay();
}

void replication_worker::sleep_for_delay() {
#if defined(_REPLICATION_TIME_INTERVAL_)
    sleep(_REPLICATION_TIME_INTERVAL_);
#endif
}

void replication_worker::log_replication_result() {
#if defined(_VERBOSE_LOG_REPLICATION_RESULT_) && (_VERBOSE_LOG_REPLICATION_RESULT_ == true)
    std::cout << std::endl << std::endl << "REPLICATING" << std::endl;
    bt->bt_print();
#endif
}