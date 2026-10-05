# AI CAN Analyzer

Interpret CAN bus logs and flag unusual patterns. Fast C++17 core, no runtime dependencies.

> Detection is rule-based and statistical (learned baselines, medians, thresholds) — there is no ML model
> in the loop. That makes results explainable and reproducible, and the `Detector` interface is the
> extension point if you later want to plug in a learned model or an LLM-based triage step.

## Features
- Parses `candump -L` style logs (`(ts) can0 123#DEADBEEF`, `0x` prefix optional) and CSV (`timestamp,id,data[,bus]`)
- Six detectors: new ID, frequency gap / burst, payload range, stuck signal, flapping, bus storm
- Severity-sorted report; exit code `1` when anomalies are found (CI-friendly)
- CMake presets, unit tests (GoogleTest), ASAN/UBSAN preset, Docker image, GitHub Actions

## Quickstart
```bash
git clone https://github.com/<your-user>/ai-can-analyzer
cd ai-can-analyzer
cmake --preset ci
cmake --build --preset ci --parallel
ctest --preset ci

./build-ci/aican tests/test_data/sample.log
./build-ci/synth_log logs/demo_attack.log        # regenerate the demo trace + report
```
Or run everything with `scripts/build.sh`; analyze a log with `scripts/run_analysis.sh <log>`.

## Usage
```text
aican <log-file>
```
| Exit code | Meaning |
|---|---|
| 0 | parsed OK, no anomalies |
| 1 | anomalies detected |
| 2 | usage error / file unreadable / nothing parsed |

## Example output (`synth_log`, one injected fault per detector)
```text
=====================================================================
CAN ANALYSIS REPORT
=====================================================================
Frames:        3217
Duration:      19.99s
Unique IDs:    5
Avg rate:      160.93 fps

Top CAN IDs by volume:
  0x100     2575 frames
  0x200      401 frames
  0x500      200 frames
  0x300       40 frames
  0x7FF        1 frames

! 10 ANOMALIES DETECTED
---------------------------------------------------------------------
[HIGH    ] new_id                  0x7FF @     8.000s  Previously unseen CAN ID 0x7FF
[HIGH    ] new_id                  0x300 @    16.000s  Previously unseen CAN ID 0x300
[HIGH    ] flapping                0x300 @    16.110s  10 A-B-A toggles in 0.5s
[HIGH    ] bus_storm               0x100 @    18.111s  Bus rate 2130 fps vs baseline 140 fps (x15.2)
[HIGH    ] bus_storm               0x100 @    18.211s  Bus rate 2140 fps vs baseline 140 fps (x15.3)
[HIGH    ] bus_storm               0x100 @    18.320s  Bus rate 1910 fps vs baseline 140 fps (x13.6)
[MEDIUM  ] freq_gap                0x100 @    10.250s  Gap 260.0ms vs baseline 10.0ms (x26.0)
[MEDIUM  ] payload_range           0x200 @    12.003s  Byte 0=FF outside learned [10..1F]
[MEDIUM  ] payload_range           0x100 @    18.000s  Byte 1=FF outside learned [00..00]; Byte 2=FF outside learned […
[LOW     ] freq_burst              0x100 @    18.000s  Burst: gap 0.00ms vs 10.0ms
```

## Detectors
See [docs/DETECTORS.md](docs/DETECTORS.md) for rules, default thresholds and how to add your own.

## Docker
```bash
docker build -t ai-can-analyzer .
docker run --rm -v "$PWD/logs:/data:ro" ai-can-analyzer demo_attack.log
```

## Layout
```text
include/aican/            public headers (+ detectors/)
src/                      library + CLI (+ detectors/)
tests/                    GoogleTest suites, tests/test_data/sample.log
examples/synth_log.cpp    synthetic trace generator with injected anomalies
cmake/                    warnings, sanitizers, packaging helpers
scripts/                  build.sh, run_analysis.sh
logs/                     your captures (demo_attack.log included)
docs/                     detector reference
.github/                  CI workflow, issue / PR templates
```

## Known limitations
- Baselines are learned from the first N frames of the *same* log (500 for new-ID, 1000 for payload range).
  Anything first seen after that is reported, so use a capture whose beginning is known-good.
- Payload-range checks the first 8 data bytes per ID; CAN FD payloads beyond 8 bytes are not range-checked.
- Parsing uses `std::regex` — fine for typical captures, but multi-GB logs would want a hand-written parser.
- Rolling counters and CRC bytes legitimately span their full range; they are learned as such, but a counter
  that wraps *after* the learning window can raise `payload_range` alerts.

## License
MIT — see [LICENSE](LICENSE).
