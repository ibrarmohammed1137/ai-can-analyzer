#pragma once
#include "aican/frame.hpp"
#include "aican/anomaly.hpp"
#include <vector>

namespace aican {

struct DetectorContext {
    double t0 = 0.0;
    std::vector<Anomaly>* out = nullptr;

    void report(Severity sev,
                std::string rule,
                const CANFrame& f,
                std::string msg);
};

class Detector {
public:
    virtual ~Detector() = default;
    virtual const char* name() const = 0;
    virtual void feed(const CANFrame& frame, DetectorContext& ctx) = 0;
};

} // namespace aican
