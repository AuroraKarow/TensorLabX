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
    
    bool operator==(const net_kv &val) const { return key == val.key && value == val.value && valid == val.valid; }
    
    bool operator!=(const net_kv &val) const { return !(*this == val); }

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

template <typename k_arg> using hash_fn_t      = uint64_t(*)(const k_arg &);
template <typename k_arg,
          typename arg>   using init_list_kv_t = std::initializer_list<net_kv<k_arg, arg>>;

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

template <typename k_arg, typename arg> net_leaf<k_arg, arg> *net_leaf_pred(net_leaf<k_arg, arg> *src) {
    if (src == nullptr) return src;
    net_leaf<k_arg, arg> *ans = nullptr;
    if (src->lc) {
        ans = src->lc;
        while (ans->rc) ans = ans->rc;
    } else {
        auto tool = src;
             ans  = src->pt;
        while (ans && ans->lc == tool) {
            tool = ans;
            ans  = ans->pt;
        }
    }
    return ans;
}

template <typename k_arg, typename arg> net_leaf<k_arg, arg> *net_leaf_succ(net_leaf<k_arg, arg> *src) {
    if (src == nullptr) return src;
    net_leaf<k_arg, arg> *ans = nullptr;
    if (src->rc) {
        ans = src->rc;
        while (ans->lc) ans = ans->lc;
    } else {
        auto tool = src;
             ans  = src->pt;
        while (ans && ans->rc == tool) {
            tool = ans;
            ans  = ans->pt;
        }
    }
    return ans;
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
    /* g_tool -> grandfather
     * pre_tool -> father
     */
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
    if (!src.valid) return false;
    auto tool = new net_leaf<k_arg, arg> {hash_key, std::move(src)};
    if (root == nullptr) {
        root      = tool;
        root->red = false;
        return true;
    }
    auto pre_tool = net_leaf_find(root, hash_key);
         tool->pt = pre_tool;
    if (pre_tool->hash_key == hash_key) {
        if (pre_tool->elem.valid) return false;
        else pre_tool->elem = std::move(src);
        return true;
    }
    else if (pre_tool->hash_key > hash_key) pre_tool->lc = tool;
    else pre_tool->rc = tool;
    return net_leaf_insert_check(root, pre_tool, tool);
}

template <typename k_arg, typename arg> void net_leaf_erase_check(net_leaf<k_arg, arg> *&root, net_leaf<k_arg, arg> *pre_tool, net_leaf<k_arg, arg> *tool) {
    /* temp -> sibling
     * pre_tool -> father
     */
    net_leaf<k_arg, arg> *temp    = pre_tool->rc;
    auto                 left_rot = true;
    if (pre_tool->rc == tool) {
        temp = pre_tool->lc;
        left_rot = false;
    }
    if (temp == nullptr) return;
    if (temp->red) {
        // sibling->red
        net_leaf_rot(root, pre_tool, left_rot);
        std::swap(pre_tool->red, temp->red);
        return net_leaf_erase_check(root, pre_tool, tool);
    }
    if ((left_rot && temp->rc && temp->rc->red) || (!left_rot && temp->lc && temp->lc->red)) {
        // red nephew at same side as sibling
        net_leaf_rot(root, pre_tool, left_rot);
        std::swap(pre_tool->red, temp->red);
        if (left_rot) temp->rc->red = false;
        else temp->lc->red = false;
        return;
    }
    if ((left_rot && temp->lc && temp->lc->red) || (!left_rot && temp->rc && temp->rc->red)) {
        // red nephew at diffrent side from sibling
        net_leaf_rot(root, temp, !left_rot);
        std::swap(temp->pt->red, temp->red);
        return net_leaf_erase_check(root, pre_tool, tool);
    }
    if ((temp->lc == nullptr || !temp->lc->red) && (temp->rc == nullptr || !temp->rc->red)) {
        // all nephews are black
        temp->red = true;
        if (pre_tool->red || pre_tool == root) {
            // father is red
            pre_tool->red = false;
            return;
        }
        // father is black
        tool     = pre_tool;
        pre_tool = pre_tool->pt;
        return net_leaf_erase_check(root, pre_tool, tool);
    }
}

