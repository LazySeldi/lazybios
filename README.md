# lazybios 

### A lightweight C library for parsing SMBIOS/DMI tables on Linux systems.

### NO ITS NOT DEAD.

---
## Features

- **SMBIOS Version Detection** - Supports SMBIOS 2.x and 3.x(and future ones)
- **Clean C API** - Simple function calls, only 3 steps to assure memory-safety.
- **Zero Dependencies** - Pure C standard library, except libc.
- **Memory Safe** - Proper allocation and cleanup
- **Cross Platform** - Currently it's supported on Windows and Linux(MacOS coming next).
- **Human-readable decoders** - Convert SMBIOS bitfields and enums into readable strings with built-in helper functions.
- **Easy to integrate** - Works naturally from C, C++, and other languages capable of calling C APIs.
- **Always up-to-date** - Implemented against the latest published DMTF SMBIOS specification.

---

## Project Status
1. **Windows support:** Implemented using Windows API calls.  
   **Linux support:** Fully implemented via sysfs and /dev/mem.\
   **MacOS support:** Not implemented at all. I don't have a Mac to test this on, but I'll try to implement it.
2. Cross-platform compatibility is a goal, but the current focus is on the Linux backend and now finally Windows backend.  
3. The library is actively under development, features and structures may change.

---

## About This Project
1. Bugs may exist(but very rare, since all my releases are tested againts the SMBIOS tables in test-dumps folder), and contributions are welcome.  
2. There are no arbitrary version limits: the library implements structures according to the latest DMTF SMBIOS specification.  
3. All new structures follow the current latest [DMTF SMBIOS Specification](https://www.dmtf.org/sites/default/files/standards/documents/DSP0134_3.9.0.pdf).

---


## In here are all of the currently supported SMBIOS types:

<details>
  <summary>Implemented Types</summary>
<h3>Type 0: BIOS Information</h3>
<h3>Type 1: System Information</h3>
<h3>Type 2: Baseboard Information</h3>
<h3>Type 3: Chassis Information</h3>
<h3>Type 4: Processor Information</h3>
<h3>Type 7: Cache Information</h3>
<h3>Type 17: Memory Devices</h3>
</details>

---
## Installation
[![aur.archlinux.org.io](https://img.shields.io/aur/version/lib-lazybios)](https://aur.archlinux.org/packages/lib-lazybios)
<details>
<summary>Arch Linux</summary>

You can install lazybios from the AUR.

```shell
yay -S lib-lazybios # Or use your preferred AUR helper.
```
</details>

---
<details>
<summary>Other distros? (Community packages welcome!)</summary>

Yet to come!😞

</details>

---

<details>
<summary>Manual Building and installing with CMake.</summary>

```c
git clone https://github.com/LazySeldi/lazybios.git

cd lazybios

# Premade script
./setup.sh build
./setup.sh install # Optional, asks the user for the installation path

# Or 

# Manually building 
mkdir build

cd build

cmake .. # Will default to /usr/local unless specified with cmake -DCMAKE_INSTALL_PREFIX=/usr (or your prefeered location) ..

make

sudo make install # Optional installs to whatever location you specified in the cmake .. step
```
</details>

---
## Documentation

Generate the complete user guide and API reference with:

```shell
doxygen Doxyfile
```

The generated documentation entry point is `docs/html/index.html`.

### Heads up: When using the library you may need to read some of the current SMBIOS specification that the library uses(for example lazybios will aways use the latest specification, right now the latest is [3.9.0](https://www.dmtf.org/sites/default/files/standards/documents/DSP0134_3.9.0.pdf)) Since some fields may have some quirks. It's best to review test.c for things you're stuck on.

## Contributing:
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - Complete guide on how to contribute!

## Issues:
- **[ISSUES.md](ISSUES.md)** - Complete guide for opening issues!

## License

MIT License - see [LICENSE](LICENSE) file for details.
