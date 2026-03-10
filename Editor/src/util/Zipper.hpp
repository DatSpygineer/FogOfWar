#ifndef FOW_EDITOR_ZIPPER_HPP
#define FOW_EDITOR_ZIPPER_HPP

#include <fow/Shared.hpp>

fow::Result<> MakeZip(const fow::Path& src_dir, const fow::Path& dst_zip);
fow::Result<> Unzip(const fow::Path& src_zip, const fow::Path& dst_dir);

#endif