BMIO_BEGIN

unsigned char GDI_px(long double src) { return src; }
unsigned char GDI_px(_BAGRT decimal &src) { return src.to_integer(); }

bool GDI_CLSID_encode(const wchar_t* format, CLSID* p_CLSID)
{
    // Number of image encoders
    uint32_t  encode_num = 0;
    // Size of the image encoder array in bytes
    uint32_t  _size = 0;
    Gdiplus::GetImageEncodersSize(&encode_num, &_size);
    if(_size)
    {
        MEM_INIT(Gdiplus::ImageCodecInfo, img_code_info_ptr, _size);
        if(img_code_info_ptr)
        {
            Gdiplus::GetImageEncoders(encode_num, _size, img_code_info_ptr);
            for(auto i=0; i<encode_num; ++i)
            {
                if(!wcscmp(img_code_info_ptr[i].MimeType, format))
                {
                    *p_CLSID = img_code_info_ptr[i].Clsid;
                    MEM_RECYCLE(img_code_info_ptr);
                    return true;
                }
            }
        }
        MEM_RECYCLE(img_code_info_ptr);
    }
    return false;
}

BMIO_CALLBACK bool GDI_chann_valid(BMIO_CHANN src[BMIO_RGBA_CNT], bool alpha)
{
    if(src[BMIO_R].shape_valid(src[BMIO_G]) && src[BMIO_R].shape_valid(src[BMIO_B]))
    {
        if(alpha && !src[BMIO_B].shape_valid(src[BMIO_A])) return false;
        return true;
    }
    else return false;
}

BMIO_CALLBACK bool GDI_chann_init(BMIO_CHANN src[BMIO_RGBA_CNT], uint64_t ln_cnt, uint64_t col_cnt, bool alpha)
{
    if(ln_cnt && col_cnt)
    {
        src[BMIO_R] = BMIO_CHANN(ln_cnt, col_cnt);
        src[BMIO_G] = BMIO_CHANN(ln_cnt, col_cnt);
        src[BMIO_B] = BMIO_CHANN(ln_cnt, col_cnt);
        if(alpha) src[BMIO_A] = BMIO_CHANN(ln_cnt, col_cnt);
        return true;
    }
    else return false;    
}

BMIO_CALLBACK bool GDI_load_bitmap(BMIO_CHANN ans[BMIO_RGBA_CNT], std::wstring dir, bool alpha)
{
    ULONG_PTR gph_token;
    auto st_gph = new Gdiplus::GdiplusStartupInput;
    auto flag = Gdiplus::GdiplusStartup(&gph_token, st_gph, nullptr) == Gdiplus::Ok;
    if(flag)
    {
        auto map_ptr = new Gdiplus::Bitmap(dir.c_str());
        auto curr_px = new Gdiplus::Color;
        GDI_chann_init(ans, map_ptr->GetHeight(), map_ptr->GetWidth(), alpha);
        for(uint64_t i=0; i<map_ptr->GetHeight(); ++i) for(uint64_t j=0; j<map_ptr->GetWidth(); ++j)
            if(map_ptr->GetPixel(j, i, curr_px) == Gdiplus::Status::Ok)
            {
                ans[BMIO_R][i][j] = curr_px->GetRed();
                ans[BMIO_G][i][j] = curr_px->GetGreen();
                ans[BMIO_B][i][j] = curr_px->GetBlue();
                if(alpha) ans[BMIO_A][i][j] = curr_px->GetAlpha();
            }
            else { flag = false; break; }
        delete curr_px; curr_px = nullptr;
        delete map_ptr; map_ptr = nullptr;
    }
    Gdiplus::GdiplusShutdown(gph_token);
    delete st_gph; st_gph = nullptr;
    return flag;
}

