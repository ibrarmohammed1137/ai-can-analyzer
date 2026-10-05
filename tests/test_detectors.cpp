#include <gtest/gtest.h>
#include "aican/analyzer.hpp"
#include "aican/parser.hpp"
#include <string>

TEST(Detectors, FlagsNewId) {
    std::string txt;
    for (int i = 0; i < 600; ++i)
        txt += "(" + std::to_string(i * 0.01) + ") can0 0x100#AA\n";
    txt += "(6.5) can0 0x7FF#BB\n";
    auto frames = aican::parse_candump(txt);
    aican::Analyzer a;
    auto r = a.analyze(frames);
    bool found = false;
    for (auto& x : r.anomalies) if (x.rule == "new_id") found = true;
    EXPECT_TRUE(found);
}

namespace {

std::string line(double ts, const char* id, const char* data) {
    char buf[96];
    std::snprintf(buf, sizeof(buf), "(%.6f) can0 %s#%s\n", ts, id, data);
    return buf;
}

bool has_rule(const aican::AnalysisResult& r, const std::string& rule) {
    for (const auto& a : r.anomalies)
        if (a.rule == rule) return true;
    return false;
}

aican::AnalysisResult run(const std::string& txt) {
    aican::Analyzer a;
    return a.analyze(aican::parse_candump(txt));
}

}  // namespace

TEST(Detectors, CleanTrafficHasNoAnomalies) {
    std::string txt;
    for (int i = 0; i < 1500; ++i) {
        char d[8];
        std::snprintf(d, sizeof(d), "%02X", i % 16);
        txt += line(i * 0.01, "0x100", d);
    }
    EXPECT_TRUE(run(txt).anomalies.empty());
}

TEST(Detectors, FlagsFrequencyGap) {
    std::string txt;
    double t = 0.0;
    for (int i = 0; i < 100; ++i) {
        char d[8];
        std::snprintf(d, sizeof(d), "%02X", i % 16);
        txt += line(t += 0.01, "0x100", d);
    }
    txt += line(t += 0.25, "0x100", "05");
    EXPECT_TRUE(has_rule(run(txt), "freq_gap"));
}

TEST(Detectors, FlagsPayloadOutOfLearnedRange) {
    std::string txt;
    for (int i = 0; i < 1100; ++i) {
        char d[8];
        std::snprintf(d, sizeof(d), "%02X", 10 + i % 11);
        txt += line(i * 0.01, "0x100", d);
    }
    txt += line(11.5, "0x100", "FF");
    EXPECT_TRUE(has_rule(run(txt), "payload_range"));
}

TEST(Detectors, FlagsStuckSignal) {
    std::string txt;
    for (int i = 0; i < 80; ++i) txt += line(i * 0.01, "0x100", "AABB");
    EXPECT_TRUE(has_rule(run(txt), "stuck_signal"));
}

TEST(Detectors, FlagsFlapping) {
    std::string txt;
    for (int i = 0; i < 40; ++i)
        txt += line(i * 0.01, "0x100", (i % 2) ? "AA" : "BB");
    EXPECT_TRUE(has_rule(run(txt), "flapping"));
}

TEST(Detectors, FlagsBusStorm) {
    std::string txt;
    double t = 0.0;
    for (int i = 0; i < 500; ++i) {
        char d[8];
        std::snprintf(d, sizeof(d), "%02X", i % 16);
        txt += line(t += 0.01, "0x100", d);
    }
    for (int i = 0; i < 600; ++i) {
        char d[8];
        std::snprintf(d, sizeof(d), "%02X", i % 16);
        txt += line(t += 0.001, "0x100", d);
    }
    EXPECT_TRUE(has_rule(run(txt), "bus_storm"));
}

TEST(Parser, CandumpWithoutPrefixAndExtendedId) {
    auto f = aican::parse_candump("(1.0) can0 18FEF100#0102\n");
    ASSERT_EQ(f.size(), 1u);
    EXPECT_EQ(f[0].can_id, 0x18FEF100u);
    EXPECT_TRUE(f[0].is_extended);
}

TEST(Parser, MalformedLinesAreSkipped) {
    auto f = aican::parse_csv("timestamp,id,data\nabc,xyz,ZZ\n0.1,0x100,AA\n");
    EXPECT_EQ(f.size(), 1u);
}
