#pragma once
#include "aican/detector.hpp"
#include <unordered_set>

namespace aican {
class NewIdDetector : public Detector {
public:
    explicit NewIdDetector(std::size_t learn_frames = 500);
    const char* name() const override { return "new_id"; }
    void feed(const CANFrame&, DetectorContext&) override;
private:
    std::size_t learn_frames_;
    std::size_t count_ = 0;
    std::unordered_set<uint32_t> seen_;
};
}
