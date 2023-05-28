#ifndef DATA_NODE_H
#define DATA_NODE_H

#include <ostream>

template <class T>
class data_node {
    int key;
    T value;

    public:
    data_node();
    data_node(int key, T value);
    ~data_node();

    int get_key() const;
    T get_value() const;

};

template <typename T>
bool operator<(const data_node<T>& lhs, const data_node<T>& rhs);

template <typename T>
bool operator>(const data_node<T>& lhs, const data_node<T>& rhs);

template <typename T>
bool operator==(const data_node<T>& lhs, const data_node<T>& rhs);

template <typename T>
bool operator<=(const data_node<T>& lhs, const data_node<T>& rhs);

template <typename T>
bool operator>=(const data_node<T>& lhs, const data_node<T>& rhs);

template <typename T>
bool operator!=(const data_node<T>& lhs, const data_node<T>& rhs);

template <typename T>
bool operator<(const int& lhs, const data_node<T>& rhs);

template <typename T>
bool operator>(const int& lhs, const data_node<T>& rhs);

template <typename T>
bool operator==(const int& lhs, const data_node<T>& rhs);

template <typename T>
bool operator<=(const int& lhs, const data_node<T>& rhs);

template <typename T>
bool operator>=(const int& lhs, const data_node<T>& rhs);

template <typename T>
bool operator!=(const int& lhs, const data_node<T>& rhs);

template <typename T>
std::ostream& operator<<(std::ostream& os, const data_node<T>& dt);

#endif