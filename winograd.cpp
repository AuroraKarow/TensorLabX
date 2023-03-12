#pragma once
#include "async"
#include "string"
#include "matrix"
#include "net_decimal"

template<typename arg> using seq = neunet::net_sequence<arg>;

using decimal = neunet::net_decimal;

using namespace neunet::async;
using neunet::vect;

bool isZero(seq<decimal> &D);
seq<decimal> cutZero(seq<decimal> &aim);
void tellMuls(seq<decimal> &A, seq<decimal> &B);
vect lineSum(vect &input);

seq<seq<decimal>> modX(uint64_t Size, seq<decimal> D = seq<decimal>(0, 0), decimal value = 1);
seq<decimal> ModuloInverse(seq<decimal> &_Data_U, seq<decimal> &_Data_V);
seq<seq<seq<decimal>>> OutAG(seq<decimal> &_base, uint64_t a, uint64_t g);
seq<decimal> OutB(seq<decimal> &_base, uint64_t p);
vect svChange(seq<decimal> &_matrix, uint64_t ln_s);
seq<vect> OutWinoVect(uint64_t data_ls, uint64_t conv_ls);
vect multiMatrix(seq<vect> &_multiABG, vect &_data, vect & _kernel, uint64_t data_ls, uint64_t ker_ls);
vect ConvWinograd(seq<vect> &_multiABG, vect &_Data, vect &_Kernel);
vect chaI(vect a);


// class polynomial{

//     polynomial(polynomial &src);
//     polynomial(polynomial &&src);
//     void operator=(polynomial &src);
//     void operator=(polynomial &&src);
//     polynomial();
//     polynomial(seq<decimal> inp0, seq<decimal> inp1);
//     void clean_ploy();
//     void reset();
//     ~polynomial();

//     seq<seq<decimal>> show_Idata();
//     seq<seq<decimal>> show_idata();
//     seq<seq<decimal>> show_tdata();
//     seq<decimal> show_Inp();
//     seq<decimal> show_inp();
//     void enter_tdata(seq<decimal> &T);
//     void change_Inp(seq<decimal> &data);
//     void change_inp(seq<decimal> &data);
//     void C_pg();
//     void C_IM();
//     void C_IM(seq<decimal> cha);
//     void C_iM();
//     void C_iM(seq<decimal> cha);
//     void C_Id();
//     void C_Id(seq<decimal> cha);
//     void C_id();
//     void C_id(seq<decimal> cha);
//     void C_Im();
//     void C_Im(seq<decimal> cha);
//     void C_im();
//     void C_im(seq<decimal> cha);

//     seq<decimal> plus_poly();
//     seq<decimal> plus_poly(seq<decimal> &_inp0, seq<decimal> &_inp1);
//     seq<decimal> minus_poly();
//     seq<decimal> minus_poly(seq<decimal> &_inp0, seq<decimal> &_inp1);
//     seq<decimal> multi_poly();
//     seq<decimal> multi_poly(seq<decimal> &_inp0, seq<decimal> &_inp1);
//     seq<decimal> divide_poly();
//     seq<decimal> divide_poly(seq<decimal> &_inp0, seq<decimal> &_inp1);
//     seq<decimal> mod_poly();
//     seq<decimal> mod_poly(seq<decimal> &_inp0, seq<decimal> &_inp1);
//     seq<decimal> pai_poly(uint64_t P, seq<decimal> &_Id, seq<decimal> id = seq<decimal>(0, 0));
//     seq<decimal> sigma_poly(uint64_t P, seq<decimal> &_Id, seq<decimal> &_base, seq<decimal> id = {});
//     seq<decimal> pow_poly(seq<decimal> id, int64_t pow);

// };



bool isZero(seq<decimal> &D){
    bool tell = 1;
    int a = D.size();
    while(a){
        a--;
        if((int)D[a]){
            tell = 0;
            break;
        }
    }
    return tell;
}

seq<decimal> cutZero(seq<decimal> &aim){
    uint64_t a = 0;
    seq<decimal> out;
    while(aim[aim.size() - a - 1] == 0 && a != aim.size()){ a++; }
    out.init(aim.size() - a);
    for(uint64_t p = 0; p < out.size(); p++){ out[p] = aim[p]; }
    return out;
}

void tellMuls(seq<decimal> &A, seq<decimal> &B){
    if(A.size() < B.size()){
        seq<decimal> t = A;
        A = B;
        B = t;
    }
}

vect lineSum(vect &input){
    vect out(input.line_count, 1);
    for(auto m = 0; m < input.line_count; m++){ out[m][0] = input.elem_sum(m, m, 0, input.column_count - 1, 0, 0); }
    return out;
}

class polynomial{

private:

    seq<decimal> Inp, inp, outp, outm, outmt, outd, outmd, outl, zero;
    seq<seq<decimal>> Idata, idata, tdata;

