#include "aican/detectors/new_id.hpp"
namespace aican {
NewIdDetector::NewIdDetector(std::size_t lf) : learn_frames_(lf) {}
void NewIdDetector::feed(const CANFrame& f, DetectorContext& ctx) {
    ++count_;
    if (count_ <= learn_frames_) { seen_.insert(f.can_id); return; }
    if (seen_.insert(f.can_id).second) {
        ctx.report(Severity::High, "new_id", f,
                   "Previously unseen CAN ID " + f.id_hex());
    }
}
}
