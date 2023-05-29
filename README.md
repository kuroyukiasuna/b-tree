# b-tree

This is a generialized, efficient, thread safe, memory safe b-tree implementation.

### How to use

```main``` class of this repo is an example that uses <int, int> as data node key-value type, with tree degree level 3. Simply clone the repo to local and
```
make
```
to start trying it out.

## Features

### Configurable data node
The data node of this b-tree is a template class type, you can create your own data node type by inheriting from ```data_node``` and specify your typename, or change the template type in ```btree_node```.

Example:
```
class my_data_node : data_node<int> {
    ...
}
```

### Customized disk read/write
Disk read/write functions in ```btree_node``` are virtual. You can create an inheritance of the ```btree_node``` class and override the disk r/w functions so reads/writes to disks are customized for your application's performance.

Example:
```
class my_btree_node : btree_node {
    public:
        void disk_write() override {
            ...
        }

        void disk_read() override {
            ...
        }
}
```

### Thread safe read/write modes
We provide 2 read modes and 2 write modes for use cases of concurrent processing in application:

#### read modes
- Serialized read - waits until every single write operation has completed
- Snapshot read - reads the current state of tree, even if it is changing

#### write modes
- Serialized write - waits until every single read operation has completed
- Optimistic write - writes to the tree disregarding on-going read oprations

R/W modes can be changed in ```btree.cpp```
```
#define _READ_ISOLATION_LEVEL_ snapshot
#define _WRITE_ISOLATION_LEVEL_ serialized
```

### Upcoming features
- Logical replication between trees
- Transaction support
- Isolation level