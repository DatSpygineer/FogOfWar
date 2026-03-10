#include "fow/Shared/StringConversion.hpp"

#include <iostream>
#include <fstream>

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
        std::ifstream ifs(path.as_std_path());
        if (!ifs.is_open()) {
            return Failure(std::format("Failed to open file \"{}\"", path));
        }

        try {
            Vector<uint8_t> data;
            data.emplace_back(ifs.get());
            return Success<Vector<uint8_t>>(data);
        }  catch (const std::exception& e) {
            return Failure(std::format("Failed to read file \"{}\": Exception thrown, {}", path, e.what()));
        }
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