    /*vect indata, Data_t, Out, Ker;
    uint64_t kl, kc, ks, cnl, Dl, Dc, C, Ol;
    seq<uint64_t> size, Ks;
    

    uint64_t pt(uint64_t S, uint64_t e, uint64_t b){
        if(S > pow(b, e)){ return ++e; }
        return e;
    }

    vect intc(vect c){
        c *= 10;
        for(uint64_t i = 0; i < c.ELEM_CNT; i++){ c.pos_idx(i) = (int)c.pos_idx(i); }
        return c;
    }*/


public:

    polynomial(polynomial &src) { *this = src; }
    polynomial(polynomial &&src) { *this = std::move(src); }

    void operator=(polynomial &src) { Inp = src.Inp; inp = src.inp; outp = src.outp; outm = src.outm; outmt = src.outmt; outd = src.outd; outmd = src.outmd; outl = src.outl; Idata = src.Idata; idata = src.idata; tdata = src.tdata; }

    void operator=(polynomial &&src) { Inp = std::move(src.Inp); inp = std::move(src.inp); outp = std::move(src.outp); outm = std::move(src.outm); outmt = std::move(src.outmt); outd = std::move(src.outd); outmd = std::move(src.outmd); outl = std::move(src.outl); Idata = std::move(src.Idata); idata = std::move(src.idata); tdata = std::move(src.tdata); }

    polynomial(){}

    polynomial(seq<decimal> inp0, seq<decimal> inp1){
        zero.emplace_back(0);
        inp0 = cutZero(inp0);
        inp1 = cutZero(inp1);
        Idata.emplace_back(inp0);
        idata.emplace_back(inp1);
        Inp = inp0;
        inp = inp1;
    }
        
    void clean_ploy(){
        Idata.init(0);
        idata.init(0);
        tdata.init(0);
        Inp.init(0);
        inp.init(0);
    }

    seq<seq<decimal>> show_Idata(){ return Idata; }

    seq<seq<decimal>> show_idata(){ return idata; }
    
    seq<seq<decimal>> show_tdata(){ return tdata; }
    
    seq<decimal> show_Inp(){ return Inp; }
    
    seq<decimal> show_inp(){ return inp; }

    void enter_tdata(seq<decimal> &T){ tdata.emplace_back(T); }

    void change_Inp(seq<decimal> &data){ 
        Inp = data; 
        Idata.emplace_back(data);
    }
    
    void change_inp(seq<decimal> &data){ 
        inp = data; 
        idata.emplace_back(data);
    }

    seq<decimal> plus_poly(){
        outp.init(Inp.size() > inp.size() ? Inp.size() : inp.size());
        for(uint64_t a = 0; a < (Inp.size() < inp.size() ? Inp.size() : inp.size()); a++){ outp[a] = Inp[a] + inp[a]; } 
        for(uint64_t a = (Inp.size() < inp.size() ? Inp.size() : inp.size()); a < outp.size(); a++){ outp[a] += (Inp.size() < inp.size() ? inp[a] : Inp[a]); }
        return outp;
    }

    seq<decimal> plus_poly(seq<decimal> &_inp0, seq<decimal> &_inp1){
        seq<decimal> inp0  = _inp0, inp1 = _inp1;
        inp0 = cutZero(inp0);
        inp1 = cutZero(inp1);
        outp.init(inp0.size() > inp1.size() ? inp0.size() : inp1.size());
        for(uint64_t a = 0; a < (inp0.size() < inp1.size() ? inp0.size() : inp1.size()); a++){ outp[a] = inp0[a] + inp1[a]; } 
        for(uint64_t a = (inp0.size() < inp1.size() ? inp0.size() : inp1.size()); a < outp.size(); a++){ outp[a] += (inp0.size() < inp1.size() ? inp1[a] : inp0[a]); }
        return outp;
    }

    seq<decimal> minus_poly(){
        outp.init(Inp.size() > inp.size() ? Inp.size() : inp.size());
        for(uint64_t a = 0; a < (Inp.size() < inp.size() ? Inp.size() : inp.size()); a++){ outp[a] = Inp[a] + inp[a]; } 
        for(uint64_t a = (Inp.size() < inp.size() ? Inp.size() : inp.size()); a < outp.size(); a++){ outp[a] += (Inp.size() < inp.size() ? inp[a] : Inp[a]); }
        return outp;
    }

    seq<decimal> minus_poly(seq<decimal> &_inp0, seq<decimal> &_inp1){
        seq<decimal> inp0  = _inp0, inp1 = _inp1;
        inp0 = cutZero(inp0);
        inp1 = cutZero(inp1);
        outp.init(inp0.size() > inp1.size() ? inp0.size() : inp1.size());
        for(uint64_t a = 0; a < (inp0.size() < inp1.size() ? inp0.size() : inp1.size()); a++){ outp[a] = inp0[a] - inp1[a]; } 
        for(uint64_t a = (inp0.size() < inp1.size() ? inp0.size() : inp1.size()); a < outp.size(); a++){ outp[a] += (inp0.size() < inp1.size() ? -inp1[a] : inp0[a]); }
        return outp;
    }

