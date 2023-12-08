#include <cstring>
#include "sample_stream.h"
#include "processor.h"

using namespace std;

namespace {
    const char* wav_header = "RIFF\0\0\0\0WAVE"
        "fmt\x20\x10\0\0\0"     // fmt chunk header
        "\x01\0\x01\0"          // PCM mono
        "\x44\xAC\0\0"          // 44100
        "\x88\x58\x01\0"        // 88200
        "\2\0\x10\0"            // align & bits per sample
        "data\0\0\0\0";         // data chunk header


    inline uint32_t le2ui32(const char bytes[4]) {
        return (bytes[0] & 0xff) | ((bytes[1] & 0xff) << 8) | ((bytes[2] & 0xff) << 16) | ((bytes[3] & 0xff) << 24);
    }

    inline void ui32_2le(char bytes[4], uint32_t i) {
        bytes[0] = i & 0xff;
        bytes[1] = (i >> 8) & 0xff;
        bytes[2] = (i >> 16) & 0xff;
        bytes[3] = (i >> 24) & 0xff;
    }

    inline uint16_t le2i16(const char bytes[2]) {
        return (bytes[0] & 0xff) | ((bytes[1] & 0xff) << 8);
    }

    inline void i16_2le(char bytes[2], uint16_t i) {
        bytes[0] = i & 0xff;
        bytes[1] = (i >> 8) & 0xff;
    }

}

namespace sound_processor {
    input_sample_stream::input_sample_stream(const string& filename) : name(new string(filename)) {
        file.exceptions(ios::badbit | ios::failbit | ios::eofbit);
        try {
            file.open(filename, ios::in | ios::binary);
        }
        catch (const fstream::failure& e) {
            throw fstream::failure("Cannot open input file");
        }
        try {
            init_wav_file();
        }
        catch (const fstream::failure& e) {
            throw wav_failure("Bad input WAV file");
        }
    }

    size_t input_sample_stream::read(int16_t* buffer, size_t size) {
        size *= 2;
        streampos curpos = file.tellg();
        if (size + curpos - data_begin > bytes_of_data)
            size = bytes_of_data - (curpos - data_begin);
        size = (size / 2) * 2;
        char* cbuf = reinterpret_cast<char*>(buffer);
        try {
            file.read(cbuf, size);
        }
        catch (const ifstream::failure& e) {
            throw wav_failure("Unexpected failure during reading file.");
        }
        for (size_t i = 0; i < size / 2; ++i) {
            int16_t tmp = le2i16(cbuf + i * 2);
            buffer[i] = tmp;
        }
        return size / 2;
    }

    void input_sample_stream::init_wav_file() {
        char buff[4];
        enum { RIFF, SEARCHFMT, FMT, SEARCHDATA, DATA } state = RIFF;
        while (state != DATA) {
            if (state == RIFF) {
                file.read(buff, 4);
                if (0 != strncmp(buff, "RIFF", 4))
                    throw wav_failure();
                file.read(buff, 4); // chunk size
                file.read(buff, 4);
                if (0 != strncmp(buff, "WAVE", 4))
                    throw wav_failure();
                state = SEARCHFMT;
            }
            else if (state == SEARCHFMT) {
                file.read(buff, 4);
                if (0 != strncmp(buff, "fmt\x20", 4)) {
                    file.read(buff, 4);
                    uint32_t subchunkSize = le2ui32(buff);
                    file.seekg(subchunkSize, file.cur);
                    state = SEARCHFMT;
                }
                else {
                    state = FMT;
                }
            }
            else if (state == FMT) {
                file.read(buff, 4);
                uint32_t subchunk_size = le2ui32(buff);
                file.read(buff, 2);
                if (1 != buff[0] || buff[1] != 0 || 16u != subchunk_size)
                    throw wav_failure("Not a PCM format");
                file.read(buff, 2);
                if (1 != buff[0] || 0 != buff[1])
                    throw wav_failure("Not a mono");
                file.read(buff, 4);
                if (SAMPLE_RATE != le2ui32(buff))
                    throw wav_failure("Not a 44100 rate");
                file.seekg(8, file.cur);
                state = SEARCHDATA;
            }
            else if (state == SEARCHDATA) {
                file.read(buff, 4);
                if (0 != strncmp(buff, "data", 4)) {
                    file.read(buff, 4);
                    uint32_t subchunkSize = le2ui32(buff);
                    file.seekg(subchunkSize, file.cur);
                    state = SEARCHDATA;
                }
                else {
                    file.read(buff, 4);
                    data_begin = file.tellg();
                    bytes_of_data = le2ui32(buff);
                    state = DATA;
                }
            }
        }
    }

    output_sample_stream::output_sample_stream(const std::string& filename)
        : name(new string(filename)), data_size(0) {
        file.exceptions(ios::badbit | ios::failbit | ios::eofbit);
        try {
            file.open(filename, ios::out | ios::binary);
        }
        catch (const fstream::failure& e) {
            throw fstream::failure("Cannot open input file");
        }
        try {
            file.write(wav_header, 44);
        }
        catch (const fstream::failure& e) {
            throw fstream::failure("Unexpected exception while writing output file");
        }
    }

    void output_sample_stream::write(const int16_t* buffer, size_t size) {
        size *= 2;
        data_size += size;
        streampos tmp = file.tellp();
        commit_wav_file();
        tmp = file.tellp();
        char* cbuf = new char[size];
        for (size_t i = 0; i < size / 2; ++i)
            i16_2le(cbuf + i * 2, buffer[i]);
        file.write(cbuf, size);
        tmp = file.tellp();
    }

    void output_sample_stream::commit_wav_file() {
        char buf[4];
        ui32_2le(buf, data_size + 36);
        file.seekp(4, file.beg);
        file.write(buf, 4);
        ui32_2le(buf, data_size);
        file.seekp(40, file.beg);
        file.write(buf, 4);
        file.seekp(0, file.end);
    }

}