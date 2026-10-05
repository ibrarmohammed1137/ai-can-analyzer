#include "aican/frame.hpp"
#include <cstdio>

namespace aican {

std::string CANFrame::id_hex() const {
    char buf[16];
    std::snprintf(buf, sizeof(buf),
                  is_extended ? "0x%08X" : "0x%03X", can_id);
    return buf;
}

std::string CANFrame::data_hex() const {
    std::string s;
    char buf[4];
    for (std::size_t i = 0; i < data.size(); ++i) {
        std::snprintf(buf, sizeof(buf), "%02X", data[i]);
        if (i) s += ' ';
        s += buf;
    }
    return s;
}

} // namespace aican
