#ifndef FOW_ARGPARSE_HPP
#define FOW_ARGPARSE_HPP

#include <fow/Shared/Api.hpp>
#include <fow/Shared/String.hpp>

namespace fow {
    FOW_SHARED_API HashMap<String, Vector<String>> ParseArgs(int argc, char** argv);
}

#endif