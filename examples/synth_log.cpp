// Generates a 20 s synthetic CAN trace with one injected anomaly per detector
// and prints the analyzer report. Optionally writes the trace to a file:
//   ./synth_log [out.log]
#include "aican/analyzer.hpp"
#include "aican/parser.hpp"
#include "aican/report.hpp"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Ev { double t; std::string line; };

void emit(std::vector<Ev>& v, double t, const char* id, const char* data) {
    char buf[96];
    std::snprintf(buf, sizeof(buf), "(%.6f) can0 %s#%s", 1700000000.0 + t, id, data);
    v.push_back({t, buf});
}

}  // namespace

int main(int argc, char** argv) {
    std::vector<Ev> ev;
    char d[16];

    // 0x100: 100 Hz rolling counter  |  0x200: 20 Hz sensor sweeping 0x10..0x30
    // 0x500: 10 Hz status counter
    for (int i = 0; i < 2000; ++i) {
        const double t = i * 0.01;
        const bool dropped = (t >= 10.0 && t < 10.25);              // injected gap
        if (!dropped) { std::snprintf(d, sizeof d, "%02X00AA55", i % 16); emit(ev, t, "0x100", d); }
        if (i % 5 == 0) {
            const int k = (i / 5) % 32;
            std::snprintf(d, sizeof d, "%02X112233", 0x10 + (k < 16 ? k : 31 - k));
            emit(ev, t, "0x200", d);
        }
        if (i % 10 == 0) { std::snprintf(d, sizeof d, "%02X", (i / 10) % 16); emit(ev, t, "0x500", d); }
    }

    emit(ev, 8.000, "0x7FF", "DEADBEEF");                           // unknown ID
    emit(ev, 12.003, "0x200", "FF112233");                          // out-of-range sensor
    for (int i = 0; i < 40; ++i)                                    // flapping 0x300
        emit(ev, 16.0 + i * 0.01, "0x300", (i % 2) ? "01" : "00");
    for (int i = 0; i < 600; ++i) {                                 // injection storm
        std::snprintf(d, sizeof d, "%02XFFFFFF", i % 256);
        emit(ev, 18.0 + i * 0.0005, "0x100", d);
    }

    std::stable_sort(ev.begin(), ev.end(), [](const Ev& a, const Ev& b) { return a.t < b.t; });
    std::string text;
    for (const auto& e : ev) text += e.line + "\n";

    if (argc > 1) std::ofstream(argv[1]) << text;

    aican::Analyzer analyzer;
    std::cout << aican::summarize(analyzer.analyze(aican::parse_candump(text)));
    return 0;
}
