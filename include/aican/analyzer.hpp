#pragma once
#include "aican/frame.hpp"
#include "aican/anomaly.hpp"
#include "aican/detector.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace aican {

struct AnalysisResult {
    std::vector<CANFrame> frames;
    std::unordered_map<uint32_t, std::vector<CANFrame>> per_id;
    std::vector<Anomaly> anomalies;
    double duration = 0.0;
    double t0 = 0.0;
};

class Analyzer {
public:
    Analyzer();
    explicit Analyzer(std::vector<std::unique_ptr<Detector>> detectors);

    AnalysisResult analyze(const std::vector<CANFrame>& frames);

private:
    std::vector<std::unique_ptr<Detector>> detectors_;
};

std::vector<std::unique_ptr<Detector>> make_default_detectors();

} // namespace aican
