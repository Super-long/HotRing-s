#include <string>
#include <limits.h>
#include <algorithm>
#include <iostream>
#include "hotring.hpp"

#include <unistd.h>

namespace HotRingInstance{
    HotRing::HotRing(size_t sz):table(0), findcnt(0), minFindcnt(LONG_MAX), maxFindcnt(0) {
        size_t htsz = 1;
        while (htsz < sz) htsz <<= 1;   // 把容量扩充为参数的整数次幂
        table.resize(htsz);
        hash_mask = htsz - 1;
    } 

    bool HotRing::insert(const string &key, const string &val){
        size_t hashValue = hash_fn(key);
        size_t index = hashValue & hash_mask;
        size_t tag = hashValue & (~hash_mask);

        // 这里可以进行优化，每次插入都需要内存分配，slab是个不错的选择
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
        htEntry *res = search(key);
        unsigned int hashValue = hash_fn(key);
        unsigned int index = hashValue & hash_mask;
        // 环中没有要删除的数据，当然包括环为空的情况
        if (res == nullptr) return false;

/*      // 这段代码错哪了？
        htEntry* pre = table[index].get_head();
        htEntry* curr = pre->get_next();
        if(pre == curr){ // 冲突环只有一个结点
            table[index].set_head(nullptr);
            delete pre;
            return true;
        }
        
        while (curr->get_val() != res->get_val()) {
            pre = pre->get_next();
            curr = curr->get_next();
            std::cout << curr->get_val() << " " << res->get_val() << std::endl;
        }
        std::cout << "find it! " << key << std::endl;
        pre->set_next(curr->get_next());

        delete curr; */

        htEntry *pre = res;
        // 密集写时会很慢，所以我们需要在热点转移那里把head指向设置为热点的前一个值，这样删除会很快
        while (pre->get_next()->get_key() != res->get_key()) {
            pre = pre->get_next();
        }
        pre->set_next(res->get_next());

        if (table[index].get_head() == res){
            if (pre == res) {   // 只有一项
                table[index].set_head(nullptr);
            } else {            // 如果删除的结点是热点，head就指向删除的结点之后
                table[index].set_head(res->get_next());
            }
        }

        delete res; // TODO 后面改为智能指针
        return true;
    }

    // 在paper3.2.3中提到如果删除的项是头指针指向的热点数据的话，我们需要把新插入的数据项作为热点
    // TODO papar中的update貌似指的是把结点本身删除，替换为另外一个结点，没搞清楚paper中为什么要那样。
    bool HotRing::update(const string &key, const string &val){
        htEntry *res = search(key);
        if (res == nullptr) return false;

        size_t hashValue = hash_fn(key);
        size_t index = hashValue & hash_mask;
        res->set_val(val);
        table[index].set_head(res);

        return true;
    }

    htEntry* HotRing::search(const string &key){
        size_t hashValue = hash_fn(key);
        size_t index = hashValue & hash_mask;
        size_t tag = (hashValue & (~hash_mask)) >> __builtin_popcountl(hash_mask);

        htEntry* pre = nullptr;
        htEntry* nxt = nullptr;
        htEntry* res = nullptr;

        size_t precnt = findcnt;
        htEntry compareItem(key, "", nullptr, tag);
        bool hotspotAware = false;

        using namespace std;
        //std::cout << key << " : " << index << std::endl; 

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
            //cout << "第三个if\n";
            pre = table[index].get_head();

            if(table[index].get_active()){ // 3.2.2，active标志存在才进行采样
                pre->inc_counter(); 
            }
            nxt = table[index].get_head()->get_next();
            while (true) {
                if(compareItem.get_tag() == pre->get_tag() && key == pre->get_key()){
                    res = pre;
                    if(r >= R && pre != table[index].get_head()){   // 第R次访问不是热结点，开启采样;当热点调整的时候重置
                        if(table[index].get_active()){  // 保证在设置后的第R次进行热点调整
                            hotspot_movement(index);    // 其中清空了每个数据项的统计信息
                            table[index].reset_active();
                            table[index].reset_counter();
                        } else {
                            table[index].set_active();
                        }
                        r = 0;
                    }
                    break;
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
        ++findcnt;  // 在正常的循环的地方递增，也就是向上数两行的作用;这一行的作用是查找成功和查找失败的那一个加1，放上面就得写好多个加1

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

        // TODO 这里可以把head指向热点的前一项，这样的话删除会很快，不然需要绕一圈去找前驱指针
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

    void HotRing::clear_metadata(){
        minFindcnt = LONG_MAX;
        maxFindcnt = 0;
        findcnt = 0;
    }

    void HotRing::set_min_max(const size_t onecnt){
        this->maxFindcnt = std::max(this->maxFindcnt, onecnt);
        this->minFindcnt = std::min(this->minFindcnt, onecnt);
    }
}