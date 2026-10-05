#pragma once
#include <string>

namespace aican {

enum class Severity { Info, Low, Medium, High, Critical };

const char* to_string(Severity s);

struct Anomaly {
    Severity    severity;
    std::string rule;
    std::string can_id;
    double      timestamp;
    std::string message;

    std::string format() const;
};

} // namespace aican
