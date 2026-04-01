#include "Zipper.hpp"

#include "Process.hpp"

fow::Result<> MakeZip(const fow::Path& src_dir, const fow::Path& dst_zip) {
    auto proc = Process({
#ifdef _WIN32
        "powershell", "Create-Archive", "-Path", src_dir.as_string(), dst_zip.as_string()
#else
        "cd", "\""_s + src_dir.as_string() + "\";",
        "zip", "-r", dst_zip.as_relative(src_dir).as_string(), "."
#endif
    });

    return proc.run_and_wait() == 0 ? fow::Success() : fow::Failure(std::format("Failed to create zip archive \"{}\" from directory \"{}\"", dst_zip, src_dir));
}
fow::Result<> Unzip(const fow::Path& src_zip, const fow::Path& dst_dir) {
    auto proc = Process({
#ifdef _WIN32
        "powershell", "Expand-Archive", "-Path", src_zip.as_string(), "-DestinationPath", dst_dir.as_string()
#else
        "unzip", src_zip.as_string(), "-d", dst_dir.as_string()
#endif
    });

    return proc.run_and_wait() == 0 ? fow::Success() : fow::Failure(std::format("Failed to unpack zip archive \"{}\" to directory \"{}\"", src_zip, dst_dir));
}