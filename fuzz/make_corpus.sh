#!/bin/sh
# SPDX-License-Identifier: LGPL-2.1-or-later
#
# Builds seed corpora for the lazybios fuzz targets out of test-dumps/.
#
# Usage: fuzz/make_corpus.sh [output-directory]   (default: build/corpus)

set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
out=${1:-$root/build/corpus}

dumps=$root/test-dumps
if [ ! -d "$dumps" ]; then
	echo "no test-dumps directory at $dumps" >&2
	exit 1
fi

mkdir -p "$out/dmi_table" "$out/entry_point" "$out/single_file" "$out/two_files" "$out/decoders"

for dir in "$dumps"/*/; do
	name=$(basename "$dir")

	# fuzz_dmi_table consumes three selector bytes followed by the raw table.
	if [ -f "$dir/DMI" ]; then
		{ printf '\003\003\000'; cat "$dir/DMI"; } > "$out/dmi_table/$name"
	fi

	if [ -f "$dir/smbios_entry_point" ]; then
		cp "$dir/smbios_entry_point" "$out/entry_point/$name"
	fi

	# fuzz_single_file wants the entry point and table concatenated.
	if [ -f "$dir/$name.bin" ]; then
		cp "$dir/$name.bin" "$out/single_file/$name"
	elif [ -f "$dir/smbios_entry_point" ] && [ -f "$dir/DMI" ]; then
		cat "$dir/smbios_entry_point" "$dir/DMI" > "$out/single_file/$name"
	fi

	# fuzz_two_files splits at a leading byte holding the entry point size.
	if [ -f "$dir/smbios_entry_point" ] && [ -f "$dir/DMI" ]; then
		{
			printf "$(printf '\\%03o' "$(wc -c < "$dir/smbios_entry_point")")"
			cat "$dir/smbios_entry_point" "$dir/DMI"
		} > "$out/two_files/$name"
	fi
done

# fuzz_decoders takes an opaque byte stream; a handful of lengths is enough.
i=0
while [ "$i" -lt 8 ]; do
	head -c $((16 + i * 37)) /dev/urandom > "$out/decoders/seed-$i"
	i=$((i + 1))
done

echo "seed corpora written under $out"
