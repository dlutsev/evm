#pragma once
#include "Processor.h"

namespace sound_processor {

    class volume_converter : public converter {
    public:
        inline volume_converter(unsigned int vol, size_t b = 0, size_t e = 0) {
            begin = b;
            end = e; 
            volume = vol;
        }
        virtual void convert(int16_t*, size_t, size_t);
        virtual ~volume_converter(){}
    private:
        unsigned int volume;
        size_t begin;
        size_t end;
    };

    class volume_converter_creator : public convector_creator {
    public:
        virtual std::shared_ptr<converter> parse(
            const std::vector<int>& iargs,
            const std::vector<std::string>& streams,
            const std::vector<size_t>& stream_indexes
        ) const;
        virtual const char* conv_name() const;
        virtual const char* conv_help() const;
    };

}
