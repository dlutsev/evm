#include <fstream>
#include <sstream>
#include "processor.h"
#include "mute_converter.h"
#include "mix_converter.h"
#include "volume_converter.h"

using namespace std;

namespace {

    sound_processor::mute_converter_creator mucc;
    sound_processor::mix_converter_creator micc;
    sound_processor::volume_converter_creator vcc;

    const vector<sound_processor::convector_creator*> USABLE_CONVERTERS = {
        &mucc,
        &micc,
        &vcc
    };

}

namespace sound_processor {

    inline shared_ptr<converter> config::converter_parser(
        const string& name,
        const vector<int>& iargs,
        const vector<string>& streams,
        const vector<size_t>& stream_indexes
    ) {
        for (auto creator : creators) {
            if (0 == name.compare(creator->conv_name()))
                return creator->parse(iargs, streams, stream_indexes);
        }
        throw sound_processor::config_failure("Bad converter name");
    }

    const vector<convector_creator*>& config::creators = USABLE_CONVERTERS;

    config::config(const string& filename, const vector<string>& streams) {
        ifstream f;
        f.exceptions(fstream::badbit | fstream::failbit);
        try {
            f.open(filename, ios::in);
        }
        catch (const fstream::failure& e) {
            throw fstream::failure("Cannot open configuration file");
        }
        try {
            read_config(f, streams);
        }
        catch (const fstream::failure& e) {
            throw fstream::failure("Unexpected failure during reading configuration file");
        }
    }

    void config::read_config(ifstream& f, const vector<string>& streams) {
        while (!f.eof() && EOF != f.peek()) {
            stringstream ss;
            string name;
            getline(f, name);
            ss << name;
            ss >> name;
            if ('#' == name[0])
                continue;
            vector<int> iargs;
            vector<size_t> in_indexes;
            string buf;
            while (!ss.eof()) {
                ss >> buf;
                if (buf[0] == '$') {
                    buf.erase(0, 1);
                    size_t id = 0;
                    try {
                        id = stoul(buf);
                    }
                    catch (const exception&) {
                        throw config_failure("Bad stream number after $");
                    }
                    if (id >= streams.size() + 1 || 0 == id)
                        throw config_failure("Bad stream number after $");
                    in_indexes.push_back(id - 1);
                }
                else if (!buf.empty()) {
                    try {
                        iargs.push_back(stoi(buf));
                    }
                    catch (const exception&) {
                        throw config_failure("Bad not integer argument for converter");
                    }
                }
            }
            if (!isspace(name[0]))
                converters.push_back(converter_parser(name, iargs, streams, in_indexes));
        }
    }

    void processor::run() {
        size_t size;
        size_t i = 0;
        int16_t second[sound_processor::SAMPLE_RATE];
        const config::converter_vector& converters = config.get_converters();
        while (0 != (size = iss.read(second, sound_processor::SAMPLE_RATE)))
        {
            for (auto converter : converters)
                converter->convert(second, size, i);
            oss.write(second, size);
            i += size;
        }
    }

}