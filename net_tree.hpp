NEUNET_BEGIN

/* key -> value */
template <typename k_arg, typename arg> struct net_kv {
    k_arg key{};
    arg   value{};

    bool valid = true;

    net_kv() {}
    net_kv(const k_arg &_key, const arg &_value) :
        key(_key),
        value(_value) {}
    net_kv(k_arg &&_key, arg &&_value) :
        key(std::move(_key)),
        value(std::move(_value)) {}
    net_kv(const net_kv &src) :
        valid(src.valid),
        key(src.key),
        value(src.value) {}
    net_kv(net_kv &&src) :
        valid(src.valid),
        key(std::move(src.key)),
        value(std::move(src.value)) {}
    
    bool operator==(const net_kv &val) { return key == val.key && value == val.value; }
    
    bool operator!=(const net_kv &val) { return !(*this == val); }

    net_kv &operator=(const net_kv &src) {
        valid = src.valid;
        key   = src.key;
        value = src.value;
        return *this;
    }
    net_kv &operator=(net_kv &&src) {
        valid = src.valid;
        key   = std::move(src.key);
        value = std::move(src.value);
        return *this;
    }
};

template <typename k_arg, typename arg> struct net_leaf {
    uint64_t hash_key = 0;

    net_kv<k_arg, arg> elem;

    bool red = true;

    net_leaf *pt = nullptr,
             *lc = nullptr,
             *rc = nullptr;

    ~net_leaf() {
        hash_key = 0;
        pt       = nullptr;

        while (lc) {
            delete lc;
            lc = nullptr;
        }

        while (rc) {
            delete rc;
            rc = nullptr;
        }
    }
};

template <typename k_arg, typename arg> net_leaf<k_arg, arg> *net_leaf_root_prev(net_leaf<k_arg, arg> *src_lc) {
    if (src_lc == nullptr) return src_lc;
    if (src_lc->rc) return net_leaf_root_prev(src_lc->rc);
    else return src_lc;
}

template <typename k_arg, typename arg> net_leaf<k_arg, arg> *net_leaf_root_next(net_leaf<k_arg, arg> *src_rc) {
    if (src_rc == nullptr) return src_rc;
    if (src_rc->lc) return net_leaf_root_prev(src_rc->lc);
    else return src_rc;
}

template <typename k_arg, typename arg> net_leaf<k_arg, arg> *net_leaf_pred(net_leaf<k_arg, arg> *src) {
    if (src == nullptr) return src;
    if (src->lc) return net_leaf_root_prev(src->lc);
    else if (src->pt && src->pt->rc != src) while (src->pt && src->pt->lc == src) src = src->pt;
    return src->pt;
}

template <typename k_arg, typename arg> net_leaf<k_arg, arg> *net_leaf_succ(net_leaf<k_arg, arg> *src) {
    if (src == nullptr) return src;
    if (src->rc) return net_leaf_root_next(src->rc);
    else if (src->pt && src->pt->lc != src) while (src->pt && src->pt->rc == src) src = src->pt;
    return src->pt;
}

template <typename k_arg, typename arg> net_leaf<k_arg, arg> *net_leaf_find(net_leaf<k_arg, arg> *root, uint64_t hash_key) {
    if (root == nullptr) return nullptr;
    if (root->hash_key > hash_key)
        if (root->lc == nullptr) return root;
        else return net_leaf_find(root->lc, hash_key);
    else if (root->hash_key < hash_key)
        if (root->rc == nullptr) return root;
        else return net_leaf_find(root->rc, hash_key);
    else return root;
}

template <typename k_arg, typename arg> void net_leaf_rot(net_leaf<k_arg, arg> *&root, net_leaf<k_arg, arg> *tool, bool left_rot = true) {
    if (tool == nullptr || (tool->rc == nullptr && left_rot) || (tool->lc == nullptr && !left_rot)) return;
    net_leaf<k_arg, arg> *pre_tool = nullptr;
    if (left_rot) {
        pre_tool      = tool->rc;
        tool->rc      = pre_tool->lc;
        pre_tool->lc  = tool;
    } else {
        pre_tool      = tool->lc;
        tool->lc      = pre_tool->rc;
        pre_tool->rc  = tool;
    }
    pre_tool->pt  = tool->pt;
    tool->pt      = pre_tool;
    if (left_rot && tool->rc) tool->rc->pt = tool;
    if (!left_rot && tool->lc) tool->lc->pt = tool;
    if (pre_tool->pt)
        if (pre_tool->pt->lc == tool) pre_tool->pt->lc = pre_tool;
        else pre_tool->pt->rc = pre_tool;
    else root = pre_tool;
    pre_tool = nullptr;
}

