#pragma once
#include "aican/frame.hpp"
#include <string>
#include <vector>

namespace aican {

enum class LogFormat { Auto, CanDump, CSV };

std::vector<CANFrame> parse_candump(const std::string& text);
std::vector<CANFrame> parse_csv(const std::string& text);
std::vector<CANFrame> parse_auto(const std::string& text);
std::vector<CANFrame> load_log(const std::string& path);

} // namespace aican
