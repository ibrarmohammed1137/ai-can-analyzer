#include "aican/detectors/payload_range.hpp"
#include <algorithm>
#include <cstdio>
#include <string>

namespace aican {
PayloadRangeDetector::PayloadRangeDetector(std::size_t lf, int m)
    : learn_frames_(lf), margin_(m) {}

void PayloadRangeDetector::feed(const CANFrame& f, DetectorContext& ctx) {
    ++count_;
    const bool learning = count_ <= learn_frames_;
    auto ins = ranges_.try_emplace(f.can_id);
    auto& r = ins.first->second;
    if (ins.second) {
        for (auto& p : r) { p.first = 255; p.second = 0; }  // empty range
    }

    std::string msg;
    std::size_t violations = 0;
    for (std::size_t i = 0; i < f.data.size() && i < 8; ++i) {
        const int b = f.data[i];
        auto& lo = r[i].first;
        auto& hi = r[i].second;
        if (learning) {
            lo = std::min(lo, b);
            hi = std::max(hi, b);
        } else if (lo <= hi && (b < lo - margin_ || b > hi + margin_)) {
            if (++violations <= 3) {
                char buf[64];
                std::snprintf(buf, sizeof(buf), "%sByte %zu=%02X outside learned [%02X..%02X]",
                              msg.empty() ? "" : "; ", i, b, lo, hi);
                msg += buf;
            }
        }
    }
    if (violations == 0) return;
    if (violations > 3) msg += "; +" + std::to_string(violations - 3) + " more";

    // One alert per frame, at most one per ID per second (avoid flooding the report).
    auto la = last_alert_.find(f.can_id);
    if (la != last_alert_.end() && f.timestamp - la->second <= 1.0) return;
    last_alert_[f.can_id] = f.timestamp;
    ctx.report(Severity::Medium, "payload_range", f, msg);
}
}
