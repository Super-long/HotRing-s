#include <bits/stdc++.h>
#include <assert.h>
#include "src/htentry.hpp"

using namespace std;

// 用于测试htentry中一些位运算是否正确

int main(){
    string key1 = "one";
    string key2 = "two";
    string value = "world";
    auto one = new HotRingInstance::htEntry(key1, value); 
    auto two = new HotRingInstance::htEntry(key2, value);
    
    // 使得两者的next都有效
    one->set_next(two);
    two->set_next(one);

    one->set_occupied();
    two->set_occupied();

    int sum = 10;
    int target = 1024;
    while(sum--){
        for (size_t i = 0; i < target; i++){
            if(i&1){
                one->inc_counter();
            } else {
                two->inc_counter();
            }
        }
        assert(one->get_counter() == target/2); // 当然奇数的话就gg了
        assert(two->get_counter() == target/2);

        one->reset_counter();
        two->reset_counter();
    }

    assert(one->get_occupied());
    assert(two->get_occupied());
    one->reset_occupied();
    two->reset_occupied();


    cout << "Pass counter target.\n";

    one->set_occupied();
    two->set_occupied();

    sum = 10;
    int base = 10;
    target = (1 << 17) + base;
    while(sum--){
        for (size_t i = 0; i < target; i++){
            if(i&1){
                one->inc_counter();
            } else {
                two->inc_counter();
            }
        }
        assert(one->get_counter() == base/2);
        assert(two->get_counter() == base/2);

        one->reset_counter();
        two->reset_counter();
    }

    assert(one->get_occupied());
    assert(two->get_occupied());
    one->reset_occupied();
    two->reset_occupied();

    cout << "Pass counter Out of bounds.\n";

    sum = 10;

    while(sum--){
        one->set_occupied();
        assert(one->get_occupied());

        one->reset_occupied();
        assert(!one->get_occupied());

        two->set_occupied();
        assert(two->get_occupied());

        two->reset_occupied();
        assert(!two->get_occupied());
    }

    cout << "Pass occupied target.\n";

    sum = 10;

    while(sum--){
        one->set_rehash();
        assert(one->get_rehash());

        one->reset_rehash();
        assert(!one->get_rehash());

        two->set_rehash();
        assert(two->get_rehash());

        two->reset_rehash();
        assert(!two->get_rehash());
    }

    cout << "Pass rehash target.\n";

    return 0;
}   