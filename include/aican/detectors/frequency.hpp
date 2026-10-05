#pragma once
#include "aican/detector.hpp"
#include <deque>
#include <unordered_map>

namespace aican {
class FrequencyDetector : public Detector {
public:
    FrequencyDetector(std::size_t window = 32,
                      double gap_factor = 3.0,
                      double burst_factor = 0.25,
                      std::size_t min_samples = 8);
    const char* name() const override { return "frequency"; }
    void feed(const CANFrame&, DetectorContext&) override;
private:
    std::size_t window_;
    double gap_factor_;
    double burst_factor_;
    std::size_t min_samples_;
    std::unordered_map<uint32_t,double> last_ts_;
    std::unordered_map<uint32_t,std::deque<double>> gaps_;
    std::unordered_map<uint32_t,double> alerted_;
};
}
