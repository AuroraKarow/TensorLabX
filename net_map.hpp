NEUNET_BEGIN

/* key -> value */
template <typename k_arg, typename arg> struct net_kv {
    k_arg key{};
    arg   value{};

    net_kv() {}
    net_kv(const k_arg &_key, const arg &_value) :
        key(_key),
        value(_value) {}
    net_kv(const net_kv &src) :
        key(src.key),
        value(src.value) {}
    net_kv(net_kv &&src) :
        key(std::move(src.key)),
        value(std::move(src.value)) {}
    
    bool operator==(const net_kv &val) { return key == val.key && value == val.value; }
    
    bool operator!=(const net_kv &val) { return !(*this == val); }

    net_kv &operator=(const net_kv &src) {
        key   = src.key;
        value = src.value;
        return *this;
    }
    net_kv &operator=(net_kv &&src) {
        key   = std::move(src.key);
        value = std::move(src.value);
        return *this;
    }
};

/* map */

template <typename k_arg, typename arg> class net_map {
protected:
    struct iterator final : net_iterator_base<net_kv<k_arg, arg>, net_map<k_arg, arg>> {
    public:
        iterator(net_set<k_arg> &&iter_key_set, const net_map<k_arg, arg> *src = nullptr, uint64_t idx = 0) : net_iterator_base<net_kv<k_arg, arg>, net_map<k_arg, arg>>(src),
            iter_key(std::move(iter_key_set)),
            curr_idx(idx) { if(this->ptr && this->ptr->output_ordered) iter_key.sort(); }

        virtual bool operator==(const iterator &val) const { return curr_idx == val.curr_idx && net_iterator_base<net_kv<k_arg, arg>, net_map<k_arg, arg>>::operator==(val); }

        virtual bool operator!=(const iterator &val) const { return !(*this == val); }

        virtual net_kv<k_arg, arg> operator*() const {
            if (this->ptr) {
                auto k_temp = iter_key[curr_idx];
                auto v_temp = this->ptr->operator[](k_temp);
                return net_kv(k_temp, v_temp);
            } else return net_kv<k_arg, arg>();
        }

        virtual iterator &operator++() {
            if (this->ptr) {
                ++curr_idx;
                if (curr_idx == iter_key.length) {
                    this->ptr = nullptr;
                    curr_idx  = 0;
                    iter_key.reset();
                }
            }
            return *this;
        }
    
        virtual iterator operator++(int) { auto temp = *this; ++*this; return temp; }

        virtual iterator &operator--() {
            if (curr_idx) --curr_idx;
            else {
                this->ptr = nullptr;
                iter_key.reset();
            }
            return *this;
        }
    
        virtual iterator operator--(int) { auto temp = *this; --*this; return temp; }

    private:
        uint64_t curr_idx = 0;

        net_set<k_arg> iter_key;
    };

    uint64_t find_idx(const k_arg &key, bool lexicon, bool occ = true) const {
        long long threshold = 0,
                  idx       = hash_func(std::move(key));
        auto      sgn       = false;
        do idx = hash_next_key(idx, hash_detect(threshold, sgn), kv_data[lexicon].length);
        while (idx < 0 || (((occ && kv_occ[lexicon][idx]) || (!occ && kv_def[lexicon][idx])) && kv_data[lexicon][idx].key != key));
        return idx;
    }

    void rehash_transfer() {
        if (kv_data[backup].length == 0) return;
        uint64_t trans_cnt = rehash_load,
                 cnt       = 0;
        while (cnt != trans_cnt && len[!backup]) {
            if (kv_occ[!backup][rehash_idx]) {
                auto tgt_idx = find_idx(kv_data[!backup][rehash_idx].key, backup);
                kv_data[backup][tgt_idx]    = kv_data[!backup][rehash_idx];
                kv_occ[!backup][rehash_idx] = false;
                kv_occ[backup][tgt_idx]     = true;
                kv_def[backup][tgt_idx]     = true;
                ++len[backup];
                --len[!backup];
                ++cnt;
            }
            ++rehash_idx;
        }
        if (len[!backup]) rehash_load <<= 1;
        else {
            backup = !backup;
            kv_data[backup].reset();
            kv_occ[backup].reset();
            kv_def[backup].reset();
            rehash_load = rehash_load_base;
            rehash_idx  = 0;
        }
    }

    void rehash_backup_init(uint64_t alloc_size) {
        if (kv_data[backup].length == alloc_size) return;
        if (len[!backup]) {
            rehash_load = len[!backup];
            rehash_transfer();
        }
        kv_data[backup].init(alloc_size);
        kv_occ[backup].init(alloc_size);
        kv_def[backup].init(alloc_size);
        rehash_load = rehash_load_base;
    }

    void value_assign(const net_map &src) {
        backup            = src.backup;
        hash_upper_factor = src.hash_upper_factor;
        hash_lower_factor = src.hash_lower_factor;
        rehash_load_base  = src.rehash_load_base;
        rehash_load       = src.rehash_load;
        rehash_idx        = src.rehash_idx;
        len[0]            = src.len[0];
        len[1]            = src.len[1];
        hash_func         = src.hash_func;
    }
    
    void value_copy(const net_map &src) {
        value_assign(src);
        kv_data[0] = src.kv_data[0];
        kv_data[1] = src.kv_data[1];
        kv_occ[0]  = src.kv_occ[0];
        kv_occ[1]  = src.kv_occ[1];
        kv_def[0]  = src.kv_def[0];
        kv_def[1]  = src.kv_def[1];
    }

    void value_move(net_map &&src) {
        value_assign(src);
        kv_data[0] = std::move(src.kv_data[0]);
        kv_data[1] = std::move(src.kv_data[1]);
        kv_occ[0]  = std::move(src.kv_occ[0]);
        kv_occ[1]  = std::move(src.kv_occ[1]);
        kv_def[0]  = std::move(src.kv_def[0]);
        kv_def[1]  = std::move(src.kv_def[1]);
        src.reset();
    }

public:
    net_map(uint64_t alloc_size = 128, const std::function<uint64_t(const k_arg&)> &hash_key_func = [](const k_arg &key) { return hash_in_built(key); }, uint64_t rehash_load_val = 4, long double hash_upper = 0.75, long double hash_lower = 0.1) :
        hash_func(hash_key_func),
        rehash_load_base(rehash_load_val),
        hash_upper_factor(hash_upper),
        hash_lower_factor(hash_lower) {
            kv_data[!backup].init(alloc_size);
            kv_occ[!backup].init(alloc_size);
            kv_def[!backup].init(alloc_size);
        }
    net_map(const net_set<net_kv<k_arg, arg>> &init_list, uint64_t alloc_size = 128, const std::function<uint64_t(const k_arg&)> &hash_key_func = [](const k_arg &key) { return hash_in_built(key); }, uint64_t rehash_load_val = 4, long double hash_upper = 0.75, long double hash_lower = 0.1) :
        hash_func(hash_key_func),
        rehash_load_base(rehash_load_val),
        hash_upper_factor(hash_upper),
        hash_lower_factor(hash_lower) {
            kv_data[!backup].init(alloc_size);
            kv_occ[!backup].init(alloc_size);
            kv_def[!backup].init(alloc_size);
            insert(init_list);
        }
    net_map(const net_map &src) { value_copy(src); }
    net_map(net_map &&src) { value_move(std::move(src)); }

    uint64_t size() const { return len[0] + len[1]; }

    uint64_t mem_size() const {
        if (len[backup]) return kv_data[backup].length;
        else return kv_data[!backup].length;
    }

    net_set<k_arg> key_set() const {
        net_set<k_arg> ans_backup(len[backup]),
                       ans_curr(len[!backup]);
        if (len[!backup] == 0) return ans_curr;
        auto cnt = 0ull;
        if (ans_backup.length) {
            for (auto i = 0ull; i < kv_occ[backup].length; ++i) if (kv_occ[backup][i]) {
                ans_backup[cnt++] = kv_data[backup][i].key;
                if (cnt == ans_backup.length) break;
            }
            cnt = 0;
        }
        for (auto i = 0ull; i < kv_occ[!backup].length; ++i) if (kv_occ[!backup][i]) {
            ans_curr[cnt++] = kv_data[!backup][i].key;
            if (cnt == ans_curr.length) break;
        }
        return ans_curr.unit(ans_backup);
    }

    net_set<uint64_t> find_key(const arg &value) const {
        auto all_key = key_set();
        net_set<uint64_t> ans;
        if (all_key.length == 0) return ans;
        net_ptr_base<uint64_t> ans_ptr;
        ans_ptr.init(all_key.length);
        auto cnt = 0ull;
        for (auto i = 0ull; i < all_key.length; ++i) if(*this[all_key[i]] == value) *(ans_ptr.ptr_base + cnt++) = all_key[i];
        if (cnt != ans_ptr.len) ptr_alter(ans_ptr.ptr_base, ans_ptr.len, cnt);
        ans_ptr.len = cnt;
        ans.pointer = std::move(ans_ptr);
        return ans;
    }

    uint64_t insert(const net_set<net_kv<k_arg, arg>> &elem_list) {
        // memory check
        auto lexicon = !backup;
        auto upper   = 0ull,
             alloc   = 0ull;
        if (len[backup]) {
            upper   = (len[backup] + elem_list.size()) / hash_upper_factor,
            alloc   = kv_data[backup].length;
            lexicon = backup;
        } else {
            upper = (len[!backup] + elem_list.size()) / hash_upper_factor,
            alloc = kv_data[!backup].length;
        }
        if (alloc < upper) {
            while(alloc < upper) alloc <<= 1;
            rehash_backup_init(alloc);
            lexicon = backup;
        }
        // insert
        auto cnt = 0ull;
        for (auto i = 0ull; i < elem_list.length; ++i) {
            auto tgt_idx = find_idx(elem_list[i].key, lexicon);
            kv_data[lexicon][tgt_idx] = elem_list[i];
            kv_occ[lexicon][tgt_idx]  = true;
            kv_def[lexicon][tgt_idx]  = true;
            ++cnt;
        }
        len[lexicon] += cnt;
        rehash_transfer();
        return cnt ? (cnt == elem_list.size() ? NEUNET_MAP_INSERT_SUCCESS : NEUNET_MAP_INSERT_PARTIAL) : NEUNET_MAP_INSERT_FAILED;
    }
    uint64_t insert(const k_arg &key, const arg &value) { return insert({net_kv<k_arg, arg>(key, value)}); }

    net_set<net_kv<k_arg, arg>> erase(const net_set<k_arg> &key_list) {
        // erase
        net_set<net_kv<k_arg, arg>> ans;
        if (key_list.size() == 0) return ans;
        net_ptr_base<net_kv<k_arg, arg>> ans_ptr;
        ans_ptr.init(key_list.size());
        auto cnt = 0ull;
        for (auto i = 0ull; i < key_list.length; ++i) {
            auto tgt_idx = 0ull;
            if (len[backup]) {
                tgt_idx = find_idx(key_list[i], backup, false);
                if (kv_def[backup][tgt_idx]) {
                    kv_occ[backup][tgt_idx] = false;
                    ans_ptr.ptr_base[cnt++] = kv_data[backup][tgt_idx];
                    --len[backup];
                    continue;
                }
            }
            tgt_idx = find_idx(key_list[i], !backup, false);
            if (kv_def[!backup][tgt_idx]) {
                kv_occ[!backup][tgt_idx] = false;
                ans_ptr.ptr_base[cnt++] = kv_data[!backup][tgt_idx];
                --len[!backup];
            }
        }
        if (cnt != ans_ptr.len) ptr_alter(ans_ptr.ptr_base, ans_ptr.len, cnt);
        ans.pointer = std::move(ans_ptr);
        // shrink
        uint64_t lower = (len[0] + len[1]) / hash_lower_factor,
                 alloc = kv_data[!backup].length;
        if (len[backup]) alloc = kv_data[backup].length;
        auto lower_lim = alloc >> 1;
        if (lower < lower_lim) {
            do
            {
                alloc       = lower_lim;
                lower_lim >>= 1;
            } while (lower < lower_lim);
            rehash_backup_init(alloc);
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

    iterator begin() const { return iterator(key_set(), this); }

    iterator end() const { return iterator(net_set<k_arg>(), nullptr); }

    arg &operator[](const k_arg &key) const {
        auto tgt_idx = 0ull;
        if (len[backup]) {
            tgt_idx = find_idx(key, backup, false);
            if (kv_occ[backup][tgt_idx]) return kv_data[backup][tgt_idx].value;
        }
        tgt_idx = find_idx(key, !backup, false);
        assert(kv_occ[!backup][tgt_idx]);
        return kv_data[!backup][tgt_idx].value;
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
        auto all_key     = key_set(),
             src_all_key = src.key_set();
        if (all_key.length == src_all_key.length) {
            for (auto i = 0ull; i < all_key.length; ++i) {
                auto idx     = find_idx(all_key[i], !backup),
                     src_idx = find_idx(all_key[i], !backup);
            }
            return true;
        } else return false;
    }

    bool operator!=(const net_map &src) const { return !(*this == src); }

    void reset() {
        backup            = true;
        hash_upper_factor = 0.75;
        hash_lower_factor = 0.1;
        rehash_load_base  = 4;
        rehash_load       = rehash_load_base;
        rehash_idx        = 0;
        len[0]            = 0;
        len[1]            = 0;
        kv_data[0].reset();
        kv_data[1].reset();
        kv_occ[0].reset();
        kv_occ[1].reset();
        kv_def[0].reset();
        kv_def[1].reset();
    }

    ~net_map() { reset(); }

protected:
    bool        backup            = true;
    long double hash_upper_factor = 0.75,
                hash_lower_factor = 0.1;
    uint64_t    rehash_load_base  = 4,
                rehash_load       = rehash_load_base,
                rehash_idx        = 0,
                len[2]            {0, 0};

    net_set<net_kv<k_arg, arg>> kv_data[2];
    net_set<bool>               kv_occ[2],
                                kv_def[2];

    // hash function
    std::function<uint64_t(const k_arg&)> hash_func;

public:
    __declspec(property(get=size))     uint64_t length;
    __declspec(property(get=mem_size)) uint64_t memory_length;

    bool output_ordered = false;

    friend std::ostream &operator<<(std::ostream &out, net_map &src) {
        auto all_key = src.key_set();
        if(src.output_ordered) all_key.sort();
        out << "[Length " << src.len[!src.backup] << "][Memory " << src.kv_data[!src.backup].length << ']';
        if (all_key.length) {
            out << '\n';
            for (auto i = 0ull; i < all_key.length; ++i) {
                out << "[K][\n" << all_key[i] << "\n]";
                out << " -> ";
                out << "[V][\n" << src[all_key[i]] << "\n]";
                if (i + 1 != all_key.length) out << '\n';
            }
        }
        return out;
    }
};

NEUNET_END