    seq<decimal> multi_poly(){
        Inp = cutZero(Inp);
        inp = cutZero(inp);
        outmt.init(Inp.size() + inp.size() - 1);
        for(int a = 0; a < Inp.size(); a++){
            for(int b = 0; b < inp.size(); b++){
                outmt[a + b] += Inp[a] * inp[b];
            }
        }
        return outmt;
    }

    seq<decimal> multi_poly(seq<decimal> &_inp0, seq<decimal> &_inp1){
        seq<decimal> inp0  = _inp0, inp1 = _inp1;
        inp0 = cutZero(inp0);
        inp1 = cutZero(inp1);
        outmt.init(inp0.size() + inp1.size() - 1);
        for(int a = 0; a < inp0.size(); a++){
            for(int b = 0; b < inp1.size(); b++){
                outmt[a + b] += inp0[a] * inp1[b];
            }
        }
        return outmt;
    }

    seq<decimal> divide_poly(){
        Inp = cutZero(Inp);
        inp = cutZero(inp);
        if(isZero(inp)){ return zero; }
        outd.init(Inp.size() - inp.size() + 1);
        for(int a = 0; a < outd.size(); a++){
            outd[outd.size() - a - 1] = Inp[Inp.size() - a - 1] / inp[inp.size() - 1];
            for(int b = 1; b <= inp.size(); b++){
                Inp[Inp.size() - a - b] -= outd[outd.size() - a - 1] * inp[inp.size() - b];
            }
        }
        Inp = Idata[Idata.size() - 1];
        return outd;
    }

    seq<decimal> divide_poly(seq<decimal> &_inp0, seq<decimal> &_inp1){
        seq<decimal> inp0  = _inp0, inp1 = _inp1;
        inp0 = cutZero(inp0);
        inp1 = cutZero(inp1);
        if(isZero(inp1)){ return zero; }
        outd.init(inp0.size() - inp1.size() + 1);
        for(int a = 0; a < outd.size(); a++){
            outd[outd.size() - a - 1] = inp0[inp0.size() - a - 1] / inp1[inp1.size() - 1];
            for(int b = 1; b <= inp1.size(); b++){
                inp0[inp0.size() - a - b] -= outd[outd.size() - a - 1] * inp1[inp1.size() - b];
            }
        }
        return outd;
    }

    seq<decimal> mod_poly(){
        Inp = cutZero(Inp);
        inp = cutZero(inp);
        if(isZero(inp)){ return zero; }
        outd.init(Inp.size() - inp.size() + 1);
        outmd.init(inp.size() - 1);
        for(int a = 0; a < outd.size(); a++){
            outd[Inp.size() - inp.size() - a] = Inp[Inp.size() - a - 1] / inp[inp.size() - 1];
            for(int b = 1; b <= inp.size(); b++){
                Inp[Inp.size() - a - b] -= outd[Inp.size() - inp.size() - a] * inp[inp.size() - b];
            }
        }
        for(uint64_t c = 0; c < outmd.size(); c++){ outmd[c] = Inp[c]; }
        outmd = cutZero(outmd);
        Inp = Idata[Idata.size() - 1];
        return outmd;
    }

    seq<decimal> mod_poly(seq<decimal> &_inp0, seq<decimal> &_inp1){
        seq<decimal> inp0  = _inp0, inp1 = _inp1;
        inp0 = cutZero(inp0);
        inp1 = cutZero(inp1);
        if(isZero(inp1)){ return zero; }
        outd.init(inp0.size() - inp1.size() + 1);
        outmd.init(inp1.size() - 1);
        for(int a = 0; a < outd.size(); a++){
            outd[inp0.size() - inp1.size() - a] = inp0[inp0.size() - a - 1] / inp1[inp1.size() - 1];
            for(int b = 1; b <= inp1.size(); b++){
                inp0[inp0.size() - a - b] -= outd[inp0.size() - inp1.size() - a] * inp1[inp1.size() - b];
            }
        }
        for(uint64_t c = 0; c < outmd.size(); c++){ outmd[c] = Inp[c]; }
        outmd = cutZero(outmd);
        return outmd;
    }
    
    seq<decimal> pai_poly(uint64_t P, seq<decimal> &_Id, seq<decimal> id = seq<decimal>(0, 0)){
        seq<decimal> Id = _Id;
        if(id.size() == 0){ 
            id = Id;
            P -= 2;
        }
        Id[0]++;
        if(P == 1){ return multi_poly(Id, id); }
        return pai_poly(--P, Id, multi_poly(Id, id)); 
    }

    seq<decimal> sigma_poly(uint64_t P, seq<decimal> &_Id, seq<decimal> &_base, seq<decimal> id = {}){
        seq<decimal> Id = _Id, base = _base;
        if(P == 0){ return id; }
        if(id.size() == 0){ id.init(base.size()); }
        seq<decimal> ID = Id;
        ID[0]++;
        return sigma_poly(--P, ID, base, plus_poly(divide_poly(base, Id), id)); 
    }

