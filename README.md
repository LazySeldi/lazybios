# lazybios 

### A lightweight C library for parsing SMBIOS/DMI tables on Linux systems.

---
## Features

- **SMBIOS Version Detection** - Supports SMBIOS 2.x and 3.x
- **Clean C API** - Simple function calls, only 3 steps to assure memory-safety
- **Zero Dependencies** - Pure C standard library, except libc, but that will change in the future
- **Memory Safe** - Proper allocation and cleanup
- **Cross Platform** - Currently it's supported on Windows and Linux.

---

## Project Status
1. **Windows support:** Implemented using Windows API calls.  
   **Linux support:** Currently implemented via sysfs. /dev/mem support is planned for the future.  
2. Cross-platform compatibility is a goal, but the current focus is on the Linux backend as the library is still in early development.  
3. The library is actively under development, features and structures may change.

---

## About This Project
1. This library began as a personal learning project to improve C skills. Bugs may exist, and contributions are welcome.  
2. There are no arbitrary version limits: the library implements structures according to the latest DMTF SMBIOS specification.  
3. All new structures follow the current [DMTF SMBIOS Specification](https://www.dmtf.org/sites/default/files/standards/documents/DSP0134_3.9.0.pdf).

---


## In here are all of the currently supported SMBIOS types:

<details>
  <summary><h3 style="display:inline;">Implemented Types:</h3></summary>
<h3>Type 0: BIOS Information</h3>
<h3>Type 1: System Information</h3>
<h3>Type 2: Baseboard Information</h3>
<h3>Type 3: Chassis Information</h3>
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
## For complete usage instructions and API reference guide, see:
- **[DOCS.md](DOCS.md)** - Complete usage documentation and API reference!

## Contributing:
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - Complete guide on how to contribute!

## Issues:
- **[ISSUES.md](ISSUES.md)** - Complete guide for opening issues!

## License

MIT License - see [LICENSE](LICENSE) file for details.


