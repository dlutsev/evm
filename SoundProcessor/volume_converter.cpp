#include <cmath>
#include "volume_converter.h"
#include "converter.h"

using namespace std;

namespace sound_processor {
    void volume_converter::convert(int16_t* block, size_t number_of_blocks, size_t block_begin) {
        if ((block_begin >= end && 0 != end) || block_begin + number_of_blocks < begin)
            return;
        double vol = volume;
        vol /= 100;
        size_t i = begin < block_begin ? 0 : begin - block_begin;
        for (; i < number_of_blocks && (i < end - block_begin || 0 == end); ++i) {
            double new_value = block[i];
            new_value *= vol;
            if (block[i] < 0)
                block[i] = (new_value >= INT16_MIN) ? static_cast<int16_t>(new_value) : INT16_MIN;
            else
                block[i] = (new_value <= INT16_MAX) ? static_cast<int16_t>(new_value) : INT16_MAX;
        }
    }

    shared_ptr<converter> volume_converter_creator::parse(
        const std::vector<int>& iargs,
        const std::vector<std::string>& files,
        const std::vector<size_t>& fileidxs
    ) const {
        if (iargs.size() < 1 || iargs.size() > 3 || fileidxs.size() != 0)
            throw config_failure("Bad number of arguments for volume converter");
        int vol = iargs[0];
        int beg = iargs.size() >= 2 ? iargs[1] : 0;
        int end = iargs.size() == 3 ? iargs[2] : 0;
        if (beg < 0 || end < 0)
            throw config_failure("Bad arguments for volume converter");
        return make_shared<volume_converter>(
            static_cast<size_t>(vol),
            static_cast<size_t>(beg) * SAMPLE_RATE,
            static_cast<size_t>(end) * SAMPLE_RATE
        );
    }

    const char* volume_converter_creator::conv_help() const {
        return "volume converter: "
            "Multiplying samples in interval by a <volume>/100.\n"
            "configuration: vol <volume> <begin>[=0] <end>[=0]\n";
    }

    const char* volume_converter_creator::conv_name() const { return "vol"; }

}