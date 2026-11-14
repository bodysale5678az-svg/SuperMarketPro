#pragma once
// Small helpers shared by the service implementation.
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

namespace smp::io {

inline std::vector<std::string> readLines(const std::filesystem::path& file) {
    std::vector<std::string> lines;
    std::ifstream in(file);
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty()) lines.push_back(line);
    }
    return lines;
}

inline void writeLines(const std::filesystem::path& file,
                       const std::vector<std::string>& lines) {
    std::filesystem::create_directories(file.parent_path());
    std::ofstream out(file, std::ios::trunc);
    for (const auto& l : lines) out << l << '\n';
}

} // namespace smp::io