template <typename k_arg, typename arg> bool net_leaf_insert_check(net_leaf<k_arg, arg> *&root, net_leaf<k_arg, arg> *pre_tool, net_leaf<k_arg, arg> *tool) {
    if (!pre_tool->red) return true;
    auto g_tool = pre_tool->pt;
    // uncle is red
    if ((g_tool->hash_key > tool->hash_key && g_tool->rc && g_tool->rc->red) || (g_tool->hash_key < tool->hash_key && g_tool->lc && g_tool->lc->red)) {
        pre_tool->red = false;
        if (g_tool->hash_key > tool->hash_key) g_tool->rc->red = false;
        else g_tool->lc->red = false;
        tool = g_tool;
        if (tool == root) return true;
        else tool->red = true;
        pre_tool = tool->pt;
        g_tool   = pre_tool->pt;
        return net_leaf_insert_check(root, pre_tool, tool);
    }
    // uncle is black
    if (g_tool->hash_key > tool->hash_key) {
        // left
        if (tool == pre_tool->rc) {
            net_leaf_rot(root, pre_tool);
            tool     = pre_tool;
            pre_tool = tool->pt;
        }
        net_leaf_rot(root, g_tool, false);
        std::swap(pre_tool->red, g_tool->red);
    } else {
        // right
        if (tool == pre_tool->lc) {
            net_leaf_rot(root, pre_tool, false);
            tool     = pre_tool;
            pre_tool = tool->pt;
        }
        net_leaf_rot(root, g_tool);
        std::swap(pre_tool->red, g_tool->red);
    }
    return true;
}

template <typename k_arg, typename arg> bool net_leaf_insert(net_leaf<k_arg, arg> *&root, uint64_t hash_key, net_kv<k_arg, arg> &&src) {
    auto tool = new net_leaf<k_arg, arg> {hash_key, std::move(src)};
    if (root == nullptr) {
        root      = tool;
        root->red = false;
        return true;
    }
    auto pre_tool = net_leaf_find(root, hash_key);
         tool->pt = pre_tool;
    if (pre_tool->hash_key > hash_key) pre_tool->lc = tool;
    else pre_tool->rc = tool;
    return net_leaf_insert_check(root, pre_tool, tool);
}

template <typename k_arg, typename arg> net_kv<k_arg, arg> net_leaf_erase_check(net_leaf<k_arg, arg> *&root, net_leaf<k_arg, arg> *pre_tool, net_leaf<k_arg, arg> *tool) {
    // with one child
    if (tool->lc) {
        // left child
        std::swap(tool->elem, tool->lc->elem);
        std::swap(tool->hash_key, tool->lc->hash_key);
        tool = tool->lc;
    }
    if (tool->rc) {
        // right child
        std::swap(tool->elem, tool->rc->elem);
        std::swap(tool->hash_key, tool->rc->hash_key);
        tool = tool->rc;
    }
    pre_tool = tool->pt;
    if (tool->lc == nullptr && tool->rc == nullptr && !tool->red && tool != root) {
        net_leaf<k_arg, arg> *temp = nullptr;
        auto rot_left = true;
        if (tool == pre_tool->lc) {
            temp     = pre_tool->rc;
            rot_left = true;
        } else {
            temp     = pre_tool->lc;
            rot_left = false;
        }
        if (temp && temp->red && temp->lc && temp->rc) {
            // uncle -> red
            net_leaf_rot(root, pre_tool, rot_left);
            std::swap(pre_tool->red, pre_tool->pt->red);
            temp = nullptr;
            return net_leaf_erase_check(root, pre_tool, tool);
        }
        // left nephew is red
        if (temp && temp->lc && temp->lc->red) {
            if (rot_left) { // left
                net_leaf_rot(root, temp, !rot_left);
                temp = temp->pt;
                std::swap(temp->red, temp->rc->red);
            } else { // right
                temp->lc->red = false;
                std::swap(pre_tool->red, temp->red);
                net_leaf_rot(root, pre_tool, rot_left);
            }
            temp = nullptr;
            return net_leaf_erase_check(root, pre_tool, tool);
        }
        // right nephew is red
        if (temp && temp->rc && temp->rc->red) {
            if (rot_left) { // left
                temp->rc->red = false;
                std::swap(pre_tool->red, temp->red);
                net_leaf_rot(root, pre_tool, rot_left);
            } else { // right
                net_leaf_rot(root, temp, !rot_left);
                temp = temp->pt;
                std::swap(temp->red, temp->lc->red);
            }
            temp = nullptr;
            return net_leaf_erase_check(root, pre_tool, tool);
        }
        if (pre_tool) pre_tool->red = false;
        if (temp) temp->red = true;
    }
    auto ans = std::move(tool->elem);
    if (tool == root) root = nullptr;
    else if (pre_tool->lc == tool) pre_tool->lc = nullptr;
    else pre_tool->rc = nullptr;
    delete tool;
    tool     = nullptr;
    pre_tool = nullptr;
    return ans;
}

