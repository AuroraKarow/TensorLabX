BMIO_BEGIN

bool chann_shape_verify(const bmio_bitmap fst, const bmio_bitmap snd) {
    for (auto i = 0ull; i < bmio_rgba; ++i) if (!fst[i].shape_verify(snd[i])) return false;
    return true;
}

bool chann_verify(const bmio_bitmap src) {
    if (src[bmio_r].shape_verify(src[bmio_g]) && src[bmio_g].shape_verify(src[bmio_b]))
        if (src[bmio_a].verify) return src[bmio_b].shape_verify(src[bmio_a]);
        else return true;
    else return false;
}

bool chann_init(bmio_bitmap dest, uint64_t ln_cnt, uint64_t col_cnt, bool alpha = false) {
    if (ln_cnt && col_cnt) {
        auto chann_len = bmio_rgb;
        if (alpha) chann_len = bmio_rgba;
        for (auto i = 0; i < chann_len; ++i) dest[i] = vect(ln_cnt, col_cnt);
        return true;
    } else return false;
}

void chann_refresh(bmio_bitmap src) { for (auto i = 0; i < bmio_rgba; ++i) if (src[i].verify) src[i].fill_elem(0); }

bool gdi_load_bitmap(bmio_bitmap dest, const wchar_t *dir, bool alpha = false) {
    ULONG_PTR gph_token;
    auto st_gph = new Gdiplus::GdiplusStartupInput;
    auto flag   = Gdiplus::GdiplusStartup(&gph_token, st_gph, nullptr) == Gdiplus::Ok;
    if (flag) {
        auto map_ptr = new Gdiplus::Bitmap(dir);
        auto curr_px = new Gdiplus::Color;
        chann_init(dest, map_ptr->GetHeight(), map_ptr->GetWidth(), alpha);
        for (auto i = 0ull; i<map_ptr->GetHeight(); ++i) for (auto j = 0ull; j<map_ptr->GetWidth(); ++j) if (map_ptr->GetPixel(j, i, curr_px) == Gdiplus::Status::Ok) {
            dest[bmio_r][i][j] = (long double)curr_px->GetRed();
            dest[bmio_g][i][j] = curr_px->GetGreen();
            dest[bmio_b][i][j] = curr_px->GetBlue();
            if(alpha) dest[bmio_a][i][j] = curr_px->GetAlpha();
        } else { 
            flag = false;
            break;
        }
        while (curr_px) { delete curr_px; curr_px = nullptr; }
        while (map_ptr) { delete map_ptr; map_ptr = nullptr; }
    }
    Gdiplus::GdiplusShutdown(gph_token);
    while (st_gph) { delete st_gph; st_gph = nullptr; }
    return flag;
}

bool gdi_CLSID_encode(const wchar_t *format, CLSID *p_CLSID) {
    // Number of image encoders
    uint32_t encode_num = 0,
    // Size of the image encoder array in bytes
             _size      = 0;
    Gdiplus::GetImageEncodersSize(&encode_num, &_size);
    if (_size) {
        auto img_code_info_ptr = ptr_init<Gdiplus::ImageCodecInfo>(_size);
        if (img_code_info_ptr) {
            Gdiplus::GetImageEncoders(encode_num, _size, img_code_info_ptr);
            for (auto i = 0ull; i < encode_num; ++i) {
                if (!wcscmp(img_code_info_ptr[i].MimeType, format)) {
                    *p_CLSID = img_code_info_ptr[i].Clsid;
                    ptr_reset(img_code_info_ptr);
                    return true;
                }
            }
        }
        ptr_reset(img_code_info_ptr);
    }
    return false;
}

// Need to startup the GDI environment
std::wstring gdi_extensive_name(uint64_t ex_name, CLSID *p_CLSID) {
    std::wstring ans = L"",
                 f_n = L"";
    switch (ex_name) {
    case bmio_png: f_n = L"image/png";  ans = L".png"; break;
    case bmio_gif: f_n = L"image/gif";  ans = L".gif"; break;
    case bmio_bmp: f_n = L"image/bmp";  ans = L".bmp"; break;
    case bmio_jpg: f_n = L"image/jpeg"; ans = L".jpg"; break;
    case bmio_tif: f_n = L"image/tiff"; ans = L".tiff"; break;
    default: ans = L""; break;
    }
    if (f_n != L"" && gdi_CLSID_encode(f_n.c_str(), p_CLSID)) return ans;
    else return L"";
}

bool gdi_save_bitmap(const bmio_bitmap src, const wchar_t *dir_root, const wchar_t *name, uint64_t ex_name, wchar_t backslash = L'\\') {
    if (!chann_verify(src)) return false;
    ULONG_PTR gph_token;
    auto st_gph = new Gdiplus::GdiplusStartupInput;
    auto flag   = Gdiplus::GdiplusStartup(&gph_token, st_gph, nullptr) == Gdiplus::Ok && st_gph;
    if (flag) {
        auto color_form = PixelFormat32bppRGB;
        if (src[bmio_a].verify) color_form = PixelFormat32bppARGB;
        auto map_ptr = new Gdiplus::Bitmap(src[bmio_r].column_count, src[bmio_r].line_count, color_form);
        if (map_ptr) {
            Gdiplus::Graphics gph_img(map_ptr);
            for (auto i = 0ull; i < src[bmio_r].line_count && flag; ++i) for (auto j = 0ull; j < src[bmio_r].column_count; ++j) {
                Gdiplus::Pen curr_pen(Gdiplus::Color(0, 0, 0));
                if (src[bmio_a].verify) curr_pen.SetColor(Gdiplus::Color(src[bmio_a][i][j], src[bmio_r][i][j], src[bmio_g][i][j], src[bmio_b][i][j]));
                else curr_pen.SetColor(Gdiplus::Color(src[bmio_r][i][j], src[bmio_g][i][j], src[bmio_b][i][j]));
                flag = gph_img.DrawLine(&curr_pen, (int)j, (int)i, (int)(j + 1), (int)(i + 1)) == Gdiplus::Status::Ok;
                if (!flag) break;
            }
            if (flag) {
                auto         p_clsid   = new CLSID;
                std::wstring path      = dir_root, 
                             file_name = name + gdi_extensive_name(ex_name, p_clsid);
                if (backslash == L'\\') path += L'\\' + file_name;
                else path += L'/' + file_name;
                flag = Gdiplus::Status::Ok == map_ptr->Save(path.c_str(), p_clsid);
                while (p_clsid) { delete p_clsid; p_clsid = nullptr; }
            }
        } else flag = false;
        while (map_ptr) { delete map_ptr; map_ptr = nullptr; }
    }
    Gdiplus::GdiplusShutdown(gph_token);
    while (st_gph) { delete st_gph; st_gph = nullptr; }
    return flag;
}

BMIO_END