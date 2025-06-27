#include <mach-o/dyld.h>

#include <map>

#pragma GCC diagnostic ignored "-Wundefined-bool-conversion"

#define ASLR_BIAS _dyld_get_image_vmaddr_slide

/*
template <typename T>
struct monoArrayi
{
    void* klass;
    void* monitor;
    void* bounds;
    int   max_length;
    int32_t capacity;
    void* vector [1];
    T m_Items[0];
    [[maybe_unused]] int getCapacity() {
    return capacity;
    }
    int getLength()
    {
        return max_length;
    }
    T getPointer()
    {
        return m_Items;
    }

};
*/



template<typename T>
struct monoArray {
    void *klass;
    void *monitor;
    void *bounds;
    int32_t capacity;
    T m_Items[0];
    [[maybe_unused]] int32_t getCapacity() { if (!this) return 0; return capacity; }
    T *getPointer() { if (!this) return nullptr; return m_Items; }
    std::vector<T> toCPPlist() {
        if (!this) return {};
        std::vector<T> ret;
        for (int i = 0; i < capacity; i++)
            ret.push_back(m_Items[i]);
        return std::move(ret);
    }
T getPointeri()
    {
        return m_Items;
    }
    bool copyFrom(const std::vector<T> &vec) { if (!this) return false; return copyFrom((T*)vec.data(), (int)vec.size()); }
    [[maybe_unused]] bool copyFrom(T *arr, int size) {
        if (!this) return false;
        if (size < capacity)
            return false;
        memcpy(m_Items, arr, size * sizeof(T));
        return true;
    }
    [[maybe_unused]] void copyTo(T *arr) { if (!this || !CheckObj(m_Items)) return; memcpy(arr, m_Items, sizeof(T) * capacity); }
    T& operator[] (int index) { if (getCapacity() < index) {T a{};return a;} return m_Items[index]; }
    T& at(int index) { if (!this || getCapacity() <= index || empty()) {T a{};return a;} return m_Items[index]; }
    bool empty() { if (!this) return false; return getCapacity() <= 0;}
    static monoArray<T> *Create(int capacity) {
        auto monoArr = (monoArray<T> *)malloc(sizeof(monoArray) + sizeof(T) * capacity);
        monoArr->capacity = capacity;
        return monoArr;
    }
    [[maybe_unused]] static monoArray<T> *Create(const std::vector<T> &vec) { return Create(vec.data(), vec.size()); }
    static monoArray<T> *Create(T *arr, int size) {
        monoArray<T> *monoArr = Create(size);
        monoArr->copyFrom(arr, size);
        return monoArr;
    }
};

typedef struct _monoString
{
    void* klass;
    void* monitor;
    int length;    
    char chars[1];   
    int getLength()
    {
      return length;
    }
    char* getChars()
    {
        return chars;
    }
    NSString* toNSString()
    {
      return [[NSString alloc] initWithBytes:(const void *)(chars)
                     length:(NSUInteger)(length * 2)
                     encoding:(NSStringEncoding)NSUTF16LittleEndianStringEncoding];
    }

    char* toCString()
    {
      NSString* v1 = toNSString();
      return (char*)([v1 UTF8String]);  
    }
    std::string toCPPString()
    {
      return std::string(toCString());
    }
}monoString;

/*static monoString *il2cpp_string_new(const char *str) {
    static monoString *(*String_CreateString)(void *instance, const char *str, int start, int len) = (monoString *(*)(void *, const char *, int, int)) getRealOffset(ENCRYPTOFFSET("0x2766908"));
    return String_CreateString(nullptr, str, 0, strlen(str));
}
*/

template <typename T>
struct monoList {
	void *unk0;
	void *unk1;
	monoArray<T> *items;
	int size;
	int version;
	
	T getItems(){
		return items->getPointer();
	}
	
	int getSize(){
		return size;
	}
	
