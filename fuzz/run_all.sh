#!/bin/sh
# SPDX-License-Identifier: LGPL-2.1-or-later
#
# Runs every built lazybios fuzz target with its matching corpus.
#
# Usage: fuzz/run_all.sh [build-directory] [seconds-per-target] [maximum-input-size]

set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)

usage() {
	echo "Usage: $0 [BUILD_DIR] [SECONDS_PER_TARGET] [MAX_INPUT_BYTES]"
	echo
	echo "Runs every available fuzz target with its matching seed corpus."
	echo "Defaults: BUILD_DIR=build-fuzz, SECONDS_PER_TARGET=60,"
	echo "          MAX_INPUT_BYTES=1048576"
	echo
	echo "Failure inputs are saved under BUILD_DIR/artifacts/."
}

case ${1:-} in
	-h|--help)
		usage
		exit 0
		;;
esac

build=${1:-$root/build-fuzz}
seconds=${2:-60}
max_len=${3:-1048576}

case $build in
	/*) ;;
	*) build=$root/$build ;;
esac

corpus=$build/corpus
artifacts=$build/artifacts

echo "Fuzz run"
echo "  build:     $build"
echo "  corpus:    $corpus"
echo "  findings:  $artifacts"
echo "  per target: ${seconds}s (max input: $max_len bytes)"
echo

"$root/fuzz/make_corpus.sh" "$corpus"
mkdir -p "$artifacts"

targets="dmi_table entry_point helpers backend_buffers json single_file two_files"
for name in $targets; do
	binary=$build/fuzz/fuzz_$name
	if [ ! -x "$binary" ]; then
		echo "skip fuzz_$name (not built on this platform)"
		continue
	fi

	echo "run  fuzz_$name (${seconds}s, max_len=$max_len)"
	"$binary" "$corpus/$name" \
		-max_total_time="$seconds" \
		-max_len="$max_len" \
		-artifact_prefix="$artifacts/$name-"
done

echo
echo "All available targets completed without a detected failure."
echo "Saved failure inputs, if any: $artifacts"
