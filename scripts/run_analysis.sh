#!/usr/bin/env bash
set -euo pipefail
LOG="${1:-tests/test_data/sample.log}"
./build-ci/aican "$LOG"
