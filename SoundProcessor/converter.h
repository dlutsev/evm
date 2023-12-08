#pragma once
#include <cstdint>
#include <cstddef>
#include <memory>
#include "sample_stream.h"

namespace sound_processor {

    class converter {
    public:
        virtual void convert(int16_t*, size_t, size_t) = 0;
        virtual ~converter() {}
    };

    class convector_creator {
    public:
        virtual std::shared_ptr<converter> parse(const std::vector<int>& iargs, const std::vector<std::string>& streams, const std::vector<size_t>& stream_indexes) const = 0;
        virtual const char* conv_name() const = 0;
        virtual const char* conv_help() const = 0;
    };

}