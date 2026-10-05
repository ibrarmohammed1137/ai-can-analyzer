#include <cstddef>
#include "aican/detectors/bus_load.hpp"
#include <algorithm>
#include <vector>
#include <cstdio>

namespace aican {
BusLoadDetector::BusLoadDetector(double w, double sf, double ws)
    : window_sec_(w), spike_factor_(sf), warmup_sec_(ws) {}

void BusLoadDetector::feed(const CANFrame& f, DetectorContext& ctx) {
    if (!has_t0_)     { t0_ = f.timestamp;           has_t0_ = true; }
    if (!has_bucket_) { bucket_start_ = f.timestamp; has_bucket_ = true; }

    if (f.timestamp - bucket_start_ >= window_sec_) {
        const double rate = bucket_count_ / window_sec_;
        buckets_.push_back(rate);
        if (buckets_.size() > 100) buckets_.pop_front();

        if (!has_baseline_ && buckets_.size() >= 10) {
            std::vector<double> tmp(buckets_.begin(), buckets_.end());
            std::nth_element(tmp.begin(),
                             tmp.begin() + static_cast<std::ptrdiff_t>(tmp.size() / 2), tmp.end());
            baseline_     = tmp[tmp.size() / 2];
            has_baseline_ = true;
        }
        if (has_baseline_ && f.timestamp - t0_ > warmup_sec_ &&
            rate > baseline_ * spike_factor_) {
            char buf[160];
            std::snprintf(buf, sizeof(buf),
                "Bus rate %.0f fps vs baseline %.0f fps (x%.1f)",
                rate, baseline_, rate / baseline_);
            ctx.report(Severity::High, "bus_storm", f, buf);
        }
        bucket_start_ = f.timestamp;
        bucket_count_ = 0;
    }
    ++bucket_count_;
}
}
