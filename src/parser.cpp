#include "aican/parser.hpp"
#include <fstream>
#include <sstream>
#include <regex>
#include <cctype>
#include <stdexcept>

namespace aican {
namespace {
const std::regex kCandump{
    R"(\(([\d.]+)\)\s+(\S+)\s+(?:0[xX])?([0-9A-Fa-f]+)#([0-9A-Fa-f]*))"
};

std::vector<uint8_t> hex_to_bytes(const std::string& h) {
    std::vector<uint8_t> out;
    out.reserve(h.size() / 2);
    for (std::size_t i = 0; i + 1 < h.size(); i += 2) {
        out.push_back(static_cast<uint8_t>(
            std::stoul(h.substr(i, 2), nullptr, 16)));
    }
    return out;
}

std::string trim(const std::string& s) {
    auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return {};
    auto e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}
} // namespace

std::vector<CANFrame> parse_candump(const std::string& text) {
    std::vector<CANFrame> frames;
    std::istringstream in(text);
    std::string line;
    std::smatch m;
    while (std::getline(in, line)) {
        if (!std::regex_search(line, m, kCandump)) continue;
        try {
            CANFrame f;
            f.timestamp = std::stod(m[1].str());
            f.bus       = m[2].str();
            const std::string id = m[3].str();
            f.can_id      = static_cast<uint32_t>(std::stoul(id, nullptr, 16));
            f.is_extended = id.size() > 3;
            f.data        = hex_to_bytes(m[4].str());
            frames.push_back(std::move(f));
        } catch (const std::exception&) {
            continue;  // skip malformed line
        }
    }
    return frames;
}

std::vector<CANFrame> parse_csv(const std::string& text) {
    std::vector<CANFrame> frames;
    std::istringstream in(text);
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (std::tolower(static_cast<unsigned char>(line[0])) == 't') continue;
        std::istringstream ls(line);
        std::string tok;
        std::vector<std::string> parts;
        while (std::getline(ls, tok, ',')) parts.push_back(trim(tok));
        if (parts.size() < 3) continue;

        try {
            CANFrame f;
            f.timestamp = std::stod(parts[0]);
            const std::string& id = parts[1];
            const bool hex = id.rfind("0x", 0) == 0 || id.rfind("0X", 0) == 0;
            f.can_id = static_cast<uint32_t>(
                std::stoul(hex ? id.substr(2) : id, nullptr, hex ? 16 : 10));
            f.is_extended = f.can_id > 0x7FF;
            f.data = hex_to_bytes(parts[2]);
            f.bus  = parts.size() > 3 ? parts[3] : "can0";
            frames.push_back(std::move(f));
        } catch (const std::exception&) {
            continue;  // skip malformed line
        }
    }
    return frames;
}

std::vector<CANFrame> parse_auto(const std::string& text) {
    if (std::regex_search(text, kCandump)) return parse_candump(text);
    return parse_csv(text);
}

std::vector<CANFrame> load_log(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("cannot open " + path);
    std::ostringstream ss;
    ss << in.rdbuf();
    return parse_auto(ss.str());
}

} // namespace aican
