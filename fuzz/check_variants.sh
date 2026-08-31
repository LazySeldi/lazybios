#!/bin/sh
# SPDX-License-Identifier: MIT
#
# Builds and runs the default, debug-logging, and MemorySanitizer variants.
#
# Usage: fuzz/check_variants.sh [seconds-per-target] [maximum-input-size]

set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)

usage() {
	echo "Usage: $0 [SECONDS_PER_TARGET] [MAX_INPUT_BYTES]"
	echo
	echo "Builds and runs the ASan/UBSan, debug-logging, and MSan variants."
	echo "Defaults: SECONDS_PER_TARGET=60, MAX_INPUT_BYTES=1048576"
	echo "Builds and saved findings are placed under the project root."
}

case ${1:-} in
	-h|--help)
		usage
		exit 0
		;;
esac

seconds=${1:-60}
max_len=${2:-1048576}

configure_and_build() {
	build=$1
	sanitizers=$2
	logging=$3

	cmake -S "$root" -B "$build" \
		-DCMAKE_C_COMPILER=clang \
		-DLAZYBIOS_BUILD_FUZZERS=ON \
		-DLAZYBIOS_FUZZ_SANITIZERS="$sanitizers" \
		-DLAZYBIOS_FUZZ_LOGGING="$logging"
	cmake --build "$build"
}

default_build=$root/build-fuzz
debug_build=$root/build-fuzz-log
msan_build=$root/build-fuzz-msan

echo "Instrumented variant check"
echo "  findings: $default_build/artifacts"
echo "  debug log: $debug_build/debug-run.log"
echo "  debug findings: $debug_build/artifacts"
echo "  MSan findings: $msan_build/artifacts"
echo

configure_and_build "$default_build" address,undefined OFF
ASAN_OPTIONS=halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 \
	"$root/fuzz/run_all.sh" "$default_build" "$seconds" "$max_len"

configure_and_build "$debug_build" address,undefined ON
mkdir -p "$debug_build/artifacts"
ASAN_OPTIONS="halt_on_error=1:log_path=$debug_build/artifacts/asan" \
UBSAN_OPTIONS="halt_on_error=1:log_path=$debug_build/artifacts/ubsan" \
	"$root/fuzz/run_all.sh" "$debug_build" "$seconds" "$max_len" \
	>"$debug_build/debug-run.log" 2>&1

configure_and_build "$msan_build" memory OFF
MSAN_OPTIONS=halt_on_error=1 \
	"$root/fuzz/run_all.sh" "$msan_build" "$seconds" "$max_len"

echo "all instrumented variants completed"
echo "debug run output: $debug_build/debug-run.log"