BMIO_CALLBACK bool GDI_save_bitmap(BMIO_CHANN src[BMIO_RGBA_CNT], std::wstring dir_root, std::wstring name, uint64_t ex_name, bool alpha, wchar_t div_syb)
{
    if(GDI_chann_valid(src, alpha))
    {
        if(alpha && !src[BMIO_R].shape_valid(src[BMIO_A])) return false;
        ULONG_PTR gph_token;
        auto st_gph = new Gdiplus::GdiplusStartupInput;
        auto flag = Gdiplus::GdiplusStartup(&gph_token, st_gph, nullptr) == Gdiplus::Ok && st_gph;
        if(flag)
        {
            auto color_form = PixelFormat32bppRGB;
            if(alpha) color_form = PixelFormat32bppARGB;
            auto map_ptr = new Gdiplus::Bitmap(src[BMIO_R].column_count, src[BMIO_R].line_count, color_form);
            if(map_ptr)
            {
                Gdiplus::Graphics gph_img(map_ptr);
                for(auto i=0; i<src[BMIO_R].line_count && flag; ++i) for(auto j=0; j<src[BMIO_R].column_count; ++j)
                {
                    if(alpha) flag = gph_img.DrawLine(&Gdiplus::Pen(Gdiplus::Color(GDI_px(src[BMIO_A][i][j]), GDI_px(src[BMIO_R][i][j]), GDI_px(src[BMIO_G][i][j]), GDI_px(src[BMIO_B][i][j]))), j, i, j+1, i+1) == Gdiplus::Status::Ok;
                    else flag = gph_img.DrawLine(&Gdiplus::Pen(Gdiplus::Color(GDI_px(src[BMIO_R][i][j]), GDI_px(src[BMIO_G][i][j]), GDI_px(src[BMIO_B][i][j]))), j, i, j+1, i+1) == Gdiplus::Status::Ok;
                    if(!flag) break;
                }
                if(flag)
                {
                    auto p_CID_STR = new CLSID;
                    std::wstring s_ex_name = L"";
                    switch (ex_name)
                    {
                    case BMIO_PNG: if(GDI_CLSID_encode(L"image/png", p_CID_STR)) s_ex_name = L".png"; else flag =  false; break;
                    case BMIO_JPG: if(GDI_CLSID_encode(L"image/jpeg", p_CID_STR) && !alpha) s_ex_name = L".jpg"; else flag = false; break;
                    case BMIO_GIF: if(GDI_CLSID_encode(L"image/gif", p_CID_STR) && !alpha) s_ex_name = L".gif"; else flag = false; break;
                    case BMIO_TIF: if(GDI_CLSID_encode(L"image/tiff", p_CID_STR)) s_ex_name = L".tiff"; else flag = false; break;
                    case BMIO_BMP: if(GDI_CLSID_encode(L"image/bmp", p_CID_STR) && !alpha) s_ex_name = L".bmp"; else flag = false; break;
                    default: flag = false; break;
                    }
                    if(flag)
                    {
                        std::wstring path = dir_root, file_name = name + s_ex_name;
                        if(div_syb == L'\\') path += L'\\' + file_name;
                        else path += L'/' + file_name;
                        flag = Gdiplus::Status::Ok == map_ptr->Save(path.c_str(), p_CID_STR);
                    }
                    delete p_CID_STR; p_CID_STR = nullptr;
                }
            }
            else flag = false;
            delete map_ptr; map_ptr = nullptr;
        }
        Gdiplus::GdiplusShutdown(gph_token);
        delete st_gph; st_gph = nullptr;
        return flag;
    }
    else return false;
}

