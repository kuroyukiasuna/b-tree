#include "rw_lock.h"

rw_lock::rw_lock() : read{true}, write{false} {}

void rw_lock::acquire_read() {
    while(this->read);
    this->read = true;
}

void rw_lock::acquire_write() {
    while(this->write);
    this->write = true;
}

void rw_lock::unlock_read() {
    this->read = false;
}

void rw_lock::unlock_write() {
    this->write = false;
}