template <typename k_arg, typename arg> net_kv<k_arg, arg> net_leaf_erase(net_leaf<k_arg, arg> *&root, uint64_t hash_key) {
    auto tool = net_leaf_find(root, hash_key);
    if (tool == nullptr || tool->hash_key != hash_key) {
        net_kv<k_arg, arg> ans;
        ans.valid = false;
        return ans;
    }
    if (tool->lc && tool->rc) {
        auto temp = net_leaf_root_next(tool->rc);
        std::swap(tool->hash_key, temp->hash_key);
        std::swap(tool->elem, temp->elem);
        tool = temp;
        temp = nullptr;
    }
    auto pre_tool = tool->pt;
    return net_leaf_erase_check(root, pre_tool, tool);
}

template <typename k_arg, typename arg, typename f_arg, typename ... args> void net_leaf_iter(const net_leaf<k_arg, arg> *root, f_arg &&func, args &&...paras) {
    if (root == nullptr) return;
    net_leaf_iter(root->lc, func, std::forward<args>(paras)...);
    func(root, std::forward<args>(paras)...);
    net_leaf_iter(root->rc, func, std::forward<args>(paras)...);
}

template <typename k_arg, typename arg> void net_leaf_copy(net_leaf<k_arg, arg> *&dest, net_leaf<k_arg, arg> *src, net_leaf<k_arg, arg> *dest_pt = nullptr, bool is_pt_lt = true) {
    if (src == nullptr) return;
    while (dest) {
        delete dest;
        dest = nullptr;
    }
    dest           = new net_leaf<k_arg, arg>;
    dest->elem     = src->elem;
    dest->hash_key = src->hash_key;
    dest->red      = src->red;
    dest->pt       = dest_pt;
    if (dest->pt) {
        if (is_pt_lt) dest->pt->lc = dest;
        else dest->pt->rc = dest;
    }
    net_leaf_copy(dest->lc, src->lc, dest);
    net_leaf_copy(dest->rc, src->rc, dest, false);
}

template <typename k_arg, typename arg> bool net_leaf_compare(net_leaf<k_arg, arg> *fst, net_leaf<k_arg, arg> *snd) {
    while (fst->lc) fst = fst->lc;
    while (snd->lc) snd = snd->lc;
    while (fst && snd) if (fst->elem == snd->elem) {
        if (fst->rc) fst = net_leaf_next(fst->rc);
        else if (fst->pt && fst->pt->lc == fst) fst = fst->pt;
        else {
            while (fst->pt && fst->pt->rc == fst) fst = fst->pt;
            fst = fst->pt;
        }
        if (snd->rc) snd = net_leaf_next(snd->rc);
        else if (snd->pt && snd->pt->lc == snd) snd = snd->pt;
        else {
            while (snd->pt && snd->pt->rc == snd) snd = snd->pt;
            snd = snd->pt;
        }
    } else return false;
    if (fst == nullptr && snd == nullptr) return true;
    else return false;
}