    seq<decimal> pow_poly(seq<decimal> id, int64_t pow){}

    void C_pg(){
        seq<decimal> a  = Inp;
        Inp = inp;
        inp = a;
        Idata.emplace_back(Inp);
        idata.emplace_back(inp); 
    }

    void C_IM(){ 
        Inp = multi_poly(); 
        Idata.emplace_back(Inp);
    }

    void C_IM(seq<decimal> cha){ 
        Inp = multi_poly(); 
        change_inp(cha);
        Idata.emplace_back(Inp);
    }

    void C_iM(){ 
        inp = multi_poly(); 
        idata.emplace_back(inp);
    }

    void C_iM(seq<decimal> cha){ 
        inp = multi_poly();
        change_Inp(cha);
        idata.emplace_back(inp);
    }

    void C_Id(){ 
        Inp = divide_poly(); 
        Idata.emplace_back(Inp);
    }

    void C_Id(seq<decimal> cha){ 
        Inp = divide_poly(); 
        change_Inp(cha);
        Idata.emplace_back(Inp);
    }

    void C_id(){ 
        inp = divide_poly(); 
        idata.emplace_back(inp);
    }

    void C_id(seq<decimal> cha){ 
        inp = divide_poly(); 
        change_Inp(cha);
        idata.emplace_back(inp);
    }

    void C_Im(){ 
        Inp = mod_poly(); 
        Idata.emplace_back(Inp);
    }

    void C_Im(seq<decimal> cha){ 
        Inp = mod_poly(); 
        change_inp(cha);
        Idata.emplace_back(Inp);
    }

    void C_im(){ 
        inp = mod_poly(); 
        idata.emplace_back(inp);
    }

    void C_im(seq<decimal> cha){ 
        inp = mod_poly(); 
        change_Inp(cha);
        idata.emplace_back(inp);
    }

    void reset()
    {
        Inp.reset(); inp.reset(); outp.reset(); outm.reset(); outmt.reset(); outd.reset(); outmd.reset(); outl.reset(); Idata.reset(); idata.reset(); tdata.reset();
    }

    ~polynomial(){ reset(); }

