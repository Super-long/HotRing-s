#include <bits/stdc++.h>
#include <assert.h>
#include "src/hotring.hpp"

using namespace std;

constexpr size_t buckets = 3000;
constexpr size_t len = 10;

int main(){
    HotRingInstance::HotRing ht(buckets);
    auto changeValue = [](size_t para) -> string {
        return to_string(para) + "a";
    };

    auto changeUpdateValue = [](size_t para) -> string {
        return to_string(para) + "aaaaa";
    };

    for (size_t i = 0; i < buckets*len; i++){ // 链平均长度为3
        ht.insert(to_string(i), changeValue(i));
    }


    for (size_t i = 0; i < buckets*len/2; i++){
        assert(ht.remove(to_string(i)));
    }

    for (size_t i = 0; i < buckets*len; i++){
        if(i < buckets*len/2){
            assert(!ht.search(to_string(i)));
        } else {
            assert(ht.search(to_string(i)));
        }
    }

    cout << "Pass remove test.\n" << endl;

    for (size_t i = buckets*len; i < buckets*len*2; i++){
        assert(!ht.remove(to_string(i)));
    }

    cout << "Pass remove out of bound test.\n" << endl;

    for (size_t i = buckets*len/2; i < buckets*len; i++){
        ht.update(to_string(i), changeUpdateValue(i));
    }

    for (size_t i = buckets*len/2; i < buckets*len; i++){
        assert(ht.search(to_string(i))->get_val() == changeUpdateValue(i));
    }

    cout << "Pass update test.\n" << endl;
    return 0;
}