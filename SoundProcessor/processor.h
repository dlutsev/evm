#pragma once
#include <string>
#include <vector>
#include "converter.h"
#include "sample_stream.h"

namespace sound_processor {
    const size_t SAMPLE_RATE = 44100ul;
    class config_failure : public std::exception {
    public:
        inline config_failure(const char* massage) : m(massage) {}
        inline config_failure() : m(nullptr) {}
        virtual const char* what() const noexcept {
            return m == nullptr ? "Bad configuration file" : m;
        }
    private:
        const char* m;
    };

    class config {
    public:
        using converter_vector = std::vector<std::shared_ptr<converter>>;
        config(const std::string&, const std::vector<std::string>&);
        inline const converter_vector& get_converters() const { return converters; }
        const static std::vector<convector_creator*>& creators;
    private:
        converter_vector converters;
        void read_config(std::ifstream&, const std::vector<std::string>&);
        std::shared_ptr<converter> converter_parser(
            const std::string& name,
            const std::vector<int>& iargs,
            const std::vector<std::string>& streams,
            const std::vector<size_t>& stream_indexes
        );
    };

    class processor {
    public:
        inline processor(const config& conf, const std::string& output, const std::string& input)
            : config(conf), iss(input), oss(output) {  }
        void run();
    private:
        const config& config;
        input_sample_stream iss;
        output_sample_stream oss;
    };

}
