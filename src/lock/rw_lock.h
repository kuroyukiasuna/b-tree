#ifndef _RW_LOCK_H
#define _RW_LOCK_H

/**
 * R/W lock for node level
 *
 * Note: this lock is not resilient to process crashes, will result in deadlock.
 *
 * TODO: implement better locking solution to achieve isolation levels.
 */
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