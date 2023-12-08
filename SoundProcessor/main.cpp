#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <cstdint>
#include "processor.h"

using namespace std;

namespace {
    void get_arguments(string& out, string& config, vector<string>& in, int argc, char* argv[]) {
        unsigned int argi = 1;
        enum { FIRST, CONF, OUT, IN } state = FIRST;
        while (argi < argc) {
            if (state == FIRST)
                if (0 == strcmp("-c", argv[argi])) {
                    config = argv[++argi];
                    state = OUT;
                }
                else {
                    out = argv[argi];
                    state = CONF;
                }
            else if (state == CONF)
                if (0 == strcmp("-c", argv[argi])) {
                    config = argv[++argi];
                    state = IN;
                }
                else {
                    in.push_back(argv[argi]);
                    state = CONF;
                }
            else if (state == OUT) {
                out = argv[argi];
                state = IN;
            }
            else if (state == IN) {
                in.push_back(argv[argi]);
                state = IN;
            }
            ++argi;
        }
    }

}

int main(int argc, char* argv[]) {
    if (argc > 1 && 0 == strcmp(argv[1], "-h")) {
        cout << "sound_processor [-h] "
            "[-c config.txt "
            "output.wav input1.wav [input2.wav ...]]\n" << endl;
        cout << "Valid input files are WAV files; mono PCM s16le 44100Hz format.\n" << endl;
        cout << "Converters:\n" << endl;
        for (auto creator : sound_processor::config::creators)
            cout << creator->conv_help() << endl;
        return 0;
    }
    string out;
    string conf;
    vector<string> input;
    get_arguments(out, conf, input, argc, argv);
    if (out.compare("") == 0) {
        cerr << "No output file!" << endl;
        return 1;
    }
    if (conf.compare("") == 0) {
        cerr << "Configuration file is required!" << endl;
        return 1;
    }
    if (0 == input.size()) {
        cerr << "No input files!" << endl;
        return 1;
    }

    try {
        sound_processor::config configuration{ conf, input };
        sound_processor::processor proc{ configuration, out, input[0] };
        proc.run();
    }
    catch (const sound_processor::wav_failure& e) {
        cerr << e.what() << endl;
        return 3;
    }
    catch (const fstream::failure& e) {
        cerr << e.what() << endl;
        return 2;
    }
    catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}