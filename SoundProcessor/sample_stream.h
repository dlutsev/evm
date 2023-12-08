#pragma once
#include <fstream>
#include <cstdint>
#include <string>

namespace sound_processor {
    class wav_failure : public std::exception {
    public:
        wav_failure(const char* massage) : m(massage) {}
        wav_failure() : m(nullptr) {}
        virtual const char* what() const {
            if(m == nullptr)
                return "Bad WAV format";
            else
                return m;
        }
    private:
        const char* m;
    };

    class input_sample_stream {
    public:
        input_sample_stream(const std::string&);
        input_sample_stream(const input_sample_stream& o) : input_sample_stream(*o.name) {}
        input_sample_stream(input_sample_stream&& o) noexcept : input_sample_stream(*o.name) {}
        ~input_sample_stream() {
            file.close();
            delete name;
        }
        size_t read(int16_t* buffer, size_t size);
        const std::string& getName() const { return *name; }
    private:
        std::ifstream file;
        std::streampos data_begin;
        std::size_t bytes_of_data;
        const std::string* name;
        void init_wav_file();
    };

    class output_sample_stream {
    public:
        explicit output_sample_stream(const std::string&);
        inline ~output_sample_stream() {
            file.close();
            delete name;
        }
        void write(const int16_t* buffer, size_t size);
        const std::string& get_name() const { return *name; }
    private:
        std::ofstream file;
        std::size_t data_size;
        const std::string* name;
        void commit_wav_file();
    };

}
