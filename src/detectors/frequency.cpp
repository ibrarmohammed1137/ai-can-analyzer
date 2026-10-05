#include <cstddef>
#include "aican/detectors/frequency.hpp"
#include <algorithm>
#include <vector>
#include <cstdio>

namespace aican {
FrequencyDetector::FrequencyDetector(std::size_t w, double gf,
                                     double bf, std::size_t ms)
    : window_(w), gap_factor_(gf), burst_factor_(bf), min_samples_(ms) {}

void FrequencyDetector::feed(const CANFrame& f, DetectorContext& ctx) {
    auto it = last_ts_.find(f.can_id);
    if (it == last_ts_.end()) {
        last_ts_.emplace(f.can_id, f.timestamp);
        return;
    }
    const double gap = f.timestamp - it->second;
    it->second = f.timestamp;
    auto& hist = gaps_[f.can_id];

    if (hist.size() >= min_samples_) {
        std::vector<double> tmp(hist.begin(), hist.end());
        std::nth_element(tmp.begin(),
                         tmp.begin() + static_cast<std::ptrdiff_t>(tmp.size() / 2),
                         tmp.end());
        const double base = tmp[tmp.size() / 2];
        if (base > 0) {
            const double last = alerted_.count(f.can_id)
                              ? alerted_[f.can_id] : -1e9;
            if (f.timestamp - last > 1.0) {
                char buf[256];
                if (gap > base * gap_factor_) {
                    std::snprintf(buf, sizeof(buf),
                        "Gap %.1fms vs baseline %.1fms (x%.1f)",
                        gap * 1000, base * 1000, gap / base);
                    ctx.report(Severity::Medium, "freq_gap", f, buf);
                    alerted_[f.can_id] = f.timestamp;
                } else if (gap < base * burst_factor_ && gap < 0.001) {
                    std::snprintf(buf, sizeof(buf),
                        "Burst: gap %.2fms vs %.1fms",
                        gap * 1000, base * 1000);
                    ctx.report(Severity::Low, "freq_burst", f, buf);
                    alerted_[f.can_id] = f.timestamp;
                }
            }
        }
    }
    hist.push_back(gap);
    if (hist.size() > window_) hist.pop_front();
}
}
