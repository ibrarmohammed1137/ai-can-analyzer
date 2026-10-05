#!/usr/bin/env bash
set -euo pipefail
cmake --preset ci
cmake --build --preset ci --parallel
ctest --preset ci --output-on-failure
