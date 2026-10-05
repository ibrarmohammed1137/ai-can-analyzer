# Detector reference

All detectors implement `aican::Detector` (`feed(frame, ctx)`) and report through `DetectorContext::report`.
Defaults below are the constructor defaults in `src/analyzer.cpp::make_default_detectors()`.

| Rule | Severity | Fires when | Defaults |
|---|---|---|---|
| `new_id` | HIGH | An ID appears that was not seen in the learning window | learn = 500 frames |
| `freq_gap` | MEDIUM | Inter-arrival gap > factor × median gap of that ID | window 32, factor 3.0, min 8 samples |
| `freq_burst` | LOW | Gap < 0.25 × median **and** < 1 ms | same window; 1 alert / ID / s |
| `payload_range` | MEDIUM | A data byte leaves the min..max learned for that ID/byte | learn = 1000 frames, margin 0; 1 alert / ID / s |
| `stuck_signal` | LOW | Payload identical for N consecutive frames | N = 50 |
| `flapping` | HIGH | ≥ N A→B→A toggles within a time window (rolling counters are *not* flagged) | N = 10, window 0.5 s; 1 alert / ID / s |
| `bus_storm` | HIGH | Frame rate in a 100 ms bucket > factor × baseline median rate | factor 5.0, warm-up 2 s |

## Adding a detector
1. Create `include/aican/detectors/my_rule.hpp` and `src/detectors/my_rule.cpp`, deriving from `Detector`.
2. Add the `.cpp` to `add_library(aican ...)` in `CMakeLists.txt`.
3. Register it in `make_default_detectors()` (`src/analyzer.cpp`).
4. Add a test in `tests/test_detectors.cpp` that builds a small candump string and asserts the rule fires —
   and one clean-traffic case asserting it does not.
