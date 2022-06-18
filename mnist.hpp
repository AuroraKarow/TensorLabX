DATASET_BEGIN

inline bool mnist::open_stream(std::string elem_dir, std::string lbl_dir)
{
    strm_elem = std::ifstream(elem_dir, std::ios::in | std::ios::binary);
    strm_lbl = std::ifstream(lbl_dir, std::ios::in | std::ios::binary);
    return strm_elem.is_open() && strm_lbl.is_open();
}
inline void mnist::close_stream() { strm_elem.close(); strm_lbl.close(); }
inline bool mnist::magic_verify()
{
    uint32_t magic = 0;
    // data
    strm_elem.read(reinterpret_cast<char*>(&magic), sizeof(uint32_t));
    auto dat_magic = _BAGRT num_swap_endian(magic);
    if (dat_magic != MNIST_MAGIC_ELEM) return false;
    magic = 0;
    // label
    strm_lbl.read(reinterpret_cast<char*>(&magic), sizeof(uint32_t));
    auto lbl_magic = _BAGRT num_swap_endian(magic);
    if (lbl_magic != MNIST_MAGIC_LBL) return false;
    return true;
}
inline uint64_t mnist::load_elem_cnt()
{
    uint32_t dat_cnt = 0, lbl_cnt = 0;
    strm_elem.read(reinterpret_cast<char*>(&dat_cnt), sizeof(uint32_t));
    auto shrk_dat_cnt = _BAGRT num_swap_endian(dat_cnt);
    strm_lbl.read(reinterpret_cast<char*>(&lbl_cnt), sizeof(uint32_t));
    auto shrk_lbl_cnt = _BAGRT num_swap_endian(lbl_cnt);
    if(shrk_dat_cnt == shrk_lbl_cnt) return shrk_lbl_cnt;
    else return 0;
}
inline uint64_t mnist::load_elem_shape()
{
    uint32_t _temp = 0;
    strm_elem.read(reinterpret_cast<char*>(&_temp), sizeof(uint32_t));
    return _BAGRT num_swap_endian(_temp);
}
inline vect mnist::read_curr_elem(bool w_flag, uint64_t padding, bool gray)
{
    auto data_length = elem_ln_cnt * elem_col_cnt;
    MEM_INIT(char, data_ptr, data_length);
    strm_elem.read(data_ptr, data_length);
    vect vec_data;
    if(w_flag)
    {
        vec_data = vect(elem_ln_cnt, elem_col_cnt);
        for(auto i=0; i<data_length; ++i)
        {
            int curr_pt = data_ptr[i];
            if(curr_pt) switch (elem_status)
            {
            case MNIST_ELEM_ORGN: vec_data.pos_idx(i) = curr_pt; break;
            case MNIST_ELEM_BOOL: vec_data.pos_idx(i) = 1; break;
            case MNIST_ELEM_GRAY: vec_data.pos_idx(i) = 255; break;
            default: MEM_RECYCLE(data_ptr); return false;
            }
            else vec_data.pos_idx(i) = 0;
        }
        if(padding) vec_data = vec_data.pad(padding, padding, padding, padding);
        vec_data.reshape(data_length, 1);
    }
    MEM_RECYCLE(data_ptr);
    return vec_data;
}
inline uint64_t mnist::read_curr_lbl()
{
    char label = 0;
    strm_lbl.read(&label, 1);
    return (uint64_t)label;
}
inline bool mnist::preprocess(std::string elem_dir, std::string lbl_dir)
{
    if(open_stream(elem_dir, lbl_dir) && magic_verify())
    {
        elem_cnt = load_elem_cnt();
        if(elem_cnt)
        {
            elem_ln_cnt = load_elem_shape();
            elem_col_cnt = load_elem_shape();
            return true;
        }
    }
    return false;
}
inline data_seq<uint64_t> mnist::curr_idx_set(uint64_t curr_batch_idx)
{
    if(elem_idx_seq.size())
    {
        auto curr_batch_size = batch_size;
        if(rear_batch_size && curr_batch_idx+1==batch_size) curr_batch_size = rear_batch_size;
        // Dataset shuffled indexes for current batch
        return elem_idx_seq.sub_sequence(mtx::mtx_elem_pos(curr_batch_idx, 0, batch_size), mtx::mtx_elem_pos(curr_batch_idx, curr_batch_size-1, batch_size));
    }
    else return data_seq<uint64_t>();
}

