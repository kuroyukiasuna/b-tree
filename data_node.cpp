#include "data_node.h"

template <typename T>
data_node<T>::data_node() : key{0} {};

template <typename T>
data_node<T>::data_node(int key, T value) : key{key}, value{value} {}

template <typename T>
data_node<T>::~data_node() {}

template <typename T>
int data_node<T>::get_key() const {
    return this->key;
}

template<typename T>
T data_node<T>::get_value() const {
    return this->value;
}

template <typename T>
bool operator<(const data_node<T>& lhs, const data_node<T>& rhs) {
    return lhs.get_key() < rhs.get_key();
}

template <typename T>
bool operator>(const data_node<T>& lhs, const data_node<T>& rhs) {
    return lhs.get_key() > rhs.get_key();
}

template <typename T>
bool operator==(const data_node<T>& lhs, const data_node<T>& rhs) {
    return lhs.get_key() == rhs.get_key();
}

template <typename T>
bool operator<=(const data_node<T>& lhs, const data_node<T>& rhs) {
    return lhs.get_key() <= rhs.get_key();
}

template <typename T>
bool operator>=(const data_node<T>& lhs, const data_node<T>& rhs) {
    return lhs.get_key() >= rhs.get_key();
}

template <typename T>
bool operator!=(const data_node<T>& lhs, const data_node<T>& rhs) {
    return lhs.get_key() != rhs.get_key();
}

template <typename T>
bool operator<(const int& lhs, const data_node<T>& rhs) {
    return lhs < rhs.get_key();
}

template <typename T>
bool operator>(const int& lhs, const data_node<T>& rhs) {
    return lhs > rhs.get_key();
}

template <typename T>
bool operator==(const int& lhs, const data_node<T>& rhs) {
    return lhs == rhs.get_key();
}

template <typename T>
bool operator<=(const int& lhs, const data_node<T>& rhs) {
    return lhs <= rhs.get_key();
}

template <typename T>
bool operator>=(const int& lhs, const data_node<T>& rhs) {
    return lhs >= rhs.get_key();
}

template <typename T>
bool operator!=(const int& lhs, const data_node<T>& rhs) {
    return lhs != rhs.get_key();
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const data_node<T>& dn) {
    os << dn.get_key();
    return os;
}