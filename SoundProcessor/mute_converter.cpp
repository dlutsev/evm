#include "mute_converter.h"
#include "converter.h"

using namespace std;

namespace sound_processor {

    void mute_converter::convert(int16_t* block, size_t number_of_blocks, size_t block_begin) {
        if ((block_begin >= mute_end && 0 != mute_end) || block_begin + number_of_blocks < mute_begin)
            return;
        size_t i = mute_begin < block_begin ? 0 : mute_begin - block_begin;
        for (; i < number_of_blocks && (i < mute_end - block_begin || 0 == mute_end); ++i)
            block[i] = 0;
    }

    shared_ptr<converter> mute_converter_creator::parse(
        const std::vector<int>& iargs,
        const std::vector<std::string>& files,
        const std::vector<size_t>& file_indexes
    ) const {
        if (iargs.size() > 2 || file_indexes.size() != 0)
            throw config_failure("Bad number of arguments for mute converter");
        int beg = iargs.size() >= 1 ? iargs[0] : 0;
        int end = iargs.size() == 2 ? iargs[1] : 0;
        if (beg < 0 || end < 0)
            throw config_failure("Bad arguments for mute converter");
        return make_shared<mute_converter>(
            static_cast<size_t>(beg) * SAMPLE_RATE,
            static_cast<size_t>(end) * SAMPLE_RATE
        );
    }

    const char* mute_converter_creator::conv_help() const {
        return "mute converter: "
            "Mutes stream from previous converter at interval\n"
            "configuration: mute <begin>[=0] <end>[=0]\n";
    }

    const char* mute_converter_creator::conv_name() const { return "mute"; }

}