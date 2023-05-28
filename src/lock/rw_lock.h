#ifndef _RW_LOCK_H
#define _RW_LOCK_H

class rw_lock {
    bool read;
    bool write;
public:
    rw_lock();
    void acquire_read();
    void acquire_write();
    void unlock_read();
    void unlock_write();
};
#endif