	int getVersion(){
		return version;
	}
};
/*

template <typename K, typename V>
struct monoDictionary {
	void *unk0;
	void *unk1;
	monoArray<int **> *table;
	monoArray<void **> *linkSlots;
	monoArray<K> *keys;
	monoArray<V> *values;
	int touchedSlots;
	int emptySlot;
	int size;
	
	K getKeys(){
		return keys->getPointer();
	}
	
	V getValues(){
		return values->getPointer();
	}
	
	int getNumKeys(){
		return keys->getLength();
	}
	
	int getNumValues(){
		return values->getLength();
	}
	
	int getSize(){
		return size;
	}
};




template<typename V, typename T>
struct vp_value {
float value;
float newvalue;
void get(){
return value^newvalue;
}
};

    */
    
        template<typename TKey, typename TValue>
        struct [[maybe_unused]] monoDictionary {
            struct Entry {
                [[maybe_unused]] int hashCode, next;
                TKey key;
                TValue value;
            };
            void *klass;
            void *monitor;
            [[maybe_unused]] monoArray<int> *buckets;
            monoArray<Entry> *entries;
            int count;
            int version;
            [[maybe_unused]] int freeList;
            [[maybe_unused]] int freeCount;
            [[maybe_unused]] void *comparer;
            monoArray<TKey> *keys;
            monoArray<TValue> *values;
            [[maybe_unused]] void *syncRoot;
            std::map<TKey, TValue> toMap() {
                std::map<TKey, TValue> ret;
                for (auto it = (Entry*)&entries->m_Items; it != ((Entry*)&entries->m_Items + count); ++it) ret.emplace(std::make_pair(it->key, it->value));
                return std::move(ret);
            }
            std::vector<TKey> getKeys() {
                std::vector<TKey> ret;
                for (int i = 0; i < count; ++i) ret.emplace_back(entries->at(i).key);
                return std::move(ret);
            }
            std::vector<TValue> getValues() {
                std::vector<TValue> ret;
                for (int i = 0; i < count; ++i) ret.emplace_back(entries->at(i).value);
                return std::move(ret);
            }
            int getSize() { return count; }
            [[maybe_unused]] int getVersion() { return version; }
            TValue Get(TKey key) {
                TValue ret;
                if (TryGet(key, ret))
                    return ret;
                return {};
            }
            TValue operator [](TKey key)  { return Get(key); }
        };

template<typename TKey, typename TValue>
    struct monoDictionary2 {
        struct Entry {
            int hashCode, next;
            TKey key;
            TValue value;
        };
        void *klass;
        void *monitor;
        monoArray<int> *buckets;
        monoArray<Entry> *entries;
        int count;
        int version;
        int freeList;
        int freeCount;
        void *comparer;
        monoArray<TKey> *keys;
        monoArray<TValue> *values;
        void *syncRoot;

        std::map<TKey, TValue> toMap() {
            std::map<TKey, TValue> ret;
            auto lst = entries->toCPPlist();
            for (auto enter : lst)
                ret.insert(std::make_pair(enter.key, enter.value));
            return std::move(ret);
        }

        std::vector<TKey> getKeys() {
            std::vector<TKey> ret;
            auto lst = entries->toCPPlist();
            for (auto enter : lst)
                ret.push_back(enter.key);
            return std::move(ret);
        }

        std::vector<TValue> getValues() {
            std::vector<TValue> ret;
            auto lst = entries->toCPPlist();
            for (auto enter : lst)
                ret.push_back(enter.value);
            return std::move(ret);
        }

        int getSize() {
            return count;
        }

        int getVersion() {
            return version;
        }

        bool TryGet(TKey key, TValue &value);
        void Add(TKey key, TValue value);
        void Insert(TKey key, TValue value);
        bool Remove(TKey key);
        bool ContainsKey(TKey key);
        bool ContainsValue(TValue value);

        TValue Get(TKey key) {
            TValue ret;
            if (TryGet(key, ret))
                return ret;
            return {};
        }

        TValue operator [](TKey key) {
            return Get(key);
        }
    };
