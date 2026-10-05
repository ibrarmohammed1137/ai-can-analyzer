#include "aican/detectors/sequence.hpp"
#include <cstdio>

namespace aican {
SequenceDetector::SequenceDetector(int st, double fw, int ft)
    : stuck_threshold_(st), flap_window_(fw), flap_threshold_(ft) {}

void SequenceDetector::feed(const CANFrame& f, DetectorContext& ctx) {
    auto it = last_payload_.find(f.can_id);
    if (it == last_payload_.end()) {
        last_payload_.emplace(f.can_id, f.data);
        return;
    }

    // Unchanged payload -> stuck-signal counter.
    if (it->second == f.data) {
        if (++repeat_count_[f.can_id] == stuck_threshold_) {
            char buf[160];
            std::snprintf(buf, sizeof(buf),
                "Payload unchanged for %d frames: %s",
                stuck_threshold_, f.data_hex().c_str());
            ctx.report(Severity::Low, "stuck_signal", f, buf);
        }
        return;
    }
    repeat_count_[f.can_id] = 0;

    // Flapping = value returns to the one before the previous (A->B->A).
    // Monotonic counters / sweeping sensors never do this, so they are not flagged.
    auto pit = prev_payload_.find(f.can_id);
    if (pit != prev_payload_.end() && pit->second == f.data) {
        auto& ts = toggles_[f.can_id];
        ts.push_back(f.timestamp);
        while (!ts.empty() && f.timestamp - ts.front() > flap_window_)
            ts.pop_front();

        if (static_cast<int>(ts.size()) >= flap_threshold_) {
            auto la = last_flap_alert_.find(f.can_id);
            if (la == last_flap_alert_.end() || f.timestamp - la->second > 1.0) {
                char buf[128];
                std::snprintf(buf, sizeof(buf),
                    "%zu A-B-A toggles in %.1fs", ts.size(), flap_window_);
                ctx.report(Severity::High, "flapping", f, buf);
                last_flap_alert_[f.can_id] = f.timestamp;
            }
            ts.clear();
        }
    }
    prev_payload_[f.can_id] = it->second;
    it->second = f.data;
}
}
