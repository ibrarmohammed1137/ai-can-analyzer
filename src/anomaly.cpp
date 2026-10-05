#include "aican/anomaly.hpp"
#include <cstdio>

namespace aican {

const char* to_string(Severity s) {
    switch (s) {
        case Severity::Info:     return "INFO";
        case Severity::Low:      return "LOW";
        case Severity::Medium:   return "MEDIUM";
        case Severity::High:     return "HIGH";
        case Severity::Critical: return "CRITICAL";
    }
    return "?";
}

std::string Anomaly::format() const {
    char buf[512];
    std::snprintf(buf, sizeof(buf), "[%-8s] %-18s %10s @ %9.3fs  %s",
                  to_string(severity), rule.c_str(),
                  can_id.c_str(), timestamp, message.c_str());
    return buf;
}

} // namespace aican
