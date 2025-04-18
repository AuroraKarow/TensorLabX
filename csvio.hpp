CSVIO_BEGIN

net_set<net_set<std::string>> input_table(const std::string &file_path) {
    std::ifstream in(file_path);
    net_set<net_set<std::string>> ans;
    if (!in.is_open()) {
        std::cerr << "Error opening file";
        in.close();
        return ans;
    }
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string dat(buffer.str());
    in.close();
    ans.init(std::count(dat.begin(), dat.end(), '\n') + 1);
    auto from = dat.begin(),
         to   = from;
    for (auto i = 0ull; i < ans.length; ++i) {
        for (; to != dat.end(); ++to) if (*to == '\n') break;
        ans[i].init(std::count(from, to, ',') + 1);
        auto j = 0ull;
        for (; from != to; ++from) if (*from == ',') ++j;
        else ans[i][j].push_back(*from);
        ++from;
        ++to;
    }
    return ans;
}

void output_table(const net_set<net_set<std::string>> &output_strings, const std::string &file_path) {
    std::ofstream of_file;
    of_file.open(file_path, std::ios::out|std::ios::trunc);
    for(auto i = 0ull; i < output_strings.size(); ++i) {
        for(auto j=0ull; j<output_strings[i].size(); ++j) of_file << output_strings[i][j] << ',';
        of_file << std::endl;
    }
    of_file.close();
}

CSVIO_END