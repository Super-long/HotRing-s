#include <bits/stdc++.h>
#include <assert.h>
#include "src/hotring.hpp"

using namespace std;

constexpr size_t buckets = 3000;
constexpr size_t len = 10;

int main(){
    HotRingInstance::HotRing ht(buckets);

    auto changeValue  = [](size_t para) -> string {
        return to_string(para) + "a";
    };

    auto chengeKey = [&](size_t para) -> size_t {   // 保证参数有效是我们的事情，所以需要在函数里面加语句
        size_t base = buckets*len/100;
        return para%base;   // 保证读取百分之一的热点数据
    };

    for (size_t i = 0; i < buckets*len; i++){ // 链平均长度为3
        ht.insert(to_string(i), changeValue(i));
    }

    // 无热点读取
    auto start = std::chrono::high_resolution_clock::now(); 

    for (size_t i = 0; i < buckets*len; i++){
        assert(ht.search(to_string(i))->get_val() == changeValue(i)); 
    }

    auto end = std::chrono::high_resolution_clock::now(); 

    cout << "HashTable:" << endl;
    cout << "findcnt:" << ht.getfindcnt() << "次" << endl;
    cout << "maxFindcnt:" << ht.getmaxFindcnt() << "次" << endl;
    cout << "minFindcnt:" << ht.getminFindcnt() << "次" << endl;
    cout << "averageFindcnt:" << (double)ht.getfindcnt() / (buckets*len) << "次" << endl;
    
    std::chrono::duration<double, std::ratio<1,1000>> time_span     // 毫秒
    = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1,1000>>>(end - start);

    std::cout << time_span.count() << std::endl;

    ht.clear_metadata();

    cout << "\n----------------------------------------------------\n\n";

    // 热点读取
    start = std::chrono::high_resolution_clock::now(); 

    for (size_t i = 0; i < buckets*len; i++){   // 比较极端，全部的读请求读取百分之一的热点数据
        assert(ht.search(to_string(chengeKey(i)))->get_val() == changeValue(chengeKey(i))); 
    }

    end = std::chrono::high_resolution_clock::now(); 

    cout << "HashTable:" << endl;
    cout << "findcnt:" << ht.getfindcnt() << "次" << endl;
    cout << "maxFindcnt:" << ht.getmaxFindcnt() << "次" << endl;
    cout << "minFindcnt:" << ht.getminFindcnt() << "次" << endl;
    cout << "averageFindcnt:" << (double)ht.getfindcnt() / (buckets*len) << "次" << endl;
    time_span = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1,1000>>>(end - start);
    std::cout << time_span.count() << std::endl;

    cout << "\nPass insert and target.\n";

    return 0;
}