    /*vect Cal(vect D, vect K, uint64_t E, vect DD = {}, vect DK = {}, int c = -1){
        //std::cout << std::endl << "D:" << std::endl << D << std::endl << "K:" << std::endl << K << std::endl << "DD:" << std::endl << DD << std::endl << "DK:" << std::endl << DK << std::endl << "COUNT:" << std::endl << c << std::endl;
        if(E == 0){ return D.elem_cal_opt(K, MATRIX_ELEM_MULT); }
        else{
            seq<vect> M;
            M.init(4);
            E--;
            //std::cout << "POW:" << pow(3, E) << std::endl << std::endl;
            //std::cout << "Child: " << std::endl << D.child(0, pow(2, E) - 1, 0, pow(3, E) - 1, 0, 0) << std::endl << D.child(0, pow(2, E) - 1, 2 * pow(3, E), 3 * pow(3, E) - 1, 0, 0) << std::endl << D.child(0, pow(2, E) - 1, 0, pow(3, E) - 1, 0, 0) - D.child(0, pow(2, E) - 1, 2 * pow(3, E), 3 * pow(3, E) - 1, 0, 0) << std::endl;
            M[0] = Cal(D.child(0, pow(2, E) - 1, 0, pow(3, E) - 1, 0, 0) - D.child(0, pow(2, E) - 1, 2 * pow(3, E), 3 * pow(3, E) - 1, 0, 0), K.child(0, pow(3, E) - 1, 0, 0, 0, 0), E, D, K, 0);
            M[1] = Cal(D.child(0, pow(2, E) - 1, pow(3, E), 2 * pow(3, E) - 1, 0, 0) + D.child(0, pow(2, E) - 1, 2 * pow(3, E), 3 * pow(3, E) - 1, 0, 0), (K.child(0, pow(3, E) - 1, 0, 0, 0, 0) + K.child(pow(3, E), 2 * pow(3, E) - 1, 0, 0, 0, 0) + K.child(2 * pow(3, E), 3 * pow(3, E) - 1, 0, 0, 0, 0)).elem_cal_opt(2, MATRIX_ELEM_DIV), E, D, K, 1);
            M[2] = Cal(D.child(0, pow(2, E) - 1, 2 * pow(3, E), 3 * pow(3, E) - 1, 0, 0) - D.child(0, pow(2, E) - 1, pow(3, E), 2 * pow(3, E) - 1, 0, 0), (K.child(0, pow(3, E) - 1, 0, 0, 0, 0) - K.child(pow(3, E), 2 * pow(3, E) - 1, 0, 0, 0, 0) + K.child(2 * pow(3, E), 3 * pow(3, E) - 1, 0, 0, 0, 0)).elem_cal_opt(2, MATRIX_ELEM_DIV), E, D, K, 2);
            M[3] = Cal(D.child(pow(2, E), 2 * pow(2, E) - 1, 0, pow(3, E) - 1, 0, 0) - D.child(pow(2, E), 2 * pow(2, E) - 1, 2 * pow(3, E), 3 * pow(3, E) - 1, 0, 0), K.child(2 * pow(3, E), 3 * pow(3, E) - 1, 0, 0, 0, 0), E, D, K, 3);
            //std::cout << std::endl << "E:" << std::endl << E << std::endl << "M:" << std::endl << M << std::endl;
            Out = {{M[0] + M[1] + M[2]}, {M[1] - M[2] - M[3]}};
        }
        return Out;
     }

    seq<uint64_t> Wino_cha(){
        if(!indata.is_matrix()){
            std::cout << "NO DATA!" << std::endl;
            return size;
        }
        size.init(7);
        auto a = (uint64_t)(log(kl) / log(3));
        auto b = (uint64_t)(log(kc) / log(3));
        auto c = (uint64_t)(log(Dl - kl + 1) / log(2));
        auto d = (uint64_t)(log(Dc - kc + 1) / log(2));
        a = pt(kl * kc, (uint64_t)((a + b) / 2) + (a + b) % 2, 9);
        c = pt((Dl - kl + 1) * (Dc - kc + 1), (uint64_t)((c + d) / 2) + (c + d) % 2, 4);
        size[0] = a > c ? a : c;
        a = size[0];
        b = a;
        c = size[0];
        d = c;
        size[0] *= 2;
        size[1] = pow(3, a);
        size[2] = pow(3, b);
        size[3] = (pow(2, c) - 1) * size[1] / 3 + size[1];
        size[4] = (pow(2, d) - 1) * size[2] / 3 + size[2];
        size[5] = size[1] * size[2];
        size[6] = size[3] * size[4];
        return size;
    }
    
    int Wino_d(){
        if(!indata.is_matrix()){
            std::cout << "NO DATA!" << std::endl;
            return -1;
        }
        uint64_t lp(0), cp(0);
        std::cout << "Size:" << std::endl;
        std::cout << Wino_cha() << std::endl;
        vect Ker(size[1] * size[2], 1, true);
        Ker = intc(Ker);
        indata = intc(indata);
        std::cout << std::endl << "INDATA: " << std::endl << indata << std::endl;
        indata = conv::Im2ColFeaturePad(indata.reshape(C, cnl), Dl, Dc, Dl, Dc, size[3] - Dl, size[4] - Dc, 0, 0, 0, 0);
        vect data(indata.ELEM_CNT, 1, true);
        data = intc(data);
        Data_t = conv::Im2ColInputTransform(data, Ol, Dl, size[1], size[2], size[1]/3, size[2]/3, 0, 0, 0, 0, 0, 0, 0, 0);
        std::cout << "DATA:" << std::endl << Data_t.line_count << std::endl << Data_t.column_count << std::endl;
        std::cout << "K:" << std::endl << Data_t.child(0, 7, 80, 107, 0, 0).child(0, 3, 9, 17, 0, 0) << std::endl << std::endl << Data_t.child(0, 7, 80, 107, 0, 0).child(4, 7, 0, 8, 0, 0) << std::endl;
        //std::cout << Data_t << std::endl;
        vect cal = Cal(Data_t, Ker, size[0]);
        std::cout << std::endl << "CAL:" << std::endl << cal << std::endl << std::endl;
        std::cout << "CAL_Size:" << std::endl << cal.line_count << std::endl << std::endl;
        vect conv_c = conv::ConvIm2Col(Data_t, Ker);
        std::cout << "CONV_C:" << std::endl << conv_c << std::endl << std::endl;
        return 0;
    }*/
    


    /*uint64_t Numc(std::string number){
        uint64_t out(0);
        for(int a = 0; a < number.length(); a++){
            out = 10 * out + (number[a] - '0');
        }
        return out;
    }

    std::string Zip(std::string str_d){
        std::string num = "", data = "", Out = "";
        int tell = 0;
        for(int a = 0; a < str_d.length(); a++){
            if(str_d[a] == '['){
                tell = 1;
                data = "";
                continue;
            }
            if(str_d[a] == ']'){
                tell = 0;
                uint64_t len = Numc(num);
                for(uint64_t b = 0; b < len; b++){
                    for(int c = 0; c < data.length(); c++){
                        Out += data[c];
                    }
                }
                num = "";
                continue;
            }
            if(tell == 0){
                num += str_d[a];
            }
            else{
                data += str_d[a];
            }
        }
        return Out;
    }*/

};


seq<long double> minus_p(seq<long double> &inp0, seq<long double> &inp1, long double value, uint64_t p, int radix){
    int ans;
    for(int x = inp1.length - 1; x >= 0; x--){
                    ans = inp0[p + x + 1] + radix * inp0[p + x] - value * inp1[x];
                    inp0[p + x] = (int)(ans / radix);
                    inp0[p + x + 1] = (int)(ans % radix);
    }
}



