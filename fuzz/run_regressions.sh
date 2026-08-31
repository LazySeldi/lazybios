#!/bin/sh
# SPDX-License-Identifier: MIT
#
# Replays every input under fuzz/regressions/ through the target that found it.
#
# Usage: fuzz/run_regressions.sh [build-directory]   (default: build-fuzz)

set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
build=${1:-$root/build-fuzz}
case $build in
	/*) ;;
	*) build=$root/$build ;;
esac

status=0
count=0

for dir in "$root"/fuzz/regressions/*/; do
	[ -d "$dir" ] || continue
	target=fuzz_$(basename "$dir")
	binary=$build/fuzz/$target

	if [ ! -x "$binary" ]; then
		echo "SKIP $target (not built in $build)" >&2
		status=1
		continue
	fi

	for input in "$dir"*; do
		[ -f "$input" ] || continue
		count=$((count + 1))
		if "$binary" "$input" >/dev/null 2>&1; then
			echo "ok   $target $(basename "$input")"
		else
			echo "FAIL $target $(basename "$input")"
			status=1
		fi
	done
done

echo "$count regression input(s) replayed"
exit $status
