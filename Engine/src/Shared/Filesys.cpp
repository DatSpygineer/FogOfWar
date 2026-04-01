#include <iostream>
#include <fstream>
#include <fow/Shared/Filesys.hpp>

namespace fow::Files {
    Result<String> ReadAllText(const Path& path) {
        std::ifstream ifs(path.as_std_path());
        if (!ifs.is_open()) {
            return Failure(std::format("Failed to open file \"{}\"", path));
        }

        try {
            std::string line;
            std::ostringstream oss;
            while (std::getline(ifs, line)) {
                oss << line << std::endl;
            }
            return Success<String>(oss.str());
        } catch (const std::exception& e) {
            return Failure(std::format("Failed to read file \"{}\": Exception thrown, {}", path, e.what()));
        }
    }
    Result<Vector<String>> ReadAllLines(const Path& path) {
        std::ifstream ifs(path.as_std_path());
        if (!ifs.is_open()) {
            return Failure(std::format("Failed to open file \"{}\"", path));
        }

        try {
            Vector<String> lines;
            std::string line;
            while (std::getline(ifs, line)) {
                lines.emplace_back(line);
            }
            return Success<Vector<String>>(lines);
        } catch (const std::exception& e) {
            return Failure(std::format("Failed to read file \"{}\": Exception thrown, {}", path, e.what()));
        }
    }
    Result<Vector<uint8_t>> ReadAllBytes(const Path& path) {
        if (!path.exists()) {
            return Failure(std::format("File \"{}\" does not exist", path));
        }

        FILE* f = fopen(path.as_std_path().c_str(), "rb");
        if (f == nullptr) {
            return Failure(std::format("Failed to open file \"{}\"", path));
        }

        Vector<uint8_t> data;
        fseek(f, 0, SEEK_END);
        const size_t size = ftell(f);
        data.resize(size);
        fseek(f, 0, SEEK_SET);
        fread(data.data(), 1, size, f);
        fclose(f);
        return data;
    }

    Result<> WriteAllText(const Path& path, const String& text) {
        std::ofstream ofs(path.as_std_path());
        if (!ofs.is_open()) {
            return Failure(std::format("Failed to open file \"{}\"", path));
        }
        ofs << text.as_std_str() << std::endl;
        return Success();
    }
    Result<> WriteAllLines(const Path& path, const Vector<String>& lines) {
        std::ofstream ofs(path.as_std_path());
        if (!ofs.is_open()) {
            return Failure(std::format("Failed to open file \"{}\"", path));
        }
        for (const auto line : lines) {
            ofs << line.as_std_str() << std::endl;
        }
        return Success();
    }
    Result<> WriteAllBytes(const Path& path, const Vector<uint8_t>& lines) {
        std::ofstream ofs(path.as_std_path());
        ofs.write(reinterpret_cast<const char*>(lines.data()), lines.size());
        return Success();
    }
}