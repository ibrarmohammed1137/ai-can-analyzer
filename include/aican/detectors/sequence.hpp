#pragma once
#include "aican/detector.hpp"
#include <deque>
#include <unordered_map>
#include <vector>

namespace aican {
class SequenceDetector : public Detector {
public:
    SequenceDetector(int stuck_threshold = 50,
                     double flap_window = 0.5,
                     int flap_threshold = 10);
    const char* name() const override { return "sequence"; }
    void feed(const CANFrame&, DetectorContext&) override;
private:
    int stuck_threshold_;
    double flap_window_;
    int flap_threshold_;
    std::unordered_map<uint32_t,std::vector<uint8_t>> last_payload_;
    std::unordered_map<uint32_t,std::vector<uint8_t>> prev_payload_;
    std::unordered_map<uint32_t,int> repeat_count_;
    std::unordered_map<uint32_t,std::deque<double>> toggles_;
    std::unordered_map<uint32_t,double> last_flap_alert_;
};
}
