#include "htentry.hpp"

namespace HotRingInstance{
        size_t htEntry::get_hash_value() const &{
            return hash_value;
        }

        string htEntry::get_key() const &{
            return key;
        }
        void htEntry::set_key(const string &s){
            key = s;
        }

        string htEntry::get_val() const &{
            return val;
        } 
        void htEntry::set_val(const string &s){
            val = s;
        }

        htEntry* htEntry::get_next() const &{
            return next;
        }

        void htEntry::set_next(htEntry *n){
            next = n;
        }
        
        size_t htEntry::get_tag() const &{
            return tag;
        }
        void htEntry::set_tag(const size_t t){
            tag = t;
        }
        
        bool htEntry::get_occupied() const &{
            size_t ptr = reinterpret_cast<size_t>(next);
            return ptr & (1l << 62l);
        }

        void htEntry::set_occupied(){
            size_t ptr = reinterpret_cast<size_t>(next);
            ptr |= (1l << 62l); // 某一位置为1
            next = reinterpret_cast<htEntry*>(ptr);
        }

        void htEntry::reset_occupied(){
            size_t ptr = reinterpret_cast<size_t>(next);
            ptr &= ~(1l << 62l);// 某一位置为0
            next = reinterpret_cast<htEntry*>(ptr);
        }

        bool htEntry::get_rehash() const &{
            size_t ptr = reinterpret_cast<size_t>(next);
            return ptr & (1l << 63l);
        }

        void htEntry::set_rehash(){
            size_t ptr = reinterpret_cast<size_t>(next);
            ptr |= (1l << 63l);
            next = reinterpret_cast<htEntry*>(ptr);
        }

        void htEntry::reset_rehash(){
            size_t ptr = reinterpret_cast<size_t>(next);
            ptr &= ~(1l << 63l);
            next = reinterpret_cast<htEntry*>(ptr);
        }

        int htEntry::get_counter() const{
            size_t ptr = reinterpret_cast<size_t>(next);
            ptr &= ~(1l << 63l);
            ptr &= ~(1l << 62l);
            return ptr>>48;
        }

        void htEntry::inc_counter(){
            size_t ptr = reinterpret_cast<size_t>(next);
            bool occupied = ptr & (1l << 62l);  // 保存这两个标记是担心进位影响标记位
            bool rehash = ptr & (1l << 63l);
            ptr += inc_base;
            // 先置空，不然下面的位运算会出现问题
            ptr &= ~(1l << 62l);
            ptr &= ~(1l << 63l);

            if(occupied){
                ptr |= (1l << 62l);
            }
            if(rehash){
                ptr |= (1l << 63l);
            }

            next = reinterpret_cast<htEntry*>(ptr);
        }

        void htEntry::reset_counter(){
            size_t ptr = reinterpret_cast<size_t>(next);
            bool occupied = ptr & (1l << 62l);
            bool rehash = ptr & (1l << 63l);
            // 这两部是清空counter位            ptr <<= 16;
            ptr >>= 16;
            if(occupied){
                ptr |= (1l << 62l);
            }
            if(rehash){
                ptr |= (1l << 63l);
            }
            next = reinterpret_cast<htEntry*>(ptr);
        }

        bool htEntry::operator<(htEntry* other){
            if(!other) return false;
            if (this->tag == other->get_tag()) {
                return this->key < other->get_key();
            }
            return this->tag < other->get_tag();
        }
}