seq<long double> divide_poly(seq<long double> &_inp0, seq<long double> &_inp1, int mode=0, uint64_t radix=10, uint64_t bt=0){
        seq<long double> inp0 = _inp0, inp1 = _inp1;
        seq<long double> outd;
        inp0 = cutZero(inp0);
        inp1 = cutZero(inp1);
        if(isZero(inp1)){ return zero; }
        if(mode == 0) {
            outd.init(inp0.length - inp1.length + 1);
            for(int a = 0; a < outd.length; a++){
                outd[outd.length - a - 1] = inp0[inp0.length - a - 1] / inp1[inp1.length - 1]; 
                for(int b = 1; b <= inp1.length; b++){
                    inp0[inp0.length - a - b] -= outd[outd.length - a - 1] * inp1[inp1.length - b];
                }
            }
        }
        else {
            outd.init(inp0.length - inp1.length + 1 + bt);
            inp0.insert(0, 0);
            for(int a = 0; a < outd.length; a++){
                inp0.incert(inp0.length, 0);
                outd[a] = (int)((inp0[a + 1] + radix * inp0[a]) / (inp1[0] + inp1[0] / fabs(inp1[0])));  
                if(outd[a] == 0){ continue; }
                minus_p(inp0, inp1, out[a], a, radix);
            }
            while(outd[a + 1] != 0 || fabs(inp0[a + 1]) > fabs(inp1[0])){
                outd[a] += outd[a] / fabs(outd[a]) * 2;
                minus_p(inp0, inp1, 2, a, radix);
            }
            if(inp0[a + 1] == 0){ continue; }
            int t = (int)(inp1[0] / inp0[a + 1]) / fabs((int)(inp1[0] / inp0[a + 1]));
            minus_p(inp0, inp1, outd[a] / fabs(outd[a]) * t, a, radix);
        }
}


seq<seq<decimal>> modX(uint64_t Size, seq<decimal> D, decimal value){
    seq<seq<decimal>> xdata, outx;
    xdata.init(Size);
    uint64_t tell(1), A(0);
    decimal T;
    while(A++ < Size){ xdata[A - 1].init(Size); }
    for(uint64_t a = 0; a < Size; a++){ xdata[a][a] = value; }
    if(D.size() <= Size){
        while(tell){
            for(uint64_t a = Size - 1; a >= D.size() - 1; a--){
                if(isZero(xdata[a])){ continue; }
                for(uint64_t b = 0; b < Size; b++){
                    if(xdata[a][b] == 0){ continue; }
                    for(uint64_t c = 0; c < D.size(); c++){
                        if(!c){ T = xdata[a][b]; }
                        xdata[a - c][b] -= T * D[D.size() - c - 1];
                    }
                }
                break;
            }
            tell--;
            for(uint64_t t = D.size() - 1; t < Size; t++){
                if(!isZero(xdata[t])){
                    tell++; 
                    break; 
                } 
            }
        }
    }
    for(uint64_t o = 0; o < D.size() - 1; o++){ outx.emplace_back(xdata[o]); }
    return outx;
}


seq<decimal> ModuloInverse(seq<decimal> &_Data_U, seq<decimal> &_Data_V){
    seq<decimal> Data_U(_Data_U), Data_V(_Data_V);
    tellMuls(Data_U, Data_V);
    seq<decimal>  M, T, U, V, O;
    uint64_t s(1), e(0);
    U.emplace_back(1.0);
    V.emplace_back(0.0);
    polynomial mul(Data_U, Data_V);
    M = mul.mod_poly();
    mul.enter_tdata(mul.divide_poly());
    while(!isZero(M)){
        //std::cout << "M" << std::endl << M << std::endl;
        s++;
        M = mul.show_inp();
        mul.C_im(M);
        M = mul.mod_poly();
        mul.enter_tdata(mul.divide_poly());
    }
    while(s > e){
        e++;
        T = U;
        U = V;
        V = mul.minus_poly(T, mul.multi_poly(U, mul.show_tdata()[mul.show_tdata().size() - e])); 
    }
    //std::cout << "DU:" << std::endl << Data_U << std::endl << "DV:" << std::endl << Data_V << std::endl;
    //std::cout << "U:" << std::endl << U << std::endl << "A:" << std::endl << mul->plus_poly(mul->multi_poly(U, Data_U), mul->multi_poly(V, Data_V)) << std::endl;
    O = mul.divide_poly(U, mul.plus_poly(mul.multi_poly(U, Data_U), mul.multi_poly(V, Data_V)));
    //std::cout << std::endl << "O: " << std::endl << O << std::endl;
    return O;
}


