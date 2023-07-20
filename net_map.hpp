NEUNET_BEGIN

/* map */

template <typename k_arg, typename arg> class net_map {
public:
    struct iterator final : net_iterator_base<net_kv<k_arg, arg>, net_map<k_arg, arg>> {
    public:
        iterator(const net_map *src = nullptr) : net_iterator_base<net_kv<k_arg, arg>, net_map<k_arg, arg>>(src) {
            if (this->ptr) {
                for (curr_idx = 0ull; curr_idx < this->ptr->kv_data[!this->ptr->backup].length; ++curr_idx) if (this->ptr->kv_data[!this->ptr->backup][curr_idx].length) break;
                itr_leaf = this->ptr->kv_data[!this->ptr->backup][curr_idx].begin();
            }
        }

        virtual bool operator!=(const iterator &val) const { return !(*this == val); }

        virtual net_kv<k_arg, arg> operator*() const {
            if (this->ptr) return net_kv(*itr_leaf);
            else return net_kv<k_arg, arg>();
        }

        virtual iterator &operator++() {
            if (this->ptr) {
                ++itr_leaf;
                if (itr_leaf.is_end()) {
                    ++curr_idx;
                    while (curr_idx != this->ptr->kv_data[!this->ptr->backup].length && !this->ptr->kv_data[!this->ptr->backup][curr_idx].length) ++curr_idx;
                }
                if (curr_idx == this->ptr->kv_data[!this->ptr->backup].length) this->ptr = nullptr;
                else itr_leaf = this->ptr->kv_data[!this->ptr->backup][curr_idx].begin();
            }
            return *this;
        }
        virtual iterator operator++(int) {
            auto temp = *this;
            ++*this;
            return temp;
        }

        virtual iterator &operator--() {
            if (this->ptr) {
                --itr_leaf;
                auto flag = true;
                if (itr_leaf.is_end()) {
                    if (flag) while (!this->ptr->kv_data[!this->ptr->backup][curr_idx].length) if (curr_idx) --curr_idx;
                    else {
                        flag      = false;
                        this->ptr = nullptr;
                        break;
                    }
                }
                if (flag) {
                    itr_leaf = this->ptr->kv_data[!this->ptr->backup][curr_idx].begin();
                    auto curr_iter_leaf = itr_leaf;
                    ++curr_iter_leaf;
                    while (!curr_iter_leaf.is_end()) {
                        itr_leaf = curr_iter_leaf;
                        ++curr_iter_leaf;
                    }
                } else this->ptr = nullptr;
            }
            return *this;
        }
        virtual iterator operator--(int) {
            auto temp = *this;
            --*this;
            return temp;
        }

        virtual ~iterator() { curr_idx = 0; }

    private:
        net_tree<k_arg, arg>::iterator itr_leaf;

        uint64_t curr_idx = 0;
    };
protected:
    void value_assign(const net_map &src) {
        backup       = src.backup;
        bucket_upper = src.bucket_upper;
        bucket_lower = src.bucket_lower;
        rehash_base  = src.rehash_base;
        rehash_load  = src.rehash_hash;
        rehash_idx   = src.rehash_idx;
        rehash_leaf  = src.rehash_leaf;
        p_hash_fn    = src.p_hash_fn;
        len[0]       = src.len[0];
        len[1]       = src.len[1];
    }

    void value_copy(const net_map &src) {
        value_assign(src);
        kv_data[0] = src.kv_data[0];
        kv_data[1] = src.kv_data[1];
    }

    void value_move(net_map &&src) {
        value_assign(src);
        kv_data[0] = std::move(src.kv_data[0]);
        kv_data[1] = std::move(src.kv_data[1]);
        src.reset();
    }

    uint64_t hash_key_verify(uint64_t &idx, uint64_t hash_key) const {
        idx = hash_key % kv_data[!backup].length;
        if (kv_data[!backup][idx].hash_key_verify(hash_key)) return NEUNET_HASH_EXIST_CURR;
        if (len[backup]) {
            idx = hash_key % kv_data[backup].length;
            if (kv_data[backup][idx].hash_key_verify(hash_key)) return NEUNET_HASH_EXIST_BACK;
        }
        return NEUNET_HASH_NOT_FOUND;
    }

    void rehash_init(uint64_t alloc_size) {
        if (kv_data[backup].length == alloc_size) return;
        if (len[backup]) {
            rehash_load = len[!backup];
            rehash_transfer();
        }
        kv_data[backup].init(alloc_size);
        rehash_load = rehash_base;
        rehash_idx  = 0;
        rehash_leaf = kv_data[!backup][rehash_idx].begin();
    }

    void rehash_transfer() {
        if (!kv_data[backup].length) return;
        for (auto i = 0ull; i < rehash_load; ++i) {
            if (!len[!backup]) {
                backup = !backup;
                kv_data[backup].reset();
                break;
            }
            if (rehash_leaf == kv_data[!backup][rehash_idx].end()) {
                kv_data[!backup][rehash_idx].reset();
                while (!kv_data[!backup][rehash_idx].length) ++rehash_idx;
                rehash_leaf = kv_data[!backup][rehash_idx].begin();
            }
            auto curr_hash = rehash_leaf.hash_key;
            auto curr_idx  = curr_hash % kv_data[backup].length;
            kv_data[backup][curr_idx].insert(curr_hash, (*rehash_leaf));
            rehash_leaf.change_valid(false);
            --len[!backup];
            ++len[backup];
            ++rehash_leaf;
        }
        if (len[backup]) rehash_load <<= 1;
    }

public:
    net_map(uint64_t alloc_size = 128, hash_fn_t<k_arg> hash_func = hash_in_built, uint64_t rehash_load_base = 4, long double upper_factor = 0.75, long double lower_factor = 0.1) :
        p_hash_fn(hash_func),
        rehash_base(rehash_load_base),
        rehash_load(rehash_load_base),
        bucket_upper(upper_factor),
        bucket_lower(lower_factor) { kv_data[!backup].init(alloc_size); }
    net_map(init_list_kv_t<k_arg, arg> init_list, uint64_t alloc_size = 128, hash_fn_t<k_arg> hash_func = hash_in_built, uint64_t rehash_load_base = 4, long double upper_factor = 0.75, long double lower_factor = 0.1) : net_map(alloc_size, hash_func, rehash_load_base, upper_factor, lower_factor) { insert(init_list); }
    net_map(const net_map &src) { value_copy(src); }
    net_map(net_map &&src) { value_move(std::move(src)); }

    uint64_t size() const { return len[0] + len[1]; }

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
        ans.init(all_key.length);
        auto cnt = 0ull;
        for (auto temp : all_key) if (*this[temp] == value) ans[cnt++] = std::move(temp);
        if (cnt != ans.length) ans.init(cnt);
        return ans;
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
            auto curr_hash_key = p_hash_fn(elem_list[i].key),
                 curr_idx      = curr_hash_key % kv_data[lexicon].length;
            if (kv_data[lexicon][curr_idx].insert(curr_hash_key, std::move(elem_list[i]))) ++cnt;
        }
        len[lexicon] += cnt;
        rehash_transfer();
        elem_list.reset();
        if (cnt == src_len) return NEUNET_INSERT_SUCCESS;
        else if (cnt == 0) return NEUNET_INSERT_FAILED;
        else return NEUNET_INSERT_PARTIAL;
    }
    uint64_t insert(init_list_kv_t<k_arg, arg> init_list) { return insert(net_set<net_kv<k_arg, arg>>(init_list)); }
    bool insert(const k_arg &key, const arg &value) { return insert({{key, value}}) == NEUNET_INSERT_SUCCESS; }

    net_set<net_kv<k_arg, arg>> erase(const net_set<k_arg> &key_list) {
        net_set<net_kv<k_arg, arg>> ans;
        if (!key_list.length) return ans;
        ans.init(key_list.length);
        auto cnt = 0ull;
        // erase
        for (auto temp : key_list) {
            auto curr_hash = p_hash_fn(temp);
            if (!rehash_leaf.is_end() && rehash_leaf.hash_key == curr_hash) {
                ans[cnt++] = *rehash_leaf;
                ++rehash_leaf;
                --len[!backup];
            }
            else {
                auto curr_idx  = 0ull,
                     find_res  = hash_key_verify(curr_idx, curr_hash);
                if (find_res == NEUNET_HASH_NOT_FOUND) continue;
                auto lexicon = backup;
                if (find_res == NEUNET_HASH_EXIST_CURR) lexicon = !backup;
                ans[cnt++] = kv_data[lexicon][curr_idx].erase(curr_hash);
                --len[lexicon];
            }
        }
        if (ans.length != cnt) ans.init(cnt);
        // memory check
        uint64_t after_len = size(),
                 curr_mem  = (kv_data[backup].length ? kv_data[backup].length : kv_data[!backup].length),
                 next_mem  = curr_mem * bucket_lower;
        if (next_mem > after_len && after_len) {
            do {
                curr_mem >>= 1;
                next_mem  = curr_mem * bucket_lower;
            } while (next_mem > after_len);
            rehash_init(curr_mem);
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

    iterator begin() const {
        if (len[0] + len[1]) return iterator(this);
        else return end();
    }

    iterator end() const { return iterator(nullptr); }

    arg &operator[](const k_arg &key) {
        rehash_transfer();
        auto curr_hash = p_hash_fn(key),
             curr_idx  = 0ull,
             find_res  = hash_key_verify(curr_idx, curr_hash);
        if (find_res == NEUNET_HASH_NOT_FOUND) return neunet_null_ref(arg);
        if (find_res == NEUNET_HASH_EXIST_BACK) return kv_data[backup][curr_idx].get_value(curr_hash);
        else return kv_data[!backup][curr_idx].get_value(curr_hash);
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
        len[0]        = 0;
        len[1]        = 0;
        p_hash_fn     = nullptr;

        kv_data[0].reset();
        kv_data[1].reset();
    }
    
    ~net_map() { reset(); }

protected:
    net_set<net_tree<k_arg, arg>> kv_data[2];
    
    // hash function
    hash_fn_t<k_arg> p_hash_fn = nullptr;

    bool backup = true;

    long double bucket_upper = 0.75,
                bucket_lower = 0.1;
    
    uint64_t rehash_base = 4,
             rehash_load = rehash_base,
             rehash_idx  = 0,
             len[2]        {0, 0};

    net_tree<k_arg, arg>::iterator rehash_leaf;

public:
    bool print_orderly = false;

    __declspec(property(get=size))        uint64_t length;
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

NEUNET_END