template <typename k_arg, typename arg> class net_tree {
protected:
    struct iterator final : net_iterator_base<net_kv<k_arg, arg>, net_tree<k_arg, arg>> {
    public:
        iterator(const net_tree *src = nullptr) : net_iterator_base<net_kv<k_arg, arg>, net_tree<k_arg, arg>>(src) {
            if (src) {
                curr = src->root;
                while (curr->lc) curr = curr->lc;
            }
        }

        virtual bool operator==(const iterator &val) const { return net_iterator_base<net_kv<k_arg, arg>, net_tree<k_arg, arg>>::operator==(val) && curr == val.curr; }

        virtual bool operator!=(const iterator &val) const { return !(*this == val); }

        virtual net_kv<k_arg, arg> operator*() const {
            if (this->ptr) return net_kv(curr->elem);
            else return net_kv<k_arg, arg>();
        }

        virtual iterator &operator++() {
            if (this->ptr) {
                curr = net_leaf_succ(curr);
                if (curr == nullptr) this->ptr = nullptr;
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
                curr = net_leaf_pred(curr);
                if (curr == nullptr) this->ptr = nullptr;
            }
            return *this;
        }
        virtual iterator operator--(int) {
            auto temp = *this;
            --*this;
            return temp;
        }

        virtual ~iterator() { curr = nullptr; }

    private: net_leaf<k_arg, arg> *curr = nullptr;
    };

    void value_copy(const net_tree &src) {
        neunet::net_leaf_copy(root, src.root);
        len       = src.len;
        hash_func = src.hash_func;
    }

    void value_move(net_tree &&src) {
        root      = src.root;
        len       = src.len;
        src.root  = nullptr;
        src.len   = 0;
        hash_func = std::move(src.hash_func);
    }

public:
    net_tree(const std::function<uint64_t(const k_arg&)> &hash_key_func = [](const k_arg &key) { return hash_in_built(key); }) :
        hash_func(hash_key_func) {}
    net_tree(const std::initializer_list<net_kv<k_arg, arg>> &init_list, const std::function<uint64_t(const k_arg&)> &hash_key_func = [](const k_arg &key) { return hash_in_built(key); }) : net_tree(hash_key_func) { assert(insert(init_list) == NEUNET_INSERT_SUCCESS); }
    net_tree(const net_tree &src) { value_copy(src); }
    net_tree(net_tree &&src) { value_move(std::move(src)); }

    uint64_t size() const { return len; }

    net_set<k_arg> key_set() const {
        net_set<k_arg> ans;
        if (len == 0) return ans;
        ans.init(len);
        uint64_t idx = 0;
        net_leaf_iter(root, [](const neunet::net_leaf<k_arg, arg> *leaf, uint64_t &idx, net_set<k_arg> &ans) { ans[idx++] = leaf->elem.key; }, std::ref(idx), std::ref(ans));
        return ans;
    }

    net_set<k_arg> find_key(const arg &value) const {
        net_set<uint64_t> ans;
        if (len == 0) return ans;
        net_ptr_base<k_arg> ans_ptr;
        ans_ptr.init(len);
        uint64_t cnt = 0;
        net_leaf_iter(root, [](const neunet::net_leaf<k_arg, arg> *leaf, uint64_t &cnt, net_ptr_base<k_arg> &ptr, const arg &value){ if (leaf->elem.value == value) *(ptr.ptr_base + cnt++) = leaf->elem.key; }, std::ref(cnt), std::ref(ans_ptr), std::ref(value));
        if (cnt != ans_ptr.len) ptr_alter(ans_ptr.ptr_base, ans_ptr.len, cnt);
        ans_ptr.len = cnt;
        ans.pointer = std::move(ans_ptr);
        return ans;
    }

    bool insert(uint64_t hash_key, k_arg &&key, arg &&value) {
        net_kv temp(std::move(key), std::move(value));
        if (net_leaf_insert(root, hash_key, std::move(temp))) {
            ++len;
            return true;
        }
        return false;
    }
    uint64_t insert(net_set<net_kv<k_arg, arg>> &&elem_list) {
        uint64_t cnt = 0;
        for (auto i = 0ull; i < elem_list.length; ++i) {
            auto curr_leaf_val = hash_func(elem_list[i].key);
            if (net_leaf_insert(root, curr_leaf_val, std::move(elem_list[i]))) {
                ++len;
                ++cnt;
            }
        }
        auto src_len = elem_list.length;
        elem_list.reset();
        if (cnt == src_len) return NEUNET_INSERT_SUCCESS;
        else if (cnt == 0) return NEUNET_INSERT_FAILED;
        else return NEUNET_INSERT_PARTIAL;
    }
    uint64_t insert(const std::initializer_list<net_kv<k_arg, arg>> &init_list) {
        net_set<net_kv<k_arg, arg>> elem_list(init_list.size());
        auto cnt = 0ull;
        for (auto temp : init_list) elem_list[cnt++] = std::move(temp);
        return insert(std::move(elem_list));
    }
    bool insert(const k_arg &key, const arg &value) { return NEUNET_INSERT_SUCCESS == insert({net_kv<k_arg, arg>(key, value)}); }

    net_kv<k_arg, arg> erase(uint64_t hash_key) {
        auto temp = net_leaf_erase(root, hash_key);
        if (temp.valid) --len;
        return temp;
    }
    net_set<net_kv<k_arg, arg>> erase(const net_set<k_arg> &k_set) {
        uint64_t cnt = 0;
        net_ptr_base<net_kv<k_arg, arg>> ans_ptr;
        ans_ptr.init(k_set.length);
        for (auto temp : k_set) {
            auto curr_leaf_val = hash_func(temp);
            auto kv_temp       = net_leaf_erase(root, curr_leaf_val);
            if (kv_temp.valid) {
                *(ans_ptr.ptr_base + cnt++) = std::move(kv_temp);
                --len;
            }
        }
        net_set<net_kv<k_arg, arg>> ans(cnt);
        if (cnt != ans_ptr.len) ptr_alter(ans_ptr.ptr_base, ans_ptr.len, cnt);
        ans_ptr.len = cnt;
        ans.pointer = std::move(ans_ptr);
        return ans;
    }
    net_set<net_kv<k_arg, arg>> erase(const std::initializer_list<k_arg> &init_k) {
        net_set<k_arg> k_set_temp(init_k.size());
        auto cnt = 0ull;
        for (auto temp : init_k) k_set_temp[cnt++] = std::move(temp);
        return erase(k_set_temp);
    }
    net_kv<k_arg, arg> erase(const k_arg &key) { return net_leaf_erase(root, hash_func(key)); }

    bool hash_key_verify (uint64_t hash_key) const { return len && net_leaf_find(root, hash_key)->hash_key == hash_key; }

    iterator begin() const {
        if (len) return iterator(this);
        else return end();
    }

    iterator end() const { return iterator(nullptr); }

    k_arg root_key() const {
        k_arg ans {};
        if (root) ans = root->elem.key;
        return ans;
    }
    
    arg &get_value(uint64_t hash_key) const {
        auto tgt_leaf = net_leaf_find(root, hash_key);
        assert(tgt_leaf);
        return tgt_leaf->elem.value;
    }

    arg &operator[] (const k_arg &key) const {
        auto tgt_hash = hash_func(key);
        return get_value(tgt_hash);
    }

    bool operator==(const net_tree &src) const {
        if (len != src.len) return false;
        return net_leaf_compare(root, src.root);
    }

    net_tree &operator=(const net_tree &src) {
        value_copy(src);
        return *this;
    }
    net_tree &operator=(net_tree &&src) {
        value_move(std::move(src));
        return *this;
    }

    void reset() {
        delete root;
        root = nullptr;
        len  = 0;
    }

    ~net_tree() { reset(); }

protected:
    net_leaf<k_arg, arg> *root = nullptr;

    uint64_t len = 0; 

    // hash function
    std::function<uint64_t(const k_arg&)> hash_func;

public:
    bool list_mode = false;

    __declspec(property(get=size)) uint64_t length;

    friend std::ostream &operator<<(std::ostream &out, const net_tree &src) {
        auto cnt = 0ull;
        net_leaf_iter(src.root, [&src, &out](const net_leaf<k_arg, arg> *leaf, uint64_t &cnt) {
            out << "[hash "   << leaf->hash_key                << ']';
            out << "[key "    << leaf->elem.key                << ']';
            out << "[value "  << leaf->elem.value              << ']';
            out << "[color "  << (leaf->red ? "red" : "black") << ']';
            // link
            out << "[father " << (leaf->pt ? std::to_string(leaf->pt->hash_key) : "null") << ']';
            out << "[left "   << (leaf->lc ? std::to_string(leaf->lc->hash_key) : "null") << ']';
            out << "[right "  << (leaf->rc ? std::to_string(leaf->rc->hash_key) : "null") << ']';
            ++cnt;
            if (cnt < src.size())out << '\n';
        }, std::ref(cnt));
        return out;
    }

};

NEUNET_END