#include "aican/report.hpp"
#include <algorithm>
#include <sstream>
#include <cstdio>
#include <vector>

namespace aican {
namespace {
int sev_rank(Severity s) {
    switch (s) {
        case Severity::Critical: return 0;
        case Severity::High:     return 1;
        case Severity::Medium:   return 2;
        case Severity::Low:      return 3;
        case Severity::Info:     return 4;
    }
    return 5;
}
}

std::string summarize(const AnalysisResult& r) {
    std::ostringstream o;
    o << "=====================================================================\n";
    o << "CAN ANALYSIS REPORT\n";
    o << "=====================================================================\n";

    if (r.frames.empty()) { o << "No frames parsed.\n"; return o.str(); }

    o << "Frames:        " << r.frames.size() << "\n";
    o << "Duration:      " << r.duration << "s\n";
    o << "Unique IDs:    " << r.per_id.size() << "\n";
    if (r.duration > 0)
        o << "Avg rate:      " << (static_cast<double>(r.frames.size()) / r.duration) << " fps\n\n";

    o << "Top CAN IDs by volume:\n";
    std::vector<std::pair<uint32_t,std::size_t>> counts;
    for (auto& kv : r.per_id) counts.emplace_back(kv.first, kv.second.size());
    std::sort(counts.begin(), counts.end(),
              [](const auto& a, const auto& b){ return a.second > b.second; });
    for (std::size_t i = 0; i < std::min<std::size_t>(12, counts.size()); ++i) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "  0x%03X   %6zu frames\n",
                      counts[i].first, counts[i].second);
        o << buf;
    }
    o << "\n";

    if (r.anomalies.empty()) {
        o << "No anomalies detected.\n";
        return o.str();
    }

    auto sorted = r.anomalies;
    std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b){
        if (a.severity != b.severity)
            return sev_rank(a.severity) < sev_rank(b.severity);
        return a.timestamp < b.timestamp;
    });

    o << "! " << sorted.size() << " ANOMALIES DETECTED\n";
    o << "---------------------------------------------------------------------\n";
    for (auto& a : sorted) o << a.format() << "\n";
    return o.str();
}

} // namespace aican
