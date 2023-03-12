DATASET_BEGIN

class mnist final {
private:
    void value_assign(const mnist &src) {
        elem_ln_cnt  = src.elem_ln_cnt;
        elem_col_cnt = src.elem_col_cnt;
        elem_cnt     = src.elem_cnt;
    }

    void value_copy(const mnist &src) {
        value_assign(src);
        lbl  = src.lbl;
        elem = src.elem;
    }

    void value_move(mnist &&src) {
        value_assign(src);
        lbl  = std::move(src.lbl);
        elem = std::move(src.elem);
    }

    bool open_stream(const std::string &elem_dir, const std::string &lbl_dir) {
        strm_elem = std::ifstream(elem_dir, std::ios::in | std::ios::binary);
        strm_lbl  = std::ifstream(lbl_dir, std::ios::in | std::ios::binary);
        return strm_elem.is_open() && strm_lbl.is_open();
    }

    void close_stream() {
        strm_elem.close();
        strm_lbl.close();   
    }

    bool magic_verify() {
        uint32_t magic = 0;
        // data
        strm_elem.read(reinterpret_cast<char*>(&magic), sizeof(uint32_t));
        auto dat_magic = num_swap_endian(magic);
        if (dat_magic != mnist_magic_elem) return false;
        magic = 0;
        // label
        strm_lbl.read(reinterpret_cast<char*>(&magic), sizeof(uint32_t));
        auto lbl_magic = num_swap_endian(magic);
        if (lbl_magic != mnist_magic_lbl) return false;
        return true;
    }

    uint64_t load_elem_cnt() {
        uint32_t dat_cnt = 0, lbl_cnt = 0;
        strm_elem.read(reinterpret_cast<char*>(&dat_cnt), sizeof(uint32_t));
        auto shrk_dat_cnt = num_swap_endian(dat_cnt);
        strm_lbl.read(reinterpret_cast<char*>(&lbl_cnt), sizeof(uint32_t));
        auto shrk_lbl_cnt = num_swap_endian(lbl_cnt);
        if (shrk_dat_cnt == shrk_lbl_cnt) return shrk_lbl_cnt;
        else return 0;
    }

    inline uint64_t load_elem_shape() {
        uint32_t _temp = 0;
        strm_elem.read(reinterpret_cast<char*>(&_temp), sizeof(uint32_t));
        return num_swap_endian(_temp);
    }

    vect read_curr_elem(bool w_flag, uint64_t padding = 0) {
        auto data_length = elem_ln_cnt * elem_col_cnt;
        auto data_ptr    = ptr_init<char>(data_length);
        strm_elem.read(data_ptr, data_length);
        vect vec_data;
        if (w_flag) {
            vec_data = vect(data_length, 1);
            for (auto i = 0ull; i < data_length; ++i) {
                int curr_pt = data_ptr[i];
                if (curr_pt) switch (elem_status) {
                case mnist_elem_bool: vec_data.index(i) = 1; break;
                case mnist_elem_gray: vec_data.index(i) = 255; break;
                default: vec_data.index(i) = curr_pt; break;
                } else vec_data.index(i) = 0;
            }
            if (padding) vec_data = vec_data.padding(padding, padding, padding, padding);
            vec_data.reshape(data_length, 1);
            if (elem_status == mnist_elem_norm) vec_data.elem_wise_opt(vec_data.elem_sum(), MATRIX_ELEM_DIV);
        }
        ptr_reset(data_ptr);
        return vec_data;
    }

    uint64_t read_curr_lbl() {
        char label = 0;
        strm_lbl.read(&label, 1);
        return (uint64_t)label;
    }

    bool preprocess(const std::string &elem_dir, const std::string &lbl_dir) {
        if (open_stream(elem_dir, lbl_dir) && magic_verify()) {
            elem_cnt = load_elem_cnt();
            if (elem_cnt) {
                elem_ln_cnt  = load_elem_shape();
                elem_col_cnt = load_elem_shape();
                return true;
            }
        }
        return false;
    }

public:
    mnist(uint64_t load_elem_status = mnist_elem_orgn) :
        elem_status(load_elem_status < 3 ? load_elem_status : 0) {}
    mnist(const mnist &src) :
        elem_status(src.elem_status) { value_copy(src); }
    mnist(mnist &&src) :
        elem_status(src.elem_status) { value_move(std::move(src)); }
    mnist(const ch_str elem_dir, const ch_str lbl_dir, uint64_t load_cnt = 0, uint64_t load_elem_status = mnist_elem_orgn, uint64_t padding = 0) :
        elem_status(load_elem_status) { load(elem_dir, lbl_dir, load_cnt, padding); }
    mnist(const ch_str elem_dir, const ch_str lbl_dir, net_set<uint64_t> &&lbl_load_distribute, uint64_t load_elem_status = mnist_elem_orgn, uint64_t padding = 0) :
        elem_status(load_elem_status) { load(elem_dir, lbl_dir, std::move(lbl_load_distribute), padding); }
    
    uint64_t size() const { return elem.length; }

    uint64_t ln_cnt() const { return elem_ln_cnt; }

    uint64_t col_cnt() const { return elem_col_cnt; }

    bool dataset_verify() const { return elem.length == lbl.length; }

