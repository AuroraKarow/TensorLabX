NEUNET_BEGIN

struct net_chrono_epoch { long long begin = 0, dur = 0; };

struct net_chrono final {
private:
    bool id_verify(uint64_t id) const {
        if (id >= id_gen) return false;
        for (auto temp : blank_id) if (temp == id) return false;
        return true;
    }

    void value_copy(const net_chrono &src) {
        id_gen     = src.id_gen;
        chrono_log = src.chrono_log;
        blank_id   = src.blank_id;
    }

    void value_move(net_chrono &&src) {
        id_gen     = src.id_gen;
        chrono_log = std::move(src.chrono_log);
        blank_id   = std::move(src.blank_id);
    }

public:
    net_chrono(uint64_t buffer_size = 128) :
        chrono_log(buffer_size) {}
    net_chrono(net_chrono &&src) { value_move(std::move(src)); }
    net_chrono(const net_chrono &src) { value_copy(src); }
    
    uint64_t timer_begin() {
        net_chrono_epoch temp {NEUNET_CHRONO_TIME_POINT, 0};
        auto id = id_gen;
        if (blank_id.length) {
            id = blank_id.erase(0);
            chrono_log[id] = temp;
        }
        else {
            ++id_gen;
            if (id_gen == chrono_log.length) chrono_log.init(chrono_log.length << 1);
            chrono_log[id] = temp;
        }
        return id;
    }

    bool timer_end(uint64_t id) {
        auto temp = NEUNET_CHRONO_TIME_POINT;
        if (!id_verify(id)) return false;
        chrono_log[id].dur = temp - chrono_log[id].begin;
        return true;
    }

    long long duration(uint64_t id) const {
        if (!id_verify(id)) return -1;
        else return chrono_log[id].dur;
    }

    bool erase(uint64_t id) {
        if (!id_verify(id)) return false;
        else return blank_id.insert(0, id);
    }

    void reset() {
        id_gen = 0;
        chrono_log.reset();
        blank_id.reset();
    }
    
private:
    uint64_t id_gen = 0;

    net_set<net_chrono_epoch> chrono_log;
    net_list<uint64_t>        blank_id;
};

net_chrono glb_timer;

NEUNET_END