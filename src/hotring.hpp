#ifndef HOTRING_H_
#define HOTRING_H_

#include <vector>
#include "htentry.hpp"

namespace HotRingInstance{ 

    class HotRing{
        public:
            explicit HotRing(size_t sz);

            bool insert(const string &key, const string &val);
            bool remove(const string &key);
            bool update(const string &key, const string &val);
            htEntry *search(const string &key);

            size_t getfindcnt() const;
            size_t getmaxFindcnt() const;
            size_t getminFindcnt() const;
            void hotspot_movement(size_t index);
        private:
            void set_min_max(const size_t onecnt);
        private:
            static constexpr size_t address_mask = 281474976710655; // 2^48 - 1;可以快速得到地址的实际值
            static constexpr size_t R = 5;   // 控制多少次访问后进行热点转移
            inline static std::hash<std::string> hash_fn = std::hash<std::string>();  // 后期可替换为其他哈希函数，比如xxhash
            size_t hash_mask;           // 扩容的时候掩码需要改变，以得到正确的tag
            size_t r = 0;               // 热点调整使用;如果仅在search中使用的话可以使用一个局部static，没必要使用一个r记录

            // 用于测试
            size_t findcnt;       // 统计总的查找次数
            size_t maxFindcnt;    // 统计最大查找次数,一定程度上反应尾延迟
            size_t minFindcnt;    // 统计最小查找次数

            std::vector<hrHead> table; // 哈希表本身
    };

}


#endif