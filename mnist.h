DATASET_BEGIN

class mnist final
{
public:
    data_seq<vect>      curr_batch_elem;
    data_seq<uint64_t>  curr_batch_lbl;
    uint64_t            batch_size = 0,
                        batch_cnt = 0,
                        rear_batch_size = 0;
private:
    data_seq<vect>      elem;
    data_seq<uint64_t>  lbl,
                        elem_idx_seq;
    std::ifstream       strm_elem,
                        strm_lbl;
    uint64_t            elem_ln_cnt = 0, 
                        elem_col_cnt = 0,
                        elem_cnt = 0;
    const uint64_t      elem_status;
public:
    mnist(uint64_t load_elem_status = MNIST_ELEM_ORGN);
    mnist(std::string elem_dir, std::string lbl_dir, uint64_t load_cnt = 0, uint64_t load_elem_status = MNIST_ELEM_ORGN, uint64_t padding = 0);
    mnist(std::string elem_dir, std::string lbl_dir, std::initializer_list<uint64_t> load_cnt_list, uint64_t load_elem_status = MNIST_ELEM_ORGN, uint64_t padding = 0);
    bool load_elem_lbl(std::string elem_dir, std::string lbl_dir, uint64_t load_cnt = 0, uint64_t padding = 0);
    bool load_elem_lbl(std::string elem_dir, std::string lbl_dir, data_seq<uint64_t> load_cnt_seq, uint64_t padding = 0);
    bool verify_dataset();
    uint64_t size();
    uint64_t ln_cnt();
    uint64_t col_cnt();
    bool init_batch(uint64_t _batch_size = 1);
    void shuffle();
    bool init_curr_batch(uint64_t curr_batch_idx = 0);
    bool save_elem_as_bitmap(std::string dir_root, uint64_t ex_name = BMIO_BMP);
    static vect orgn(uint64_t lbl_val);
    static data_seq<vect> orgn(data_seq<uint64_t> &lbl_set);
    void reset();
    ~mnist();
private:
    bool open_stream(std::string elem_dir, std::string lbl_dir);
    void close_stream();
    bool magic_verify();
    uint64_t load_elem_cnt();
    uint64_t load_elem_shape();
    vect read_curr_elem(bool w_flag = true, uint64_t padding = 0, bool gray = false);
    uint64_t read_curr_lbl();
    bool preprocess(std::string elem_dir, std::string lbl_dir);
    data_seq<uint64_t> curr_idx_set(uint64_t curr_batch_idx);
public:
    __declspec(property(get=size))      uint64_t element_count;
    __declspec(property(get=ln_cnt))    uint64_t line_count;
    __declspec(property(get=col_cnt))   uint64_t column_count;
};

DATASET_END

#include "mnist.hpp"