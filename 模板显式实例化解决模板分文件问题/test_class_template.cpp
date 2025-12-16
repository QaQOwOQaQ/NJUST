#include "test_class_template.hpp"

#include <iostream>
#include <typeinfo>

// 定义成员函数
template<typename T>
void N::X1<T>::f1() {
    std::cout << "X1 f1: " << typeid(T).name() << "a: " << this->a << "\n";
}

template<typename T>
void N::X1<T>::f2() {
    std::cout << "X1 f2: " << typeid(T).name() << "a: " << this->a << "\n";
}

template<typename T>
void N::X2<T>::f1() {
    std::cout << "X2 f1: " << typeid(T).name() << "a: " << this->a << "\n";
}

template<typename T>
void N::X2<T>::f2() {
    std::cout << "X2 f2: " << typeid(T).name() << "a: " << this->a << "\n";
}


template void N::X1<int>::f1(); // 显式实例化定义 成员函数
template<>  // 通过模板全特化实现显式实例化的效果
void N::X1<double>::f1() {
    std::cout << "X1 f1: " << "double\n";
}

template struct N::X2<int>;     // 类模板显式实例化定义

