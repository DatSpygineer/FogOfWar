#include "fow/Shared/Argparse.hpp"

namespace fow {
    HashMap<String, Vector<String>> ParseArgs(const int argc, os_char_t** argv) {
        HashMap<String, Vector<String>> args;
        String current_arg = "";
        for (int i = 1; i < argc; ++i) {
            if (argv[i][0] == '-') {
#if FOW_OS_USE_WCHAR
                current_arg = String::FromWChar(argv[i]);
#else
                current_arg = argv[i];
#endif
                args.emplace(current_arg, Vector<String> { });
            } else if (!current_arg.is_empty()) {
#if FOW_OS_USE_WCHAR
                args.at(current_arg).emplace_back(String::FromWChar(argv[i]));
#else
                args.at(current_arg).emplace_back(argv[i]);
#endif
            }
        }
        return args;
    }
}