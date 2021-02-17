#!/usr/bin/env bash
set -euxo pipefail
if [[ ! -d out ]]; then
    CXX=clang++ meson setup out
fi
meson compile -C out
./out/bench
