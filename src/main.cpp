#include "aican/analyzer.hpp"
#include "aican/parser.hpp"
#include "aican/report.hpp"

#include <iostream>
#include <string>
#include <cstdlib>

static void usage(const char* argv0) {
    std::cerr << "Usage: " << argv0 << " <log-file>\n";
    std::exit(2);
}

int main(int argc, char** argv) {
    if (argc < 2) usage(argv[0]);
    try {
        const std::string path = argv[1];
        auto frames = aican::load_log(path);
        if (frames.empty()) {
            std::cerr << "No frames parsed from " << path << "\n";
            return 1;
        }
        aican::Analyzer analyzer;
        auto result = analyzer.analyze(frames);
        std::cout << aican::summarize(result);
        return result.anomalies.empty() ? 0 : 1;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 2;
    }
}
