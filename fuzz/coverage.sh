#!/usr/bin/env bash
# SPDX-License-Identifier: MIT
#
# Produces an aggregate source-coverage report from every fuzz seed corpus.
#
# Configure first with:
# cmake -S . -B build-fuzz-coverage -DCMAKE_C_COMPILER=clang \
#   -DLAZYBIOS_BUILD_FUZZERS=ON -DLAZYBIOS_FUZZ_SANITIZERS= \
#   -DLAZYBIOS_FUZZ_COVERAGE=ON

set -euo pipefail

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)

usage() {
	echo "Usage: $0 [BUILD_DIR] [CORPUS_DIR] [REPORT_DIR]"
	echo
	echo "Runs every available fuzz corpus and creates an aggregate LLVM"
	echo "source-coverage report. Defaults are under build-fuzz-coverage."
}

case ${1:-} in
	-h|--help)
		usage
		exit 0
		;;
esac

build=${1:-"$root/build-fuzz-coverage"}
corpus=${2:-"$build/corpus"}
report=${3:-"$build/coverage"}

case $build in
	/*) ;;
	*) build=$root/$build ;;
esac
case $corpus in
	/*) ;;
	*) corpus=$root/$corpus ;;
esac
case $report in
	/*) ;;
	*) report=$root/$report ;;
esac

echo "Coverage run"
echo "  build:  $build"
echo "  corpus: $corpus"
echo "  report: $report"
echo

find_llvm_tool() {
	local name=$1
	local clang_major
	clang_major=$(clang --version | sed -n '1s/.*version \\([0-9][0-9]*\\).*/\\1/p')

	if command -v "$name" >/dev/null; then
		command -v "$name"
	elif [[ -n $clang_major ]] && command -v "$name-$clang_major" >/dev/null; then
		command -v "$name-$clang_major"
	else
		echo "$name is required to generate a coverage report" >&2
		return 1
	fi
}

llvm_profdata=$(find_llvm_tool llvm-profdata)
llvm_cov=$(find_llvm_tool llvm-cov)

"$root/fuzz/make_corpus.sh" "$corpus"
mkdir -p "$report"
profiles=$(mktemp -d)
trap 'rm -rf "$profiles"' EXIT

first=
objects=()
targets=(dmi_table entry_point helpers backend_buffers json single_file two_files)
for name in "${targets[@]}"; do
	binary=$build/fuzz/fuzz_$name
	if [[ ! -x $binary ]]; then
		continue
	fi

	LLVM_PROFILE_FILE="$profiles/$name-%p.profraw" \
		"$binary" "$corpus/$name" -runs=0 >/dev/null
	if [[ -z $first ]]; then
		first=$binary
	else
		objects+=(-object "$binary")
	fi
done

if [[ -z $first ]]; then
	echo "no fuzz targets found under $build/fuzz" >&2
	exit 1
fi

"$llvm_profdata" merge -sparse "$profiles"/*.profraw -o "$report/fuzz.profdata"
"$llvm_cov" report "$first" "${objects[@]}" \
	-instr-profile="$report/fuzz.profdata" \
	-ignore-filename-regex='(/fuzz/|/test/)'
"$llvm_cov" show "$first" "${objects[@]}" \
	-instr-profile="$report/fuzz.profdata" \
	-ignore-filename-regex='(/fuzz/|/test/)' \
	-format=html -output-dir="$report/html"

echo "HTML coverage report: $report/html/index.html"