inline mnist::mnist(uint64_t load_elem_status) : elem_status(load_elem_status < 3 ? load_elem_status : MNIST_ELEM_ORGN) {}
inline mnist::mnist(std::string elem_dir, std::string lbl_dir, uint64_t load_cnt, uint64_t load_elem_status, uint64_t padding) : elem_status(load_elem_status < 3 ? load_elem_status : MNIST_ELEM_ORGN) { assert(load_elem_lbl(elem_dir, lbl_dir, load_cnt, padding)); }
inline mnist::mnist(std::string elem_dir, std::string lbl_dir, std::initializer_list<uint64_t> load_cnt_list, uint64_t load_elem_status, uint64_t padding) : elem_status(load_elem_status < 3 ? load_elem_status : MNIST_ELEM_ORGN) { assert(load_elem_lbl(elem_dir, lbl_dir, load_cnt_list, padding)); }
inline bool mnist::load_elem_lbl(std::string elem_dir, std::string lbl_dir, uint64_t load_cnt, uint64_t padding)
{
    auto load_flag = true;
    if(preprocess(elem_dir, lbl_dir))
    {
        data_seq<uint64_t> lbl_data_stat;
        if(load_cnt) lbl_data_stat = bagrt::rand_idx(elem_cnt, load_cnt);
        else load_cnt = elem_cnt;
        elem.init(load_cnt); lbl.init(load_cnt);
        for(auto i=0,j=0; j<load_cnt; ++i) if(i==lbl_data_stat[j] || !lbl_data_stat.size())
        {
            elem[j] = read_curr_elem(true, padding);
            lbl[j] = read_curr_lbl();
            ++ j;
        }
        else
        {
            read_curr_elem(false);
            read_curr_lbl();
        }
    }
    else load_flag = false;
    close_stream();
    return load_flag;
}
inline bool mnist::load_elem_lbl(std::string elem_dir, std::string lbl_dir, data_seq<uint64_t> load_cnt_seq, uint64_t padding)
{
    auto pcdr_flag = true;
    if(preprocess(elem_dir, lbl_dir))
    {
        auto data_cnt = 0, load_cnt = 0;
        if(load_cnt_seq.size() == 1)
        {
            auto sgl_cnt = load_cnt_seq[IDX_ZERO];
            load_cnt = sgl_cnt * MNIST_ORGN_SIZE;
            load_cnt_seq.init(MNIST_ORGN_SIZE);
            for(auto i=0; i<MNIST_ORGN_SIZE; ++i) load_cnt_seq[i] = sgl_cnt;
        }
        else if(load_cnt_seq.size() == MNIST_ORGN_SIZE) load_cnt = load_cnt_seq.sum();
        else pcdr_flag = false;
        if(pcdr_flag)
        {
            elem.init(load_cnt);
            lbl.init(load_cnt);
            auto check = MNIST_ORGN_SIZE;
            while(check)
            {
                auto curr_lbl = read_curr_lbl();
                if(load_cnt_seq[curr_lbl])
                {
                    elem[data_cnt] = read_curr_elem(true, padding, true);
                    lbl[data_cnt] = curr_lbl;
                    ++ data_cnt;
                    -- load_cnt_seq[curr_lbl];
                    if(!load_cnt_seq[curr_lbl]) -- check;
                }
                else read_curr_elem(false);
            }
        }
        else pcdr_flag = false;
    }
    else pcdr_flag = false;
    close_stream();
    return pcdr_flag;
}
inline bool mnist::verify_dataset() { return elem.length == lbl.length; }
inline uint64_t mnist::size() { return elem.length; }
inline uint64_t mnist::ln_cnt() { return elem_ln_cnt; }
inline uint64_t mnist::col_cnt() { return elem_col_cnt; }
inline bool mnist::init_batch(uint64_t _batch_size)
{
    if(_batch_size && verify_dataset())
    {
        batch_size = _batch_size;
        batch_cnt = elem.length / batch_size;
        rear_batch_size = elem.length % batch_size;
        if(rear_batch_size) ++ batch_cnt;
        else rear_batch_size = batch_size;
        if(batch_size > 1)
        {
            elem_idx_seq.init(elem.length);
            for(auto i=0; i<elem_idx_seq.length; ++i) elem_idx_seq[i] = i;
        }
        else
        {
            curr_batch_lbl = lbl;
            curr_batch_elem = elem;
        }
        return true;
    }
    else return false;
}
inline void mnist::shuffle() { if(elem_idx_seq.length) elem_idx_seq.shuffle(); }
inline bool mnist::init_curr_batch(uint64_t curr_batch_idx)
{
    if(elem_idx_seq.length && verify_dataset())
    {
        auto idx_set = curr_idx_set(curr_batch_idx);
        curr_batch_elem = elem.sub_sequence(idx_set);
        curr_batch_lbl = lbl.sub_sequence(idx_set);
        return true;
    }
    else return false;
}
inline bool mnist::save_elem_as_bitmap(std::string dir_root, uint64_t ex_name)
{
    if(elem_status == MNIST_ELEM_BOOL) return false;
    for(auto i=0; i<elem.length; ++i)
    {
        auto name = '[' + std::to_string(i) + ']' + std::to_string(lbl[i]);
        _BMIO bmio_chann bm_chann_temp;
        for(auto j=0; j<BMIO_RGB_CNT; ++j) bm_chann_temp[j] = elem[i].reshape(elem_ln_cnt, elem_col_cnt);
        _BMIO bitmap bm(bm_chann_temp);
        if(!bm.save_img(dir_root, name, ex_name)) return false;
    }
    return true;
}
inline vect mnist::orgn(uint64_t lbl_val)
{
    vect ans;
    if(lbl_val < MNIST_ORGN_SIZE)
    {
        ans = vect(MNIST_ORGN_SIZE, IDX_SGL);
        ans.pos_idx(lbl_val) = 1;
    }
    return ans;
}
inline data_seq<vect> mnist::orgn(data_seq<uint64_t> &lbl_set)
{
    data_seq<vect> ans(lbl_set.length);
    for(auto i=0; i<ans.length; ++i) ans[i] = orgn(lbl_set[i]);
    return ans;
}
inline void mnist::reset()
{
    elem.reset(); lbl.reset(); elem_idx_seq.reset();
    curr_batch_elem.reset(); curr_batch_lbl.reset();
    batch_size = 0; batch_cnt = 0; rear_batch_size = 0;
    elem_ln_cnt = 0; elem_col_cnt = 0; elem_cnt = 0;
}
inline mnist::~mnist() { reset(); }

DATASET_END