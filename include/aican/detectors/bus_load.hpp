#pragma once
#include "aican/detector.hpp"
#include <deque>

namespace aican {
class BusLoadDetector : public Detector {
public:
    BusLoadDetector(double window_sec = 0.1,
                    double spike_factor = 5.0,
                    double warmup_sec = 2.0);
    const char* name() const override { return "bus_load"; }
    void feed(const CANFrame&, DetectorContext&) override;
private:
    double window_sec_, spike_factor_, warmup_sec_;
    std::deque<double> buckets_;
    bool   has_bucket_   = false;
    double bucket_start_ = 0.0;
    int    bucket_count_ = 0;
    double baseline_     = 0.0;
    bool   has_baseline_ = false;
    double t0_           = 0.0;
    bool   has_t0_       = false;
};
}