seq<seq<seq<decimal>>> OutAG(seq<decimal> &_base, uint64_t a, uint64_t g){
    seq<decimal> base = _base;
    uint64_t p = a + g - 2;
    seq<seq<decimal>> A, G;
    A.init(2);
    G.init(2);
    A[0].init(p * a + a);
    G[0].init(p * g + g);
    A[1].emplace_back(p + 1.0);
    for(uint64_t m = 0; m < p; m++){
        seq<decimal> outa = modX(a, base)[0];
        seq<decimal> outg = modX(g, base)[0];
        for(uint64_t k = 0; k < a; k++){ A[0][k + a * m] = outa[k]; }
        for(uint64_t l = 0; l < g; l++){ G[0][l + g * m] = outg[l]; }
        base[0]++;
    }
    A[0][p * a + a - 1] = 1;
    G[0][p * g + g - 1] = 1;
    seq<seq<seq<decimal>>> out;
    out.emplace_back(A);
    out.emplace_back(G);
    return out;
}


seq<decimal> OutB(seq<decimal> &_base, uint64_t p){
    seq<decimal> d, pai, B, base = _base;
    polynomial M;
    pai = M.pai_poly(p, base);
    B.init(p * p);
    for(uint64_t a = 0; a < p - 1; a++){
        d = M.divide_poly(pai, base);
        for(uint64_t b = 0; b < p - 1; b++){ B[b * p + a] = d[b] * ModuloInverse(d, base)[0]; }
        // std::cout << std::endl << "B###:" << std::endl << B[119] << std::endl;
        // std::cout << std::endl << "B###*:" << std::endl << B[118] << std::endl;
        // std::cout << std::endl << "B###**:" << std::endl << B[129] << std::endl;
        //std::cout << std::endl << "B:" << std::endl << B << std::endl;
        base[0]++;
    }
    for(uint64_t c = 0; c < p; c++){ B[c * p + p - 1] = pai[c]; }
    // std::cout << std::endl << "B###:" << std::endl << B[119] << std::endl;
    // std::cout << std::endl << "B###*:" << std::endl << B[118] << std::endl;
    return B;
}


vect svChange(seq<decimal> &_matrix, uint64_t ln_s){
    //extract_number()
    seq<decimal> matrix = _matrix;
    uint64_t col = matrix.size() / ln_s;
    vect V(ln_s, col);
    for(uint64_t l = 0; l < ln_s; l++){
        for(uint64_t c = 0; c < col; c++){
            V[l][c] = matrix[c + l * col];
        }
    }
    return V;
}


seq<vect> OutWinoVect(uint64_t data_ls, uint64_t conv_ls){
    int K = 0;
    seq<seq<seq<decimal>>> OUT;
    vect A, G, B;
    seq<vect> AGB;
    seq<decimal> Base = {0, 1};
    uint64_t num;
    uint64_t S = data_ls - conv_ls + 1;
    uint64_t s = S + conv_ls - 2;
    Base[0] -= (s - 1) / 2;
    OUT = OutAG(Base, S, conv_ls);
    num = (uint64_t)OUT[0][1][0];
    A = svChange(OUT[0][0], num);
    G = svChange(OUT[1][0], num);
    B = svChange(OutB(Base, num), num);
    //std::cout << "B" << std::endl << B << std::endl;
    AGB.emplace_back(A.transpose);
    AGB.emplace_back(B.transpose);
    AGB.emplace_back(G);
    return AGB;
}


vect multiMatrix(seq<vect> &_multiABG, vect &_data, vect & _kernel, uint64_t data_ls, uint64_t ker_ls){
    vect data = _data, kernel = _kernel;
    // std::cout << "Data:" << std::endl << data << std::endl;
    // std::cout << "Kernel:" << std::endl << kernel << std::endl;
    seq<vect> matrix, multi = _multiABG;
    data = data.transpose;
    kernel = kernel.transpose;
    uint64_t Ln_s = data_ls - ker_ls + 1;
    matrix.init(Ln_s * ker_ls);
    for(uint64_t m = 0; m < Ln_s; m++){
        for(uint64_t n = 0; n < ker_ls; n++){
            // std::cout << std::endl << m * ker_ls + n << std::endl;
            // vect A = multi[1] * data.child(0, data.line_count - 1, (m + n) * data.column_count / data_ls, (m + n + 1) * data.column_count / data_ls - 1, 0, 0), B = multi[2] * kernel.child(0, kernel.line_count - 1, n * kernel.column_count / ker_ls, (n + 1) * kernel.column_count / ker_ls - 1, 0, 0);
            // std::cout << "A:" << std::endl << A << std::endl;
            // std::cout << "B:" << std::endl << B << std::endl;
            // std::cout << "AB:" << std::endl << A.elem_cal_opt(B, MATRIX_ELEM_MULT) << std::endl;
            matrix[m * ker_ls + n] = multi[0] * (multi[1] * data.child(0, data.line_count - 1, (m + n) * data.column_count / data_ls, (m + n + 1) * data.column_count / data_ls - 1, 0, 0)).elem_wise_opt(multi[2] * kernel.child(0, kernel.line_count - 1, n * kernel.column_count / ker_ls, (n + 1) * kernel.column_count / ker_ls - 1, 0, 0), MATRIX_ELEM_MULT);
        }
    }
    return vect(matrix, Ln_s, ker_ls);
}


