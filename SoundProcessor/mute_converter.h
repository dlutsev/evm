#pragma once
#include "processor.h"

namespace sound_processor {

    class mute_converter : public converter {
    public:
        inline mute_converter(size_t b = 0, size_t e = 0) : mute_begin(b), mute_end(e) {  }
        virtual void convert(int16_t*, size_t, size_t);
        virtual ~mute_converter() noexcept {}
    private:
        size_t mute_begin;
        size_t mute_end;
    };

    class mute_converter_creator : public convector_creator {
    public:
        virtual std::shared_ptr<converter> parse(
            const std::vector<int>& iargs,
            const std::vector<std::string>& streams,
            const std::vector<size_t>& streamIdxs
        ) const;
        virtual const char* conv_name() const;
        virtual const char* conv_help() const;
    };

}