template <typename k_arg, typename arg> net_kv<k_arg, arg> net_leaf_erase(net_leaf<k_arg, arg> *&root, uint64_t hash_key) {
    net_kv<k_arg, arg> ans;
    ans.valid = false;
    auto tool = net_leaf_find(root, hash_key);
    if (tool == nullptr || tool->hash_key != hash_key) return ans;
    if (tool->lc && tool->rc) {
        auto temp = net_leaf_succ(tool);
        std::swap(tool->hash_key, temp->hash_key);
        std::swap(tool->elem, temp->elem);
        tool = temp;
        temp = nullptr;
    }
    auto pre_tool = tool;
    if (tool->lc || tool->rc) {
        // with one child
        if (tool->lc) {
            // left child
            std::swap(tool->elem, tool->lc->elem);
            std::swap(tool->hash_key, tool->lc->hash_key);
            tool         = tool->lc;
            pre_tool->lc = nullptr;
        }
        if (tool->rc) {
            // right child
            std::swap(tool->elem, tool->rc->elem);
            std::swap(tool->hash_key, tool->rc->hash_key);
            tool         = tool->rc;
            pre_tool->rc = nullptr;
        }
        tool->red = true;
    } else {
        pre_tool = pre_tool->pt;
        if (pre_tool) {
            if (pre_tool->lc == tool) pre_tool->lc = nullptr;
            else pre_tool->rc = nullptr;
        } else tool->red = true;
    }
    auto del_red = tool->red;
    ans = std::move(tool->elem);
    if (tool == root) root = nullptr;
    delete tool;
    tool = nullptr;
    if (!del_red) net_leaf_erase_check(root, pre_tool, tool);
    return ans;
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
public:
    struct iterator final : net_iterator_base<net_kv<k_arg, arg>, net_tree<k_arg, arg>> {
    public:
        iterator(const net_tree *src = nullptr) : net_iterator_base<net_kv<k_arg, arg>, net_tree<k_arg, arg>>(src) {
            if (src) {
                curr = src->root;
                while (curr->lc) curr = curr->lc;
                hash_key  = curr->hash_key;
                color_red = curr->red;
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
                if (curr) {
                    hash_key  = curr->hash_key;
                    color_red = curr->red;
                } else {
                    this->ptr = nullptr;
                    hash_key  = 0;
                }
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
                if (curr) {
                    hash_key  = curr->hash_key;
                    color_red = curr->red;
                } else {
                    this->ptr = nullptr;
                    hash_key  = 0;
                }
            }
            return *this;
        }
        virtual iterator operator--(int) {
            auto temp = *this;
            --*this;
            return temp;
        }

        const net_kv<k_arg, arg> *operator->() { return &(curr->elem); }

        bool is_end() { return curr == nullptr; }

        void change_valid(bool valid = true) { curr->elem.valid = valid; }

        virtual ~iterator() { curr = nullptr; }

    private: net_leaf<k_arg, arg> *curr = nullptr;

    public:
        uint64_t hash_key  = 0;
        bool     color_red = true;
    };
    
protected:
    void value_copy(const net_tree &src) {
        neunet::net_leaf_copy(root, src.root);
        len       = src.len;
        p_hash_fn = src.p_hash_fn;
    }

    void value_move(net_tree &&src) {
        root          = src.root;
        len           = src.len;
        p_hash_fn     = src.p_hash_fn;
        src.p_hash_fn = nullptr;
        src.root      = nullptr;
        src.len       = 0;
    }

public:
    net_tree(hash_fn_t<k_arg> hash_func = hash_in_built) :
        p_hash_fn(hash_func) {}
    net_tree(init_list_kv_t<k_arg, arg> init_list, hash_fn_t<k_arg> hash_func = hash_in_built) : net_tree(hash_func) { net_assert(insert(init_list) == NEUNET_INSERT_SUCCESS, "net_tree", "net_tree", "Tree initializing failed."); }
    net_tree(const net_tree &src) { value_copy(src); }
    net_tree(net_tree &&src) { value_move(std::move(src)); }

    uint64_t size() const { return len; }

    net_set<k_arg> key_set() const {
        net_set<k_arg> ans;
        if (len == 0) return ans;
        ans.init(len);
        uint64_t idx = 0;
        net_leaf_iter(root, [&ans](const neunet::net_leaf<k_arg, arg> *leaf, uint64_t &idx) { ans[idx++] = leaf->elem.key; }, std::ref(idx));
        return ans;
    }

    net_set<k_arg> find_key(const arg &value) const {
        net_set<k_arg> ans;
        if (len == 0) return ans;
        ans.init(len);
        uint64_t cnt = 0;
        net_leaf_iter(root, [&ans, &value](const neunet::net_leaf<k_arg, arg> *leaf, uint64_t &cnt){ if (leaf->elem.value == value) ans[cnt++] = leaf->elem.key; }, std::ref(cnt));
        if (cnt != len) ans.init(cnt);
        return ans;
    }

    bool insert(uint64_t hash_key, net_kv<k_arg, arg> &&src) {
        if (net_leaf_insert(root, hash_key, std::move(src))) {
            ++len;
            return true;
        } else return false;
    }
    uint64_t insert(net_set<net_kv<k_arg, arg>> &&elem_list) {
        uint64_t cnt = 0;
        for (auto i = 0ull; i < elem_list.length; ++i) {
            auto curr_hash = p_hash_fn(elem_list[i].key);
            if (insert(curr_hash, std::move(elem_list[i]))) ++cnt;
        }
        auto src_len = elem_list.length;
        elem_list.reset();
        if (cnt == src_len) return NEUNET_INSERT_SUCCESS;
        else if (cnt == 0) return NEUNET_INSERT_FAILED;
        else return NEUNET_INSERT_PARTIAL;
    }
    uint64_t insert(init_list_kv_t<k_arg, arg> init_list) { return insert(net_set<net_kv<k_arg, arg>>(init_list)); }
    bool insert(const k_arg &key, const arg &value) { return insert(p_hash_fn(key), net_kv(key, value)); }

    net_kv<k_arg, arg> erase(uint64_t hash_key) {
        auto temp = net_leaf_erase(root, hash_key);
        if (temp.valid) --len;
        return temp;
    }
    net_set<net_kv<k_arg, arg>> erase(const net_set<k_arg> &k_set) {
        uint64_t cnt = 0;
        net_set<net_kv<k_arg, arg>> ans(len);
        for (auto temp : k_set) {
            auto curr_hash = p_hash_fn(temp);
            auto kv_temp   = erase(curr_hash);
            if (kv_temp.valid) ans[cnt++] = std::move(kv_temp);
        }
        if (cnt != ans.length) ans.init(cnt);
        return ans;
    }
    net_kv<k_arg, arg> erase(const k_arg &key) {
        auto curr_hash = p_hash_fn(key);
        return erase(curr_hash);
    }

    bool hash_key_verify (uint64_t hash_key) const {
        if (len) {
            auto curr_leaf = net_leaf_find(root, hash_key);
            return curr_leaf->elem.valid && curr_leaf->hash_key == hash_key;
        } else return false;
    }

    iterator begin() const {
        if (len) return iterator(this);
        else return end();
    }

    iterator end() const { return iterator(nullptr); }
    
    arg &get_value(uint64_t hash_key) const {
        auto tgt_leaf = net_leaf_find(root, hash_key);
        net_assert(tgt_leaf && tgt_leaf->hash_key == hash_key,
                   "net_tree",
                   "get_value",
                   "Hash key is invalid.");
        return tgt_leaf->elem.value;
    }

    arg &operator[] (const k_arg &key) const {
        auto tgt_hash = p_hash_fn(key);
        return get_value(tgt_hash);
    }

    bool operator==(const net_tree &src) const {
        if (len != src.len) return false;
        return net_leaf_compare(root, src.root);
    }

    bool operator!=(const net_tree &src) const { return !(*this == src); }

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
        root      = nullptr;
        p_hash_fn = nullptr;
        len       = 0;
    }

    ~net_tree() { reset(); }

protected:
    net_leaf<k_arg, arg> *root = nullptr;

    uint64_t len = 0; 

    // hash function
    hash_fn_t<k_arg> p_hash_fn = nullptr;

public:
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