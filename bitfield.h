#ifndef __BITFIELD_H__
#define __BITFIELD_H__
struct bitfield {
    // assumption: we never have more than 128 nets.  we can get this from  
    unsigned long long bits[2];
    int size;

    bool get(int net_num) {
        assert(net_num < 128);
        unsigned long long chunk = net_num/64;
        unsigned long long rem = net_num%64;
        unsigned long long mask = (1ULL<<rem);
        if ((bits[chunk] & mask) > 0ULL) {
            return true;
        }
        return false;
    };
    void set(int net_num) {
        assert(net_num < 128);
        unsigned long long chunk = net_num/64;
        unsigned long long rem = net_num%64;
        unsigned long long mask = (1ULL<<rem);
        if ((bits[chunk] & mask) == 0ULL) {
            ++size;
        }
        bits[chunk] |= mask;
    };
    void clear(int net_num) {
        assert(net_num < 128);
        unsigned long long chunk = net_num/64;
        unsigned long long rem = net_num%64;
        unsigned long long mask = ~(1ULL<<rem);
        if ((bits[chunk] & mask) > 0ULL) {
            --size;
        }
        bits[chunk] &= mask;
    };

    bitfield union_with(bitfield& other) {
        bitfield result;
        for(int i = 0; i < 2; ++i) {
            result.bits[i] = bits[i] | other.bits[i];
        }
        // fix size
        result.size = 0;
        for (int i = 0; i < 128; ++i) {
            if (result.get(i)) {
                result.size++;
            }
        }
        return result;
    };

    bitfield() {size=0; memset(bits, 0, sizeof(bits));};
    bitfield(bitfield *other) {
        memcpy(bits, other->bits, sizeof(bits));
        size = other->size;
    };

    std::vector<int> to_vec() {
        std::vector<int> ret;
        for(int i = 0; i < 128; ++i) {
            if (get(i)) {
                ret.push_back(i);
            }
        }
        return ret;
    };
};
#endif
