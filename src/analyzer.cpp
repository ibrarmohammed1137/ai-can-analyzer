#include "aican/analyzer.hpp"
#include "aican/detectors/new_id.hpp"
#include "aican/detectors/frequency.hpp"
#include "aican/detectors/payload_range.hpp"
#include "aican/detectors/sequence.hpp"
#include "aican/detectors/bus_load.hpp"

namespace aican {

void DetectorContext::report(Severity sev, std::string rule,
                             const CANFrame& f, std::string msg) {
    if (!out) return;
    out->push_back(Anomaly{sev, std::move(rule),
                           f.id_hex(), f.timestamp - t0, std::move(msg)});
}

Analyzer::Analyzer() : detectors_(make_default_detectors()) {}

Analyzer::Analyzer(std::vector<std::unique_ptr<Detector>> d)
    : detectors_(std::move(d)) {}

std::vector<std::unique_ptr<Detector>> make_default_detectors() {
    std::vector<std::unique_ptr<Detector>> v;
    v.emplace_back(std::make_unique<NewIdDetector>());
    v.emplace_back(std::make_unique<FrequencyDetector>());
    v.emplace_back(std::make_unique<PayloadRangeDetector>());
    v.emplace_back(std::make_unique<SequenceDetector>());
    v.emplace_back(std::make_unique<BusLoadDetector>());
    return v;
}

AnalysisResult Analyzer::analyze(const std::vector<CANFrame>& frames) {
    AnalysisResult r;
    r.frames = frames;
    if (frames.empty()) return r;

    r.t0 = frames.front().timestamp;
    r.duration = frames.back().timestamp - r.t0;

    DetectorContext ctx;
    ctx.t0  = r.t0;
    ctx.out = &r.anomalies;

    for (const auto& f : frames) {
        r.per_id[f.can_id].push_back(f);
        for (auto& d : detectors_) d->feed(f, ctx);
    }
    return r;
}

} // namespace aican
