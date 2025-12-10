#! /bin/bash

g++ -std=c++17 -pthread  scoped_lock_test.cpp -lgtest -o app
./app