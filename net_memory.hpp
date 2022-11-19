NEUNET_BEGIN

template <typename arg> class net_memory {
protected:
    struct net_ptr_mem final : private net_ptr_base<net_memory<arg>> {
    private:
        void value_assign(const net_ptr_mem &src) {
            this->ptr_base = src.ptr_base;
            this->len      = src.len;
            ptr_addr       = src.ptr_addr;
        }

        void destroy() {
            this->ptr_base = nullptr;
            this->len      = 0;
            ptr_addr       = 0;
        }

    public:
        net_ptr_mem(net_memory *src = nullptr, uint64_t ptr_len = 0, uint64_t addr = 0) :
            ptr_addr(addr) {
            this->ptr_base = src;
            this->len      = ptr_len;
        }
        net_ptr_mem(net_ptr_mem &&src) {
            value_assign(src);
            src.destroy();
        }
        net_ptr_mem(const net_ptr_mem &src) { value_assign(src); }

        uint64_t size() const { return this->len; }

        arg &operator[](uint64_t idx) const {
            assert(idx < this->len);
            return this->ptr_base->mem_val[ptr_addr + idx];
        }

        net_ptr_mem &operator=(const net_ptr_mem &src) {
            value_assign(src);
            return *this;
        }

        net_ptr_mem &operator=(net_ptr_mem &&src) {
            value_assign(src);
            src.destroy();
            return *this;
        }

        bool operator==(const net_ptr_mem &src) const { return this->ptr_base == src.ptr_base && this->len == src.len && ptr_addr == src.ptr_addr; }

        bool operator!=(const net_ptr_mem &src) const { !(*this == src); }

        ~net_ptr_mem() { destroy(); }

    private: uint64_t ptr_addr = 0;

    public:
        __declspec(property(get=size)) uint64_t length;

        friend std::ostream &operator<<(std::ostream &out, const net_ptr_mem &src) {
            out << "[Length " << src.len << ']';
            for (auto i = 0ull; i < src.len; ++i) {
                out << '\n';
                out << '[' << i << "][\n";
                out << src[i] << "\n]";
            }
            return out;
        }
    };

    struct net_mem_blk_info { uint64_t prev_id = 0, next_id = 0, addr = 0, addr_curr_len = 0, addr_mem_len = 0; };

    void value_assign(const net_memory &src) {
        head_id = src.head_id;
        rear_id = src.rear_id;
        id_gen  = src.id_gen;
        blk_cnt = src.blk_cnt;
        mem_len = src.mem_len;
    }

    void value_copy(const net_memory &src) {
        value_assign(src);
        mem_val       = src.mem_val;
        mem_blk_info  = src.mem_blk_info;
        mem_blk_blank = src.mem_blk_blank;
    }

    void value_move(net_memory &&src) {
        value_assign(src);
        mem_val       = std::move(src.mem_val);
        mem_blk_info  = std::move(src.mem_blk_info);
        mem_blk_blank = std::move(src.mem_blk_blank);
        src.reset();
    }

public:
    net_memory(uint64_t buffer_length = 128) :
        mem_val(buffer_length),
        mem_blk_info(buffer_length) {}
    net_memory(net_memory&& src) { value_move(std::move(src)); }
    net_memory(const net_memory& src) { value_copy(src); }
    
    uint64_t size() const {
        if (blk_cnt == 0) return 0;
        if (head_id == rear_id) return mem_blk_info[head_id].addr_curr_len;
        auto temp = head_id,
             ans = 0ull;
        while (temp != rear_id) {
            ans += mem_blk_info[temp].addr_curr_len;
            temp = mem_blk_info[temp].next_id;
        }
        return ans;
    }

    uint64_t mem_size() const { return mem_len; }

    uint64_t mem_size_max() const { return mem_val.length; }

    void print_mem_blk_info(uint64_t id, bool detail = false) {
        if (!id_verify(id)) std::cout << "[Null]" << std::endl;
        std::cout << "[ID " << id << "][Length " << mem_blk_info[id].addr_curr_len << ']';
        if (!detail) {
            std::cout << std::endl;
            return;
        }
        std::cout << "[Max " << mem_blk_info[id].addr_mem_len << "][Predecessor " << mem_blk_info[id].prev_id << "][Successor " << mem_blk_info[id].next_id << ']' << std::endl;
    }

    void print_mem_info(bool detail = false) {
        if (blk_cnt == 0) {
            std::cout << "[Null]" << std::endl;
            return;
        }
        std::cout << "[Block " << blk_cnt << "][Memory " << mem_len << "][Max " << mem_val.length << ']' << std::endl;
        auto id_temp = head_id;
        do {
            auto flag = true;
            if (mem_blk_info[id_temp].addr_curr_len == 0 && !detail) flag = false;
            if (flag) print_mem_blk_info(id_temp, detail);
            id_temp = mem_blk_info[id_temp].next_id;
        } while (id_temp != head_id);
    }

    uint64_t allocate(uint64_t alloc_size) {
        ++blk_cnt;
        uint64_t cnt      = 0,
                 id_temp  = 0;
        auto     gen_flag = true;
        while (cnt < mem_blk_blank.length) {
            id_temp = mem_blk_blank.erase(0);
            if (mem_blk_info[id_temp].addr_mem_len >= alloc_size) {
                mem_blk_info[id_temp].addr_curr_len = alloc_size;
                return id_temp;
            }
            else if (mem_blk_info[id_temp].addr_mem_len == 0) {
                gen_flag = false;
                break;
            }
            else {
                mem_blk_blank.emplace_back(id_temp);
                ++cnt;
            }
        }
        if (gen_flag) id_temp = id_gen++;
        if (id_temp == mem_blk_info.length) mem_blk_info.init(id_gen << 1);
        mem_len += alloc_size;
        if (mem_len >= mem_val.length) {
            auto mem_len_max = mem_val.length;
            while (mem_len >= mem_len_max) mem_len_max <<= 1;
            mem_val.init(mem_len_max);
        }
        net_mem_blk_info mem_info{rear_id, head_id, mem_blk_info[rear_id].addr + mem_blk_info[rear_id].addr_mem_len, alloc_size, alloc_size };
        mem_blk_info[id_temp]         = mem_info;
        mem_blk_info[head_id].prev_id = id_temp;
        mem_blk_info[rear_id].next_id = id_temp;
        rear_id                       = id_temp;
        return id_temp;
    }

    bool reallocate(uint64_t &id, uint64_t alloc_len, bool remain = true) {
        if (!id_verify(id)) return false;
        if (alloc_len <= mem_blk_info[id].addr_mem_len) {
            mem_blk_info[id].addr_curr_len = alloc_len;
            if (!remain) for (auto i = 0ull; i < mem_blk_info[id].addr_curr_len; ++i) mem_val[i + mem_blk_info[id].addr] = arg();
        }
        else {
            auto id_temp = allocate(alloc_len);
            if (remain) {
                for (auto i = 0ull; i < mem_blk_info[id].addr_curr_len; ++i) mem_val[mem_blk_info[id_temp].addr + i] = std::move(mem_val[mem_blk_info[id].addr + i]);
            }
            mem_blk_info[id].addr_curr_len = 0;
            mem_blk_blank.insert(0, id);
            id = id_temp;
        }
        return true;
    }

    bool release(uint64_t id) {
        if (!id_verify(id)) return false;
        mem_blk_info[id].addr_curr_len = 0;
        mem_blk_blank.insert(0, id);
        --blk_cnt;
        return true;
    }

    void shrink() {
        if (blk_cnt == 0 || (blk_cnt == 1 && mem_blk_info[head_id].addr_curr_len == 0)) {
            reset();
            return;
        }
        auto temp_id = head_id, curr_addr = 0ull;
        do {
            if (mem_blk_info[temp_id].addr_curr_len) {
                if (mem_blk_info[temp_id].addr != curr_addr) for (auto i = 0ull; i < mem_blk_info[temp_id].addr_curr_len; ++i) mem_val[curr_addr + i] = std::move(mem_val[mem_blk_info[temp_id].addr + i]);
                curr_addr += mem_blk_info[temp_id].addr_curr_len;
                if (mem_blk_info[temp_id].addr_mem_len != mem_blk_info[temp_id].addr_curr_len) {
                    mem_len                            -= mem_blk_info[temp_id].addr_mem_len - mem_blk_info[temp_id].addr_curr_len;
                    mem_blk_info[temp_id].addr_mem_len  = mem_blk_info[temp_id].addr_curr_len;
                }
            } else {
                auto curr_id  = temp_id;
                     temp_id  = mem_blk_info[temp_id].prev_id;
                     mem_len -= mem_blk_info[curr_id].addr_mem_len;
                if (curr_id == id_gen - 1) {
                    -- id_gen;
                    auto cnt = 0ull;
                    for (auto temp : mem_blk_blank)
                        if (temp == curr_id) break;
                        else ++cnt;
                    mem_blk_blank.erase(cnt);
                }
                // verify head or rear
                if (curr_id == head_id) head_id = mem_blk_info[curr_id].next_id;
                if (curr_id == rear_id) rear_id = mem_blk_info[curr_id].prev_id;
                mem_blk_info[mem_blk_info[curr_id].prev_id].next_id = mem_blk_info[curr_id].next_id;
                mem_blk_info[mem_blk_info[curr_id].next_id].prev_id = mem_blk_info[curr_id].prev_id;
                mem_blk_info[curr_id].prev_id                       = curr_id;
                mem_blk_info[curr_id].next_id                       = curr_id;
                mem_blk_info[curr_id].addr_mem_len                  = 0;
                mem_blk_info[curr_id].addr                          = 0;
            }
            temp_id = mem_blk_info[temp_id].next_id;
        } while (temp_id != head_id);
        // block
        if (id_gen < mem_blk_info.length * 0.1) {
            auto blk_alloc = mem_blk_info.length;
            do blk_alloc >>= 1;
            while(blk_alloc * 0.1 > id_gen);
            mem_blk_info.init(blk_alloc);
        }
        // memory
        if (mem_len < mem_val.length * 0.1) {
            auto mem_alloc = mem_val.length;
            do mem_alloc >>= 1;
            while(mem_alloc * 0.1 > mem_len);
            mem_val.init(mem_alloc);
        }
    }

    bool id_verify(uint64_t id) const {
        if (blk_cnt == 0) return false;
        auto temp = rear_id;
        do
        {
            temp = mem_blk_info[temp].next_id;
            if (temp == id) return true;
        } while (temp != rear_id);
        return false;
    }

    net_ptr_mem operator[](uint64_t id) {
        if (id_verify(id)) return net_ptr_mem(this, mem_blk_info[id].addr_curr_len, mem_blk_info[id].addr);
        else return net_ptr_mem();
    }

    net_memory &operator=(net_memory&& src) {
        value_move(std::move(src));
        return *this;
    }
    net_memory &operator=(const net_memory &src) {
        value_copy(src);
        return *this;
    }

    void reset() {
        mem_val.reset();
        mem_blk_info.reset();
        mem_blk_blank.reset();
        head_id = 0;
        rear_id = 0;
        blk_cnt = 0;
        mem_len = 0;
        id_gen  = 0;
    }
    
    ~net_memory() { reset(); }

protected:
    net_set<arg>              mem_val;
    net_set<net_mem_blk_info> mem_blk_info;

    net_list<uint64_t>        mem_blk_blank;

    uint64_t head_id = 0,
             rear_id = 0,
             blk_cnt = 0,
             mem_len = 0,
             id_gen  = 0;

public:
    __declspec(property(get=size))         uint64_t length;
    __declspec(property(get=mem_size))     uint64_t memory_length;
    __declspec(property(get=mem_size_max)) uint64_t memory_length_max;
};

NEUNET_END