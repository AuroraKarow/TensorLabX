BMIO_BEGIN

class bitmap final {
private:
    struct px {
        long double &r, &g, &b, &a;
        friend std::ostream &operator<<(std::ostream &out, const px &src) {
            out << "[R " << src.r << "]\n";
            out << "[G " << src.g << "]\n";
            out << "[B " << src.b << "]\n";
            out << "[A " << src.a << ']';
            return out;
        }
    };

    struct line_data final {
    public:
        line_data(const bitmap *ptr_src = nullptr, uint64_t curr_ln = 0, uint64_t curr_col_cnt = 0) :
            ptr(ptr_src),
            ln(curr_ln),
            col_cnt(curr_col_cnt) {}
        
        px operator[](uint64_t col) {
            net_assert(col < col_cnt,
                       "bitmap::line_data",
                       "[]",
                       "Column pixel index should be less than column count.");
            return px{ptr->rgba[bmio_r][ln][col],
                      ptr->rgba[bmio_g][ln][col],
                      ptr->rgba[bmio_b][ln][col],
                      ptr->rgba[bmio_a].verify ? ptr->rgba[bmio_a][ln][col] : ptr->rgba[bmio_r][ln][col]};
        }

        ~line_data() {
            ptr     = nullptr;
            ln      = 0;
            col_cnt = 0;
        }

    private:
        const bitmap *ptr = nullptr;
        
        uint64_t ln      = 0,
                 col_cnt = 0;
    };

    void value_copy(const bitmap &src) { for (auto i = 0; i < bmio_rgba; ++i) rgba[i] = src.rgba[i]; }

    void value_move(bitmap &&src) {
        for (auto i = 0; i < bmio_rgba; ++i) {
            rgba[i] = src.rgba[i];
            src.rgba[i].reset();
        }
    }

public:
    bitmap(const bitmap &src) { value_copy(src); }
    bitmap(bitmap &&src) { value_move(std::move(src)); }
    bitmap(bmio_bitmap &&src) { for (auto i = 0; i < bmio_rgba; ++i) rgba[i] = std::move(src[i]);  }
    bitmap(uint64_t ln_cnt = 0, uint64_t col_cnt = 0, bool alpha = false) { chann_init(rgba, ln_cnt, col_cnt, alpha); }
    bitmap(const char *dir, bool alpha = false) { load(dir, alpha); }
    bitmap(const wchar_t *dir, bool alpha = false) { load(dir, alpha); }

    bool is_bitmap() const { return chann_verify(rgba) && rgba[bmio_b].verify; }

    uint64_t ln_cnt() const { return rgba[bmio_r].line_count; }

    uint64_t col_cnt() const { return rgba[bmio_r].column_count; }

    bool load(const char *dir, bool alpha = false) {
        auto w_dir = str_charset_exchange(dir);
        auto flag  = gdi_load_bitmap(rgba, w_dir, alpha);
        ptr_reset(w_dir);
        return flag;
    }
    bool load(const wchar_t *dir, bool alpha = false) { return gdi_load_bitmap(rgba, dir, alpha); }

    bool save(const char *dir_root, const char *name, uint64_t ex_name, char backslash = '\\') const {
        auto w_dir_root = str_charset_exchange(dir_root),
             w_name     = str_charset_exchange(name);
        auto flag       = gdi_save_bitmap(rgba, w_dir_root, w_name, ex_name, backslash);
        ptr_reset(w_name, w_dir_root);
        return flag;
    }
    bool save(const wchar_t *dir_root, const wchar_t *name, uint64_t ex_name, wchar_t backslash = L'\\') const { return gdi_save_bitmap(rgba, dir_root, name, ex_name, backslash); }

    bool shape_verify(const bitmap &src) const { return chann_shape_verify(rgba, src.rgba); }

    callback_matrix_n net_set<matrix::net_matrix<matrix_elem_t>> raw() const {
        net_set<matrix::net_matrix<matrix_elem_t>> ans;
        if (!is_bitmap()) return ans;
        if (__alpha_verify__()) ans.init(bmio_rgba);
        else ans.init(bmio_rgb);
        for (auto i = 0; i < ans.length; ++i) ans[i] = rgba[i];
        return ans;
    }

    void reset() { for (auto i = 0; i < bmio_rgba; ++i) rgba[i].reset(); }

    ~bitmap() { reset(); }

private:
    bmio_bitmap rgba;

public:

    bmio_chann __gray__() const {
        bmio_chann ans;
        if (is_bitmap()) ans = rgba[bmio_r] * bmio_gray_weight_r + rgba[bmio_g] * bmio_gray_weight_g + rgba[bmio_b] * bmio_gray_weight_b;
        return ans;
    }

    bool __alpha_verify__() const { return rgba[bmio_a].verify; }

    __declspec(property(get=ln_cnt))           uint64_t   height;
    __declspec(property(get=col_cnt))          uint64_t   width;
    __declspec(property(get=is_bitmap))        bool       verify;
    __declspec(property(get=__alpha_verify__)) bool       alpha_verify;
    __declspec(property(get=__gray__))         bmio_chann gray;

    bool operator==(const bitmap &src) const {
        for (auto i = 0; i < bmio_rgba; ++i) if (rgba[i] != src.rgba[i]) return false;
        return true;
    }
    bool operator!=(const bitmap &src) const { return !(*this == src); }

    line_data operator[](uint64_t ln) {
        net_assert(ln < ln_cnt(),
                   "bitmap",
                   "[]",
                   "Line pixel index should be less than line count.");
        return line_data(this, ln, col_cnt());
    }

    bitmap &operator=(const bitmap &src) {
        value_copy(src);
        return *this;
    }
    bitmap &operator=(bitmap &&src) {
        value_move(std::move(src));
        return *this;
    }

};

BMIO_END