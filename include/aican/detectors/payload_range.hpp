#pragma once
#include "aican/detector.hpp"
#include <unordered_map>
#include <array>
#include <utility>

namespace aican {
class PayloadRangeDetector : public Detector {
public:
    explicit PayloadRangeDetector(std::size_t learn_frames = 1000, int margin = 0);
    const char* name() const override { return "payload_range"; }
    void feed(const CANFrame&, DetectorContext&) override;
private:
    std::size_t learn_frames_;
    int margin_;
    std::size_t count_ = 0;
    std::unordered_map<uint32_t, std::array<std::pair<int,int>,8>> ranges_;
    std::unordered_map<uint32_t, double> last_alert_;
};
}
