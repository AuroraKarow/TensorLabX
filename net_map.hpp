NEUNET_BEGIN

/* map */

template <typename k_arg, typename arg> class net_map {
protected:
    void value_assign(const net_map &src) {
        backup        = src.backup;
        bucket_upper  = src.bucket_upper;
        bucket_lower  = src.bucket_lower;
        rehash_base   = src.rehash_base;
        rehash_load   = src.rehash_hash;
        rehash_idx    = src.rehash_idx;
        bucket_len[0] = bucket_len[0];
        bucket_len[1] = bucket_len[1];
        len[0]        = len[0];
        len[1]        = len[1];
    }

    void value_copy(const net_map &src) {
        value_assign(src);
        kv_data[0] = src.kv_data[0];
        kv_data[1] = src.kv_data[1];
        hash_func  = src.hash_func;
    }

    void value_move(net_map &&src) {
        value_assign(src);
        kv_data[0] = std::move(src.kv_data[0]);
        kv_data[1] = std::move(src.kv_data[1]);
        hash_func  = std::move(src.hash_func);
    }

    void rehash_init(uint64_t alloc_size) {
        if (kv_data[backup].length == alloc_size) return;
        if (len[backup]) {
            rehash_load = len[!backup];
            rehash_transfer();
        }
        kv_data[backup].init(alloc_size);
        rehash_load = rehash_base;
    }

    void rehash_transfer() {
        if (!kv_data[backup].length) return;
        for (auto i = 0ull; i < rehash_load; ++i) {
            if (!len[!backup]) break;
            while (!kv_data[!backup][rehash_idx].length) ++rehash_idx;
            auto curr_key = hash_func(kv_data[!backup][rehash_idx].root_key());
            auto curr_kv  = kv_data[!backup][rehash_idx].erase(curr_key);
            --len[!backup];
            if (!kv_data[!backup][rehash_idx].length) {
                --bucket_len[!backup];
                ++rehash_idx;
            }
            auto curr_idx = curr_key % kv_data[backup].length;
            if (!kv_data[backup][curr_idx].length) ++bucket_len[backup];
            kv_data[backup][curr_idx].insert(curr_key, std::move(curr_kv.key), std::move(curr_kv.value));
            ++len[backup];
        }
        if (len[!backup]) rehash_load <<= 1;
        else {
            backup      = !backup;
            rehash_load = rehash_base;
            rehash_idx  = 0;
            kv_data[backup].reset();
        }
    }

public:
    net_map(uint64_t alloc_size = 128, const std::function<uint64_t(const k_arg&)> &hash_key_func = [](const k_arg &key) { return hash_in_built(key); }, uint64_t rehash_load_base = 4, long double upper_factor = 0.75, long double lower_factor = 0.1) :
        hash_func(hash_key_func),
        rehash_base(rehash_load_base),
        rehash_load(rehash_load_base),
        bucket_upper(upper_factor),
        bucket_lower(lower_factor) { kv_data[!backup].init(alloc_size); }
    net_map(const net_set<net_kv<k_arg, arg>> &init_list, uint64_t alloc_size = 128, const std::function<uint64_t(const k_arg&)> &hash_key_func = [](const k_arg &key) { return hash_in_built(key); }, uint64_t rehash_load_base = 4, long double upper_factor = 0.75, long double lower_factor = 0.1) : net_map(alloc_size, hash_key_func, rehash_load_base, upper_factor, lower_factor) { insert(init_list); }
    net_map(const net_map &src) { value_copy(src); }
    net_map(net_map &&src) { value_move(std::move(src)); }

    uint64_t size() const { return len[0] + len[1]; }

    uint64_t bucket_size() const { return bucket_len[backup] + bucket_len[!backup]; }

    uint64_t mem_size() const { return kv_data[backup].length ? kv_data[backup].length : kv_data[!backup].length; }

    net_set<k_arg> key_set() const {
        net_set<k_arg> ans(size());
        auto cnt = 0ull;
        for (auto i = 0; i < 2; ++i) if (len[i]) for (auto temp : kv_data[i]) if (temp.length) {
            auto curr_key = temp.key_set();
            for (auto k_temp : curr_key) ans[cnt++] = std::move(k_temp);
        }
        return ans;
    }

    net_set<k_arg> find_key(const arg &value) const {
        auto all_key = key_set();
        net_set<k_arg> ans;
        if (all_key.length == 0) return ans;
        net_ptr_base<k_arg> ans_ptr;
        ans_ptr.init(all_key.length);
        auto cnt = 0ull;
        for (auto temp : all_key) if (*this[temp] == value) *(ans_ptr.ptr_base + cnt++) = std::move(temp);
        if (cnt != ans_ptr.len) ptr_alter(ans_ptr.ptr_base, ans_ptr.len, cnt);
        ans_ptr.len = cnt;
        ans.pointer = std::move(ans_ptr);
        return ans;
    }

    uint64_t hash_key_verify(uint64_t hash_key) const {
        auto idx = hash_key % kv_data[!backup].length;
        if (kv_data[!backup][idx].hash_key_verify(hash_key)) return NEUNET_HASH_EXIST_CURR;
        if (len[backup]) {
            idx = hash_key % kv_data[backup].length;
            if (kv_data[backup][idx].hash_key_verify(hash_key)) return NEUNET_HASH_EXIST_BACK;
        }
        return NEUNET_HASH_NOT_FOUND;
    }

    uint64_t insert(net_set<net_kv<k_arg, arg>> &&elem_list) {
        auto src_len = elem_list.length;
        // memory check
        auto lexicon  = !backup;
        auto curr_len = size() + src_len,
             alloc    = 0ull;
        if (len[backup]) {
            alloc   = kv_data[backup].length;
            lexicon = backup;
        } else alloc = kv_data[!backup].length;
        if (curr_len >= alloc * bucket_upper) {
            do alloc <<= 1;
            while (alloc * bucket_upper <= curr_len);
            rehash_init(alloc);
            lexicon = backup;
        }
        // insert
        uint64_t cnt = 0;
        for (auto i = 0ull; i < elem_list.length; ++i) {
            auto curr_hash_key = hash_func(elem_list[i].key),
                 curr_idx      = curr_hash_key % kv_data[lexicon].length;
            if (kv_data[lexicon][curr_idx].insert(curr_hash_key, std::move(elem_list[i].key), std::move(elem_list[i].value))) {
                ++cnt;
                if (kv_data[lexicon][curr_idx].length == 1) ++bucket_len[lexicon];
            }
        }
        len[lexicon] += cnt;
        rehash_transfer();
        elem_list.reset();
        if (cnt == src_len) return NEUNET_INSERT_SUCCESS;
        else if (cnt == 0) return NEUNET_INSERT_FAILED;
        else return NEUNET_INSERT_PARTIAL;
    }
    uint64_t insert(const std::initializer_list<net_kv<k_arg, arg>> &init_list) {
        net_set<net_kv<k_arg, arg>> temp(init_list.size());
        auto cnt = 0ull;
        for (auto kv_temp : init_list) temp[cnt++] = std::move(kv_temp);
        return insert(std::move(temp));
    }
    uint64_t insert(const k_arg &key, const arg &value) { return insert({net_kv<k_arg, arg>(key, value)}); }

    net_set<net_kv<k_arg, arg>> erase(const net_set<k_arg> &key_list) {
        // erase
        net_set<net_kv<k_arg, arg>> ans;
        if (key_list.size() == 0) return ans;
        net_ptr_base<net_kv<k_arg, arg>> ans_ptr;
        ans_ptr.init(key_list.size());
        auto cnt = 0ull;
        for (auto k_temp : key_list) {
            auto curr_hash_key = hash_func(k_temp),
                 find_key_res  = hash_key_verify(curr_hash_key);
            if (find_key_res == NEUNET_HASH_NOT_FOUND) continue;
            auto lexicon = backup;
            if (find_key_res == NEUNET_HASH_EXIST_CURR) lexicon = !lexicon;
            auto curr_idx  = curr_hash_key % kv_data[lexicon].length;
            auto curr_temp = kv_data[lexicon][curr_idx].erase(curr_hash_key);
            if (curr_temp.valid) {
                *(ans_ptr.ptr_base + cnt++) = std::move(curr_temp);
                if (!kv_data[lexicon][curr_idx].length) --bucket_len[lexicon];
                --len[lexicon];
            }
        }
        if (cnt != ans_ptr.len) ptr_alter(ans_ptr.ptr_base, ans_ptr.len, cnt);
        ans.pointer = std::move(ans_ptr);
        // memory check
        auto lower = size(),
             alloc = kv_data[!backup].length;
        if (len[backup]) alloc = kv_data[backup].length;
        if (lower < alloc * bucket_lower) {
            auto alloc_temp = alloc >> 1;
            while (alloc_temp * bucket_lower > lower) {
                alloc      = alloc_temp;
                alloc_temp = alloc >> 1;
            }
            rehash_init(alloc);
        }
        rehash_transfer();
        return ans;
    }
    net_kv<k_arg, arg> erase(const k_arg &key) {
        net_kv<k_arg, arg> ans;
        auto ans_set = erase(net_set<k_arg>({key}));
        if (ans_set.length) ans = std::move(ans_set[0]);
        return ans;
    }

    arg &operator[](const k_arg &key) {
        rehash_transfer();
        auto curr_hash_key = hash_func(key),
             find_key_res  = hash_key_verify(curr_hash_key);
        assert(find_key_res != NEUNET_HASH_NOT_FOUND);
        if (find_key_res == NEUNET_HASH_EXIST_CURR) {
            auto idx = curr_hash_key % kv_data[!backup].length;
            return kv_data[!backup][idx].get_value(curr_hash_key);
        } else {
            auto idx = curr_hash_key % kv_data[backup].length;
            return kv_data[backup][idx].get_value(curr_hash_key);
        }
    }

    net_map &operator=(const net_map &src) {
        value_copy(src);
        return *this;
    }
    net_map &operator=(net_map &&src) {
        value_move(std::move(src));
        return *this;
    }

    bool operator==(const net_map &src) const {
        if (size() != src.size()) return false;
        auto curr_key_set = key_set();
        for (auto temp : curr_key_set) if (src[temp] != *this[temp]) return false;
        return true;
    }

    bool operator!=(const net_map &src) const { return !(*this == src); }

    void reset() {
        backup        = false;
        bucket_upper  = 0.75;
        bucket_lower  = 0.1;
        rehash_base   = 4;
        rehash_load   = rehash_base;
        rehash_idx    = 0;
        bucket_len[0] = 0;
        bucket_len[1] = 0;
        len[0]        = 0;
        len[1]        = 0;

        kv_data[0].reset();
        kv_data[1].reset();
    }
    
    ~net_map() { reset(); }

protected:
    net_set<net_tree<k_arg, arg>> kv_data[2];
    
    // hash function
    std::function<uint64_t(const k_arg &)> hash_func;

    bool backup = true;

    long double bucket_upper = 0.75,
                bucket_lower = 0.1;
    
    uint64_t rehash_base = 4,
             rehash_load = rehash_base,
             rehash_idx  = 0,
             bucket_len[2] {0, 0},
             len[2]        {0, 0};

public:
    static bool print_orderly;

    __declspec(property(get=size))        uint64_t length;
    __declspec(property(get=bucket_size)) uint64_t bucket_length;
    __declspec(property(get=mem_size))    uint64_t memory_length;

    friend std::ostream &operator<<(std::ostream &out, const net_map &src) {
        
        for (auto i = 0; i < 2; ++i) for (auto tr_temp : src.kv_data[i]) for (auto kv_temp : tr_temp) {
            out << "[Key][\n";
            out << kv_temp.key << "\n]";
            out << "[Value][\n";
            out << kv_temp.value << "\n]\n";
        }
        return out;
    }

};
template <typename k_arg, typename arg> bool net_map<k_arg, arg>::print_orderly = false;

NEUNET_END