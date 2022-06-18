#pragma once

#include <iostream>
#include "dataset"

using std::cout;
using std::endl;
using std::string;

using mtx::matrix;
using mtx::vect;
using mtx::vect_dec;

using bagrt::decimal;
using bagrt::net_list;
using bagrt::net_map;
using bagrt::net_set;

using namespace bmio;

struct elem_data
{
    int val = 0; double sup = 0;
    elem_data(int src = 0, double src_sup = 0) : val(src), sup(src_sup) {}
    bool operator==(elem_data &src) { return src.val==val && src.sup==sup; }
    bool operator!=(elem_data &src) { return !(*this == src); }
    friend std::ostream &operator<<(std::ostream &output, const elem_data &src)
    {
        output << '(' << src.val << ", " << src.sup << ')';
        return output;
    }
};

uint64_t hash_elem_data(elem_data &&src) { return HASH_FLOAT(std::move(src.sup)) + HASH_INTEGER(std::move(src.val)) * 2; }



int main(int argc, char *argv[], char *envp[])
{
    CLOCK_DECLARE(uint64_t, clk, HASH_INTEGER)
    CLOCK_BEGIN(clk, 0)

    
    decimal::calculate_digit = 5;
    decimal a = 1.70352;
    cout << a << endl;
    bitmap bm("src\\tamago.png");
    cout << "*.png -> *.bmp, *.jpg" << endl;
    cout << "HEIGHT - " << bm.height << ", WIDTH - " << bm.width << endl;
    bm.save_img("src", "tamago_bmp", BMIO_BMP);
    bm.save_img("src", "tamago_jpg", BMIO_JPG);


    CLOCK_END(clk, 0)
    cout << CLOCK_DURATION(clk, 0) << "ms" << endl;
    return EXIT_SUCCESS;
}