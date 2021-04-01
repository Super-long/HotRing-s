#include <string>
#include <limits.h>
#include <algorithm>
#include "hotring.hpp"

namespace HotRingInstance{
    HotRing::HotRing(size_t sz):table(0), findcnt(0), minFindcnt(0x7fffffff), maxFindcnt(0) {
        size_t htsz = 1;
        while (htsz < sz) htsz <<= 1;   // 把容量扩充为参数的整数次幂
        table.resize(htsz);
        hash_mask = htsz - 1;
    } 

    bool HotRing::insert(const string &key, const string &val){
        size_t hashValue = hash_fn(key);
        size_t index = hashValue & hash_mask;
        size_t tag = hashValue & (~hash_mask);

        htEntry *newItem = new htEntry(key, val, nullptr, tag >> __builtin_popcountl(hash_mask));
        htEntry *pre = nullptr;
        htEntry *nxt = nullptr;

        if(table[index].get_head() == nullptr){
            table[index].set_head(newItem);
            newItem->set_next(newItem);
        } else if(table[index].get_head()->get_next() == table[index].get_head()){
            // 环中只有一项，随便插入都有序
            table[index].get_head()->set_next(newItem);
            newItem->set_next(table[index].get_head());
        } else {    // 环中有多项数据
            pre = table[index].get_head();
            nxt = table[index].get_head()->get_next();
            while (true) {
                // 先比较tag，相同再比较key，可以避免大量的字符串比较；相同的键无需插入
                if(newItem->get_tag() == pre->get_tag() && key == pre->get_key()) {return false;}

                if (((*pre) < (newItem) && (*newItem) < (nxt)) ||     //ordre_i-1 < order_k < order_i
                    ((*newItem) < (nxt) && (*nxt) < (pre))     ||     //order_k < order_i < order_i-1 小于有序环最小值
                    ((*nxt) < (pre) && (*pre) < (newItem)))           //order_i < order_i-1 < order_k 大于有序环最大值
                {
                    newItem->set_next(nxt);
                    pre->set_next(newItem);
                    break;
                }
                nxt = nxt->get_next();
                pre = pre->get_next();
            }
        }
        return true;
    }

    bool HotRing::remove(const string &key){

    }

    bool HotRing::update(const string &key, const string &val){

    }

    htEntry* HotRing::search(const string &key){
        size_t hashValue = hash_fn(key);
        size_t index = hashValue & hash_mask;
        size_t tag = hashValue & (~hash_mask);

        htEntry* pre = nullptr;
        htEntry* nxt = nullptr;
        htEntry* res = nullptr;

        size_t precnt = findcnt;
        htEntry compareItem(key, "", nullptr, tag);
        bool hotspotAware = false;

        // 在每R个请求完成之后，我们确定是否启动新一轮采样，设置active状态。
        // 如果第R次访问是热访问，则意味着当前热点标识仍然准确，并且无需触发采样
        ++this->r;
        table[index].inc_counter(); // 整个环的访问数加1

        if(table[index].get_head() == nullptr){ // 环中没有元素
            res = nullptr;
        } else if(table[index].get_head()->get_next() == table[index].get_head()){  // 环中有一个元素
            if(table[index].get_head()->get_key() == key){
                res = table[index].get_head();
                if(r >= R){ // 第一个if可能导致加到等于R时没有任何行为，所以这里h和后面需要大于等于R
                    // 只有一个元素，它本身当然就是热点了，所以不需要触发采样
                    r == 0;
                }
            }
        } else {    // 环中有很多元素
            pre = table[index].get_head();
            if(table[index].get_active()){ // 3.2.2，active标志存在才进行采样
                pre->inc_counter(); 
            }
            nxt = table[index].get_head()->get_next();
            while (true) {
                if(compareItem.get_tag() == pre->get_tag() && key == pre->get_key()){
                    if(pre != table[index].get_head() && r >= R){   // 第R次访问不是热结点，开启采样;当热点调整的时候重置
                        if(table[index].get_active()){  // 保证在设置后的第R次进行热点调整
                            hotspot_movement(index);    // 其中清空了每个数据项的统计信息
                            table[index].reset_active();
                            table[index].reset_counter();
                            r = 0;
                        } else {
                            table[index].set_active();
                            r = 0;
                            break;
                        }
                    }
                }

                if (((*pre) < (&compareItem) && (compareItem) < (nxt)) ||       //ordre_i-1 < order_k < order_i
                    ((compareItem) < (nxt) && (*nxt) < (pre))     ||            //order_k < order_i < order_i-1 小于有序环最小值
                    ((*nxt) < (pre) && (*pre) < (&compareItem)))                //order_i < order_i-1 < order_k 大于有序环最大值
                {
                    break;
                }
                nxt = nxt->get_next();
                pre = pre->get_next();
                ++findcnt;
            }
        }

        set_min_max(findcnt - precnt);

        return res;
    }

    // 以index为下标的冲突环,顺便清空统计结果
    void HotRing::hotspot_movement(size_t index){
        htEntry* pre = table[index].get_head();
        if(!pre) return;
        htEntry* nxt = table[index].get_head()->get_next();

        htEntry* new_head = nullptr;
        size_t max_value = 0;
        htEntry compareItem("", "", nullptr, LONG_MAX); // 大于tag的最大值，保证与任何一个值都不匹配，以此遍历整个环

        // 说实话，这个计算的过程挺麻烦的,整个算法时间复杂度为N2，所以目前我只是返回每个数据项的访问数而已
        auto Calculation = [](htEntry* para) -> size_t {    // 得到每一个数据项的income，原公式在论文的3.2.2
            return para->get_counter();
        };

        if(pre == nxt){ // 仅有一项，直接返回
            return;
        }

        while (true) {  // 循环找到访问最多的那一项，设置为新的head
            size_t curr_value = Calculation(pre);
            pre->reset_counter();   // 上面用完这一项的统计信息以后就没什么用处了
            if(curr_value > max_value){
                new_head = pre;
            }

            if (((*pre) < (&compareItem) && (compareItem) < (nxt)) ||       //ordre_i-1 < order_k < order_i
                ((compareItem) < (nxt) && (*nxt) < (pre))     ||            //order_k < order_i < order_i-1 小于有序环最小值
                ((*nxt) < (pre) && (*pre) < (&compareItem)))                //order_i < order_i-1 < order_k 大于有序环最大值
            {
                break;
            }
            nxt = nxt->get_next();
            pre = pre->get_next();
        }

        table[index].set_head(new_head);
    }

    size_t HotRing::getfindcnt() const {
        return findcnt;
    }
    
    size_t HotRing::getmaxFindcnt() const {
        return maxFindcnt;
    }

    size_t HotRing::getminFindcnt() const {
        return minFindcnt;
    }

    void HotRing::set_min_max(const size_t onecnt){
        this->maxFindcnt = std::max(this->maxFindcnt, onecnt);
        this->minFindcnt = std::min(this->minFindcnt, onecnt);
    }
}