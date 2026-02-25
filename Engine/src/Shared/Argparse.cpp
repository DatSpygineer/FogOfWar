#include "fow/Shared/Argparse.hpp"

namespace fow {
    HashMap<String, Vector<String>> ParseArgs(const int argc, char** argv) {
        HashMap<String, Vector<String>> args;
        String current_arg = "";
        for (int i = 1; i < argc; ++i) {
            if (argv[i][0] == '-') {
                current_arg = argv[i];
                args.emplace(current_arg, Vector<String> { });
            } else if (!current_arg.is_empty()) {
                args.at(current_arg).emplace_back(argv[i]);
            }
        }
        return args;
    }
}