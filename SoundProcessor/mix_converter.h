#pragma once
#include "processor.h"

namespace sound_processor {

    class mix_converter : public converter {
    public:
        inline mix_converter(const input_sample_stream& mixing, size_t b = 0, size_t e = 0)
            : beg(b), end(e),
            mixing_stream(mixing) {  }
        virtual void convert(int16_t*, size_t, size_t);
        virtual ~mix_converter() noexcept {}
    private:
        size_t beg;
        size_t end;
        input_sample_stream mixing_stream;
    };

    class mix_converter_creator : public convector_creator {
        virtual std::shared_ptr<converter> parse(
            const std::vector<int>& iargs,
            const std::vector<std::string>& streams,
            const std::vector<size_t>& stream_indexes
        ) const;
        virtual const char* conv_name() const;
        virtual const char* conv_help() const;
    };

}