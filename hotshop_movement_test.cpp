#include <bits/stdc++.h>
#include <assert.h>
#include "src/hotring.hpp"

using namespace std;

constexpr size_t buckets = 3;
constexpr size_t len = 3;

int main(){
    HotRingInstance::HotRing ht(buckets);
    for (size_t i = 0; i < buckets*len; i++){ // 链平均长度为3
        ht.insert(to_string(i), to_string(i) + "a");
        cout << "hello\n";
    }
    cout << "world\n";
    for (size_t i = 0; i < buckets*len; i++){
        cout << ht.search(to_string(i)) << endl;
    }

    return 0;
}