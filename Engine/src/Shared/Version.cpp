#include "fow/Version.hpp"

namespace fow {
    String Version::to_string() const {
        return std::format("v{}.{}.{} {}", major, minor, patch, build);
    }
}
