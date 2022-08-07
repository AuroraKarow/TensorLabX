CSVIO_BEGIN

std::string parse_table(const std::string &file_path) {
    std::ifstream in(file_path);
    if (!in.is_open()) {
        std::cout << "Error opening file";
        in.close();
        return " ";
    } else {
        std::stringstream buffer;
        buffer << in.rdbuf();
        std::string dat(buffer.str());
        in.close();
        return dat;
    }
}

net_set<std::string> parse_line_strings(const std::string &strings) {
    net_sequence<std::string> out;
    std::string elem = "";
    for(auto i = 0ull; i < strings.length(); ++i) {
        char temp = strings.at(i);
        char next_temp = ' ';
        if (i + 1 != strings.length()) next_temp = strings.at(i+1);
        else next_temp = ' ';
        if (temp != ' ' && temp != '\t' && temp != '\n' && temp != '\0') elem.push_back(temp);
        else if (next_temp == ' ' || next_temp == '\t' || next_temp == '\n' || temp == '\0') continue;
        else {
            out.emplace_back(elem);
            elem = "";
        }
    }
    out.emplace_back(elem);
    out.shrink();
    return out;
}

void output_table(const net_set<net_set<std::string>> &output_strings, const std::string &file_path)
{
    std::ofstream oFile;
    oFile.open(file_path, std::ios::out|std::ios::trunc);
    for(auto i = 0ull; i < output_strings.size(); ++i) {
        for(auto j=0ull; j<output_strings[i].size(); ++j) oFile << output_strings[i][j] << ',';
        oFile << std::endl;
    }
    oFile.close();
}

net_set<net_set<std::string>> input_table(const std::string &file_path)
{
    auto tab_str = parse_table(file_path);
    auto ln_cnt  = 0ull;
    auto ln_vect = str_split(ln_cnt, tab_str.c_str(), '\n');
    net_set<net_set<std::string>> struct_tab(ln_cnt);
    for(auto i = 0ull; i < ln_cnt; ++i) {
        auto col_cnt  = 0ull;
        auto col_vect = str_split(col_cnt, ln_vect[i], ',');
        struct_tab[i].init(col_cnt);
        for (auto j = 0ull; j < col_cnt; ++j) struct_tab[i][j] = col_vect[j];
        str_arr_reset(col_vect, col_cnt);
    }
    str_arr_reset(ln_vect, ln_cnt);
    return struct_tab;
}

CSVIO_END 