    bool load(const ch_str elem_dir, const ch_str lbl_dir, uint64_t load_cnt = 0, uint64_t padding = 0) {
        if (!preprocess(elem_dir, lbl_dir)) {
            close_stream();
            return false;
        }
        long long *idx_arr = nullptr;
        if (load_cnt) idx_arr = num_rand<long long>(0, elem_cnt, 8);
        else load_cnt = elem_cnt;
        elem.init(load_cnt);
        lbl.init(load_cnt);
        data_idx.init(load_cnt);
        for (auto i = 0ull; i < load_cnt; ++i) data_idx[i] = i;
        auto cnt = 0;
        for (auto i = 0ull; i < elem_cnt; ++i) if ((idx_arr && *(idx_arr + cnt) == i) || !idx_arr) {
            elem[cnt] = read_curr_elem(true, padding);
            lbl[cnt]  = read_curr_lbl();
            ++cnt;
            if (cnt == load_cnt) break;
        } else {
            read_curr_elem(false);
            read_curr_lbl();
        }
        close_stream();
        ptr_reset(idx_arr);
        return true;
    }

    bool load(const ch_str elem_dir, const ch_str lbl_dir, net_set<uint64_t> &&lbl_load_distribute, uint64_t padding = 0) {
        if (!preprocess(elem_dir, lbl_dir)) {
            close_stream();
            return false;
        }
        uint64_t cnt = mnist_orgn_size,
                 idx = 0;
        net_set<uint64_t> load_qnty(mnist_orgn_size);
        if (lbl_load_distribute.length == 1) {
            load_qnty.fill_with(lbl_load_distribute[0]);
            elem.init(mnist_orgn_size * lbl_load_distribute[0]);
            lbl.init(elem.length);
            data_idx.init(elem.length);
            for (auto i = 0ull; i < elem.length; ++i) data_idx[i] = i;
            lbl_load_distribute.reset();
        }
        else if (lbl_load_distribute.length == 10) load_qnty = std::move(lbl_load_distribute);
        else return load(elem_dir, lbl_dir);
        for (auto i = 0ull; i < elem_cnt; ++i) {
            auto load_sgn = true;
            auto curr_lbl = read_curr_lbl();
            if (load_qnty[curr_lbl]) {
                --load_qnty[curr_lbl];
                if (load_qnty[curr_lbl] == 0) --cnt;
            } else {
                read_curr_elem(false);
                continue;
            }
            elem[idx] = read_curr_elem(true, padding);
            lbl[idx]  = curr_lbl;
            ++idx;
            if (cnt == 0) break;
        }
        close_stream();
        return true;
    }

    bool save_as_bitmap(const ch_str dir_root, uint64_t ex_name, char backslash = '\\') {
        if (elem.length == 0) return false;
        auto save_flag     = true;
        auto w_dir_root    = str_charset_exchange(dir_root);
        auto dir_backslash = L'/';
        if (backslash == '\\') dir_backslash = L'\\';
        for (auto i = 0ull; i < elem.length; ++i) {
            auto name = L'[' + std::to_wstring(i) + L']' + std::to_wstring(lbl[i]);
            bmio::bmio_bitmap raw;
            for (auto j = 0; j < bmio_rgb; ++j) {
                raw[j] = bmio::bmio_chann(elem_ln_cnt, elem_col_cnt);
                for (auto k = 0ull; k < raw[j].element_count ; ++k) {
                    if (elem[i].index(k)) raw[j].index(k) = 255;
                    else raw[j].index(k) = 0;
                }
            }
            if (!bmio::gdi_save_bitmap(raw, w_dir_root, name.c_str(), ex_name, dir_backslash)) {
                save_flag = false;
                break;
            }
        }
        return save_flag;
    }

    void reset() {
        elem_ln_cnt  = 0;
        elem_col_cnt = 0;
        elem_cnt     = 0;

        lbl.reset();
        elem.reset();
    }

    ~mnist() { reset(); }

private:

    std::ifstream strm_elem,
                  strm_lbl;

    uint64_t elem_ln_cnt  = 0, 
             elem_col_cnt = 0,
             elem_cnt     = 0;

    const uint64_t elem_status;

public:
    net_set<vect> elem;

    net_set<uint64_t> lbl;

    net_set<uint64_t> data_idx;

    __declspec(property(get=size))           uint64_t element_count;
    __declspec(property(get=ln_cnt))         uint64_t element_line_count;
    __declspec(property(get=col_cnt))        uint64_t element_column_count;
    __declspec(property(get=dataset_verify)) bool     verify;

    bool operator==(const mnist &val) const { return elem_status == val.elem_status && elem_ln_cnt == val.elem_ln_cnt && elem_col_cnt == val.elem_col_cnt && elem_cnt == val.elem_cnt && lbl == val.lbl && elem == val.elem; }
    
    bool operator!=(const mnist &val) const { return !(*this == val); }

    mnist &operator=(const mnist &src) {
        net_assert(elem_status == src.elem_status,
                   "mnist",
                   "=",
                   "Element status should be equal.");
        value_copy(src);
        return *this;
    }
    mnist &operator=(mnist &&src) {
        net_assert(elem_status == src.elem_status,
                   "mnist",
                   "=",
                   "Element status should be equal.");
        value_move(std::move(src));
        return *this;
    }
};

DATASET_END