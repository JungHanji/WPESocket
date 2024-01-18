#pragma once
#include <map>
#include <memory>
#include <stdexcept>
#include <functional>
#include <iostream>
#include <cerrno>
#include <thread>
#include <mutex>

using namespace std;

// utils

template<class K, class V>
bool isKeyInMap(map<K, V> &mp, K &elem){
    return mp.find(elem) != mp.end();
}