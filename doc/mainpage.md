# lazybios

A lightweight, dependency-free C library for parsing SMBIOS/DMI tables.

lazybios reads the firmware tables that describe a machine — its BIOS, board,
processors, memory, slots, sensors, and more — and hands them back as plain C
structures. It implements the DMTF SMBIOS specification, plus a growing set of
vendor-specific structures.

Every parsed field comes with three views, so you never have to guess what the
firmware actually said:

| View | Example | What it tells you |
| :--- | :--- | :--- |
| raw | `t->wake_up_type` → `6` | the encoding the firmware wrote |
| `decoded` | `t->decoded.wake_up_type` → `"Power Switch"` | what that encoding means |
| `field_status` | @ref LAZYBIOS_FIELD_STATUS | whether the firmware populated it at all |

## Where to start

| If you want to… | Go to |
| :--- | :--- |
| Build the library or add it to your project | @ref building |
| See working code and the required call sequence | @ref getting_started |
| Understand ownership, cleanup, and field status | @ref core_concepts |
| Look up what a particular SMBIOS type means | @ref type_guides |
| Read a vendor-specific structure | @ref oem_type_guides |
| Know where the data comes from on each OS | @ref data_sources |
| Browse every function and structure | @ref api |

New to the library? Read @ref getting_started, then @ref core_concepts. Those
two pages cover everything needed to use the rest of the API correctly.

## A complete example

@snippet quick_start.c host-system

Every workflow follows the same three steps: create a context, load data into
it, then parse the types you care about. The context owns what it hands you, so
a single @ref lazybiosCleanup at the end releases all of it.

@ref lazybiosInit is the one loader. Two NULL paths read this machine; a single
path reads a merged dump; two paths read a separate entry point and table. The
file modes need no privileges and behave the same on every platform.

Each getter returns a result set holding an `entries` array and its `count`, so
the two can never drift apart. A set whose `count` is zero means the machine has
none of that structure — an ordinary answer, not a failure. To fill the whole
context in one call, use @ref lazybiosParseAll. The full lifecycle is described
in @ref core_concepts.

## What's covered

- **Standard structures** — SMBIOS types 0 through 46, implemented against the
  latest published DMTF specification. See @ref type_guides to find a type by
  topic, or @ref api_types for the declarations.
- **Vendor structures** — Dell and HP OEM records under `ctx->oem->dell` and
  `ctx->oem->hp`, listed in @ref oem_type_guides. They hang off a vendor because
  an OEM structure number means nothing without one: type 212 is Dell Indexed
  I/O Access and also HPE 64-bit CRU Information.
- **Decoding** — every encoded field is decoded during parsing into `decoded`,
  including multi-flag text. There are no decoder functions to call.
- **Extensions** — JSON output for every standard and OEM type, described in
  @ref extensions. @ref lazybiosParseJSONAll serializes the whole table in one
  call.

## Data sources

lazybios reads from the host system on Linux, Windows, macOS, OpenBSD, FreeBSD,
NetBSD, SunOS (Solaris/illumos), DragonFly BSD, Haiku, BeOS, ReactOS, QNX
Neutrino, and MINIX 3.

It can also parse saved table dumps, which needs no privileges and works on any
build platform. That makes dump files the practical choice for testing and for
reproducing a report from another machine. Both modes are covered in
@ref data_sources, and the platform-specific caveats in @ref limitations.

## Reliability

Parsed input is untrusted firmware data, so the project treats it that way:
seven libFuzzer targets cover the entry points, table walkers, loaders, and JSON
serializers, and a deterministic semantic suite checks specification conformance
on a corpus of real machine dumps. See @ref testing.

## Project

- @ref md_CONTRIBUTING "Contributing"
- @ref md_ISSUES "Reporting issues"

Licensed under LGPL-2.1-or-later, with a static-linking exception for
open-source projects.