BMIO_CALLBACK_ inline bool BMIO_T::refresh_chann(uint64_t _chann) { return rgba[_chann].fill_with(std::move(__mtx_elem(0))); }
BMIO_CALLBACK_ inline bool BMIO_T::refresh_chann()
{
    auto flag = refresh_chann(BMIO_R) && refresh_chann(BMIO_G) && refresh_chann(BMIO_R);
    if(rgba[BMIO_A].is_matrix()) flag = flag && refresh_chann(BMIO_A);
    return flag;
}
BMIO_CALLBACK_ inline bool BMIO_T::__value_copy(const _bitmap &src)
{
    if(src.is_bitmap())
    {
        rgba[BMIO_R] = src.rgba[BMIO_R];
        rgba[BMIO_G] = src.rgba[BMIO_G];
        rgba[BMIO_B] = src.rgba[BMIO_B];
        rgba[BMIO_A] = src.rgba[BMIO_A];
        return true;
    }
    else return false;
}
BMIO_CALLBACK_ inline bool BMIO_T::value_copy(_bitmap &src) { __value_copy(src); }
BMIO_CALLBACK_ inline bool BMIO_T::value_move(_bitmap &&src) { return init(src.rgba); }
BMIO_CALLBACK_ inline BMIO_T::_bitmap(_bitmap &src) { value_copy(src); }
BMIO_CALLBACK_ inline BMIO_T::_bitmap(const _bitmap &src) { __value_copy(src); }
BMIO_CALLBACK_ inline BMIO_T::_bitmap(_bitmap &&src) { value_move(std::move(src)); }
BMIO_CALLBACK_ inline BMIO_T::_bitmap(BMIO_CHANN src[BMIO_RGBA_CNT]) { assert(init(src)); }
BMIO_CALLBACK_ inline BMIO_T::_bitmap(uint64_t ln_cnt, uint64_t col_cnt, bool alpha) { if(ln_cnt && col_cnt) init(ln_cnt, col_cnt, alpha); }
BMIO_CALLBACK_ inline BMIO_T::_bitmap(std::wstring dir, bool alpha) { load_img(dir, alpha); }
BMIO_CALLBACK_ inline BMIO_T::_bitmap(std::string dir, bool alpha) { load_img(dir, alpha); }
BMIO_CALLBACK_ inline bool BMIO_T::init(uint64_t ln_cnt, uint64_t col_cnt, bool alpha) { return GDI_chann_init(rgba, ln_cnt, col_cnt, alpha); }
BMIO_CALLBACK_ inline bool BMIO_T::init(BMIO_CHANN src[BMIO_RGBA_CNT])
{
    if(GDI_chann_valid(src))
    {
        for(auto i=0; i<BMIO_RGBA_CNT; ++i) rgba[i] = src[i];
        return true;
    }
    else return false;
}
BMIO_CALLBACK_ inline uint64_t BMIO_T::get_ln_cnt() { return rgba[BMIO_R].line_count; }
BMIO_CALLBACK_ inline uint64_t BMIO_T::get_col_cnt() { return rgba[BMIO_R].column_count; }
BMIO_CALLBACK_ inline BMIO_CHANN BMIO_T::get_gray()
{
    if(is_bitmap()) return (BMIO_GRAY_WEIGHT_R * rgba[BMIO_R] + BMIO_GRAY_WEIGHT_G * rgba[BMIO_G] + BMIO_GRAY_WEIGHT_B * rgba[BMIO_B]);
    else return BMIO_CHANN_BLANK;
}
BMIO_CALLBACK_ inline _BAGRT net_set<BMIO_CHANN> BMIO_T::get_raw()
{
    _BAGRT net_set<BMIO_CHANN> ans;
    if(is_bitmap)
    {
        ans.init(rgba[BMIO_A].is_matrix()?BMIO_RGBA_CNT:BMIO_RGB_CNT);
        for(auto i=0; i<ans.length; ++i) ans[i] = rgba[i];
    }
    return ans;
}
BMIO_CALLBACK_ inline bool BMIO_T::is_bitmap() { return GDI_chann_valid(rgba, rgba[BMIO_A].is_matrix()); }
BMIO_CALLBACK_ inline bool BMIO_T::load_img(std::wstring dir, bool alpha) { return GDI_load_bitmap(rgba, dir, alpha); }
BMIO_CALLBACK_ inline bool BMIO_T::load_img(std::string dir, bool alpha) { return load_img(_BAGRT charset_exchange(dir), alpha); }
BMIO_CALLBACK_ inline bool BMIO_T::save_img(std::wstring dir_root, std::wstring name, uint64_t ex_name, bool alpha, wchar_t div_syb) { return GDI_save_bitmap(rgba, dir_root, name, ex_name, alpha, div_syb); }
BMIO_CALLBACK_ inline bool BMIO_T::save_img(std::string dir_root, std::string name, uint64_t ex_name, bool alpha, char div_syb)
{
    string temp = "";
    temp.push_back(div_syb);
    auto w_temp = _BAGRT charset_exchange(temp);
    auto w_div_syb = w_temp.back();
    return GDI_save_bitmap(rgba, _BAGRT charset_exchange(dir_root), _BAGRT charset_exchange(name), ex_name, alpha, w_div_syb);
}
BMIO_CALLBACK_ inline bool BMIO_T::operator==(_bitmap &val)
{
    auto flag = (rgba[BMIO_R]==val.rgba[BMIO_R]) && 
                (rgba[BMIO_G]==val.rgba[BMIO_G]) && 
                (rgba[BMIO_B]==val.rgba[BMIO_B]);
    if(rgba[BMIO_A].is_matrix()) flag = flag && (rgba[BMIO_A]==val.rgba[BMIO_A]);
    return flag;
}
BMIO_CALLBACK_ inline bool BMIO_T::operator!=(_bitmap &val) { return !(*this == val); }
BMIO_CALLBACK_ inline void BMIO_T::operator=(_bitmap &src) { value_copy(src); }
BMIO_CALLBACK_ inline void BMIO_T::operator=(const _bitmap &src) { __value_copy(src); }
BMIO_CALLBACK_ inline void BMIO_T::operator=(_bitmap &&src) { value_move(std::move(src)); }
BMIO_CALLBACK_ inline typename BMIO_T::_bitmap_width BMIO_T::operator[](uint64_t ln)
{
    assert(ln < rgba[BMIO_R].line_count);
    return _bitmap_width(this, ln, rgba[BMIO_R].column_count);
}
BMIO_CALLBACK_ inline void BMIO_T::reset() { for(auto i=0; i<BMIO_RGBA_CNT; ++i) rgba[i].reset(); }
BMIO_CALLBACK_ inline BMIO_T::~_bitmap() { reset(); }
BMIO_CALLBACK_ inline _BAGRT net_set<BMIO_CHANN> BMIO_T::gray_grad(BMIO_CHANN &grad_vec)
{
    _BAGRT net_set<BMIO_CHANN> ans;
    if(grad_vec.is_matrix())
    {
        ans.init(BMIO_RGB_CNT);
        ans[BMIO_R] = grad_vec * BMIO_GRAY_WEIGHT_R;
        ans[BMIO_G] = grad_vec * BMIO_GRAY_WEIGHT_G;
        ans[BMIO_B] = grad_vec * BMIO_GRAY_WEIGHT_B;
    }
    return ans;
}
BMIO_CALLBACK_ inline BMIO_T::px::px(__mtx_elem &__r, __mtx_elem &__g, __mtx_elem &__b, __mtx_elem &__a) : r(__r), g(__g), b(__b), a(__a) {}
BMIO_CALLBACK_ inline BMIO_T::_bitmap_width::_bitmap_width(_bitmap *__ptr_src, uint64_t __ln, uint64_t __col_cnt) : __ptr(__ptr_src), ln(__ln), col_cnt(__col_cnt) {}
BMIO_CALLBACK_ inline typename BMIO_T::px BMIO_T::_bitmap_width::operator[](uint64_t col)
{
    assert(col < col_cnt);
    __mtx_elem a_temp = 0;
    px ans(__ptr->rgba[BMIO_R][ln][col],
            __ptr->rgba[BMIO_G][ln][col],
            __ptr->rgba[BMIO_B][ln][col],
            __ptr->rgba[BMIO_A].is_matrix()?__ptr->rgba[BMIO_A][ln][col]:a_temp);
    return ans;
}
BMIO_CALLBACK_ inline BMIO_T::_bitmap_width::~_bitmap_width() { __ptr = nullptr; ln = 0; col_cnt = 0; }

BMIO_END