#ifndef HOTRING_H_
#define HOTRING_H_

#include <algorithm>
#include <string>
#include <vector>
#include "htentry.hpp"

namespace HotRingInstance{ 

    class HotRing{
        public:

        private:
            size_t get_index_from_hash_value(htEntry* ht) const & {
                return ht->get_hash_value() & this->hash_mask;
            }
            size_t get_tag_from_hash_value(htEntry* ht) const & {
                return ht->get_hash_value() & (~this->hash_mask);
            }
        private:
            size_t hash_mask;           // 扩容的时候掩码需要改变，以得到正确的tag   
    };

}


#endif