#ifndef HOTRINGENTRY_H_
#define HOTRINGENTRY_H_
#include <string>
namespace HotRingInstance{

using std::string;

class htEntry;

class hrHead{
    public:
        explicit hrHead(htEntry* ptr = nullptr) : head(ptr) {}

        htEntry *get_head() const &;
        void set_head(htEntry *n);

        bool get_active() const &;
        void set_active();
        void reset_active();

        int get_counter() const;    // total counter，代表了整个冲突环的访问次数
        void inc_counter();
        void reset_counter();
    private:    
        static constexpr size_t inc_base = 281474976710656;     // counter 递增的单位 2^48
        htEntry* head;
};

class htEntry{
    public:
        // TODO 这里其实给个默认参数挺没必要的，因为0也是有效值，可能会出现问题，但是暂时先不改
        // 不设置tag的原因是tag不仅与hash_value有关，还与哈希表掩码有关，仅htEntry无法得到
        htEntry(const string& k, const string& v, htEntry* n = nullptr, size_t t = 0) : key(k), val(v), next(n), tag(t){}

        const string& get_key() const &;
        void set_key(const string &s);

        const string& get_val() const &; 
        void set_val(const string &s);

        htEntry *get_next() const &;
        void set_next(htEntry *n);
        
        size_t get_tag() const &;
        void set_tag(const size_t t);
        
        bool get_occupied() const &;
        void set_occupied();
        void reset_occupied();

        bool get_rehash() const &;
        void set_rehash();
        void reset_rehash();

        int get_counter() const;    // 可能会用做比较，所以不加&
        void inc_counter();
        void reset_counter();

        bool operator<(htEntry* other);
    private:
        string key;                 // 键；虽然比较使用tag，但是tag相同就需要key了
        string val;                 // 值
        size_t tag;                 // 使用tag环中进行比较
        static constexpr size_t inc_base = 281474976710656;     // counter 递增的单位 2^48
        htEntry* next;              // rehash,occupied,counter分布在指针的前16比特
};

}

#endif