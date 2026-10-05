#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace aican {

struct CANFrame {
    double               timestamp   = 0.0;
    uint32_t             can_id      = 0;
    bool                 is_extended = false;
    std::string          bus         = "can0";
    std::vector<uint8_t> data;

    std::string id_hex() const;
    std::string data_hex() const;
};

} // namespace aican
