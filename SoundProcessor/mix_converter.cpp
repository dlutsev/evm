#include "mix_converter.h"
#include "converter.h"

using namespace std;

namespace sound_processor {

    void mix_converter::convert(int16_t* block, size_t number_of_blocks, size_t block_begin) {
        int16_t* mix_block = new int16_t[number_of_blocks];
        size_t mix_size = mixing_stream.read(mix_block, number_of_blocks);
        if ((block_begin >= end && 0 != end) || block_begin + number_of_blocks < beg)
            return;
        size_t i = beg < block_begin ? 0 : beg - block_begin;
        for (; i < number_of_blocks && (i < end - block_begin || 0 == end); ++i) {
            int16_t avg = block[i];
            if (i < mix_size) {
                avg /= 2;
                avg += mix_block[i] / 2;
            }
            block[i] = avg;
        }

        delete[] mix_block;
    }

    shared_ptr<converter> mix_converter_creator::parse(
        const std::vector<int>& iargs,
        const std::vector<std::string>& files,
        const std::vector<size_t>& fileidxs
    ) const {
        if (iargs.size() > 2 || fileidxs.size() != 1)
            throw config_failure("Bad arguments for mix converter");
        int beg = iargs.size() >= 1 ? iargs[0] : 0;
        int end = iargs.size() == 2 ? iargs[1] : 0;
        if (beg < 0 || end < 0)
            throw config_failure("Bad arguments for mix converter");
        input_sample_stream iss{ files[fileidxs[0]] };
        return make_shared<mix_converter>(
            iss,
            static_cast<size_t>(beg) * SAMPLE_RATE,
            static_cast<size_t>(end) * SAMPLE_RATE
        );
    }

    const char* mix_converter_creator::conv_name() const { return "mix"; }
    const char* mix_converter_creator::conv_help() const {
        return "mix converter: "
            "Mixes stream from previous converter with input stream\n"
            "configuration: mix $<stream number> <begin>[=0] <end>[=0]\n";
    }

}