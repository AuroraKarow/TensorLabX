BMIO_BEGIN

BMIO_CALLBACK bool GDI_load_bitmap(BMIO_CHANN ans[BMIO_RGBA_CNT], std::wstring dir, bool alpha = false);
BMIO_CALLBACK bool GDI_save_bitmap(BMIO_CHANN src[BMIO_RGBA_CNT], std::wstring dir_root, std::wstring name, uint64_t ex_name, bool alpha = false, wchar_t div_syb = L'\\');
BMIO_CALLBACK bool GDI_chann_valid(BMIO_CHANN src[BMIO_RGBA_CNT], bool alpha = false);
BMIO_CALLBACK bool GDI_chann_init(BMIO_CHANN src[BMIO_RGBA_CNT], uint64_t ln_cnt, uint64_t col_cnt, bool alpha = false);

template<typename __mtx_elem, typename __mtx_elem_v> class _bitmap final
{
private:
    BMIO_CHANN rgba[BMIO_RGBA_CNT];
    struct px
    {
        __mtx_elem &r, &g, &b, &a;
        px(__mtx_elem &__r, __mtx_elem &__g, __mtx_elem &__b, __mtx_elem &__a);
        friend std::ostream &operator<<(std::ostream &output, px &src)
        {
            output << "[R " << src.r << ']' << std::endl;
            output << "[G " << src.g << ']' << std::endl;
            output << "[B " << src.b << ']' << std::endl;
            output << "[A " << src.a << ']';
            return output;
        }
    };
    struct _bitmap_width
    {
    public:
        _bitmap_width(_bitmap *__ptr_src = nullptr, uint64_t __ln = 0, uint64_t __col_cnt = 0);
        px operator[](uint64_t col);
        ~_bitmap_width();
    private:
        _bitmap *__ptr;
        uint64_t ln = 0, col_cnt = 0;
    };
public:
    bool value_copy(_bitmap &src);
    bool value_move(_bitmap &&src);
    _bitmap(_bitmap &src);
    _bitmap(const _bitmap &src);
    _bitmap(_bitmap &&src);
    _bitmap(BMIO_CHANN src[BMIO_RGBA_CNT]);
    _bitmap(uint64_t ln_cnt = 0, uint64_t col_cnt = 0, bool alpha = false);
    _bitmap(std::wstring dir, bool alpha = false);
    _bitmap(std::string dir, bool alpha = false);
    bool init(uint64_t ln_cnt, uint64_t col_cnt, bool alpha = false);
    bool init(BMIO_CHANN src[BMIO_RGBA_CNT]);
    uint64_t get_ln_cnt();
    uint64_t get_col_cnt();
    BMIO_CHANN get_gray();
    _BAGRT net_set<BMIO_CHANN> get_raw();
    bool is_bitmap();
    bool load_img(std::wstring dir, bool alpha = false);
    bool load_img(std::string dir, bool alpha = false);
    bool save_img(std::wstring dir_root, std::wstring name, uint64_t ex_name, bool alpha = false, wchar_t div_syb = L'\\');
    bool save_img(std::string dir_root, std::string name, uint64_t ex_name, bool alpha = false, char div_syb = '\\');
    bool operator==(_bitmap &val);
    bool operator!=(_bitmap &val);
    void operator=(_bitmap &src);
    void operator=(const _bitmap &src);
    void operator=(_bitmap &&src);
    _bitmap_width operator[](uint64_t ln);
    void reset();
    ~_bitmap();
private:
    bool refresh_chann(uint64_t _chann);
    bool refresh_chann();
    bool __value_copy(const _bitmap &src);
public:
    __declspec (property (get=get_ln_cnt)) uint64_t height;
    __declspec (property (get=get_col_cnt)) uint64_t width;
    __declspec (property (get=get_gray)) BMIO_CHANN gray;
    __declspec (property (get=get_raw)) _BAGRT net_set<BMIO_CHANN> raw;   
    static _BAGRT net_set<BMIO_CHANN> gray_grad(BMIO_CHANN &grad_vec);
};

BMIO_END

#include "bmio.hpp"