vect ConvWinograd(seq<vect> &_multiABG, vect &_Data, vect &_Kernel){
    uint64_t Data_ls = _Data.line_count, Ker_ls = _Kernel.line_count;
    vect convolution, Data = _Data, kernel = _Kernel;
    seq<vect> multi = _multiABG; 
    convolution = multiMatrix(multi, Data, kernel, Data_ls, Ker_ls);
    convolution = lineSum(convolution);
    return convolution;
}


vect chaI(vect a){
    for(uint64_t m = 0; m < a.line_count; m++){
        for(uint64_t n = 0; n < a.column_count; n++){
            a[m][n] = (int)(a[m][n] * 10);
        }
    }
    return a;
}








int main(){
    std::cout << "Hello World" << std::endl;
    seq<decimal> Indata, a00, b00;
    decimal a1, a2, a3, a4;
    a1 = 1;
    a2 = 3;
    a3 = a1 / a2;
    a4 = 0.33333333333333;
    //a4 *= 9;  
    seq<decimal> a, b, c;
    a = {0, 1};
    b = {-2, 1};
    c = {1, 1};
    polynomial Mul(c, a);
    //std::cout << "Ips" << std::endl << Mul -> show_Inp() << std::endl;
    Mul.C_IM(b);
    std::string s = "12354";
    std::cout << "S->Number" << std::endl << stoi(s) << std::endl; 
    // vect data, ker;
    // seq<vect> abg;
    // data = chaI(vect(19, 19, true));
    // ker = chaI(vect(12, 12, true));
    // abg = OutWinoVect(19, 12);
    // std::cout << abg << std::endl;
    // //std::cout << OutWinoVect(4, 3)[0].child(1, 1, 0, OutWinoVect(4, 3)[0].column_count - 1, 0, 0) << std::endl;
    // std::cout << std::endl << "Winograd:" << std::endl << ConvWinograd(abg, data, ker) << std::endl << std::endl;
    // std::cout << "Conv: " << std::endl << conv::Conv(data, ker, 1, 1) << std::endl;
    // set<std::future<bool>> sgn(3);
    // async_concurrent concurr(3);
    // async_batch batch(3);
    // auto func_ptr = package_function(capsulate_function<seq<vect>>(OutWinoVect), 17, 11);
    
    // for(auto i=0; i<batch.batch_size(); ++i)
    //     sgn[i] = batch.set_task(i, [&concurr](int idx)
    //     {
    //         seq<vect> abg = OutWinoVect(17 + idx, 11 + idx);
    //         vect data = chaI(vect(17 + idx, 17 + idx, true));
    //         vect ker = chaI(vect(11 + idx, 11 + idx, true));
    //         auto flag = false;
    //         concurr.batch_thread_attach();
    //         std::cout << std::endl << "Winograd:" << std::endl << ConvWinograd(abg, data, ker) << std::endl << std::endl;
    //         concurr.batch_thread_detach();
    //         return flag;
    //     }, i);
    // concurr.main_thread_deploy_batch_thread();
    //std::cout << ((0.1 + 0.2) == 0.3) << std::endl; 
    //std::cout << "Pai:" << std::endl << Mul->sigma_poly(5, b, Mul->pai_poly(4, b)) << std::endl;
    /*seq<seq<seq<decimal>>> Out = OutAG(b, 4, 3);
    std::cout << "MATRIX--A: " << Out[0][1] << std::endl;
    std::cout << svChange(Out[0][0], Out[0][1][0]) << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "MATRIX--G: " << Out[0][1] << std::endl;
    std::cout << svChange(Out[1][0], Out[0][1][0]) << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    seq<decimal> OUT = OutB(b, 6);
    std::cout << "MATRIX--B: " << OUT[1] << std::endl;
    std::cout << svChange(OUT, 6) << std::endl;
    std::cout << std::endl;*/
    //std::cout << "Divide" << std::endl << Mul->multi_poly(Mul->multi_poly(Mul->divide_poly(Mul->pai_poly(4, b), b), modX(3, b)[0]), modX(4, b)[0]) << std::endl;// << "Mod" << std::endl << Mul -> mod_poly() << std::endl; 
    /*(a00.init(3);
    b00.init(6);
    Indata.init(a00.size() + b00.size() - 2);
    std::cout << a4 << std::endl;
    std::cout << Indata << std::endl;
    //vect d = {{2,4,5,1,6,2,3},{2,4,5,1,6,2,3}};
    //std::cout << d.elem_cal_opt(2, MATRIX_ELEM_DIV) << std::endl;
    /*Indata = vect(9, 9, true);
    uint64_t ln = Indata.line_count;
    Indata = Indata.reshape(Indata.line_count * Indata.column_count, 1);*/
    //std::cout << Indata << std::endl << std::endl << Indata.child(2, 4, 0, 0, 0, 0) << std::endl;
    /*polynomial *M = new polynomial(Indata, ln, 5, 5, 1);
    std::cout << "ConvWinograd:\n" << std::endl;
    std::cout << M -> Wino_d() << std::endl;*/
    return 0;

}

