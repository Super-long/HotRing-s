#ifndef HOTRINGENTRY_H_
#define HOTRINGENTRY_H_
#include <string>
namespace HotRingInstance{

using std::string;

class hrHead{
 
};

class htEntry{
    public:
        // TODO 这里其实给个默认参数挺没必要的，因为0也是有效值，可能会出现问题，但是暂时先不改
        htEntry(const string& k, const string& v, htEntry* n = nullptr, size_t t = 0) : key(k), val(v), next(n), tag(t), hash_value(hash_fn(k)){}

        size_t get_hash_value() const &;

        string get_key() const &;
        void set_key(const string &s);

        string get_val() const &; 
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
        inline static std::hash<std::string> hash_fn = std::hash<std::string>();  // 后期可替换哈希函数
        string key;                 // 键；虽然比较使用tag，但是tag相同就需要key了
        string val;                 // 值
        size_t tag;                 // 使用tag环中进行比较
        const size_t hash_value;
        static constexpr size_t address_mask = 281474976710655; // 2^48 - 1
        static constexpr size_t inc_base = 281474976710656;     // counter 递增的单位 2^48
        htEntry* next;              // rehash,occupied,counter分布在指针的前16比特
};





}

#endif