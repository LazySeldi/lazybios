# lazybios 

### NO ITS NOT DEAD
### A lightweight C library for parsing SMBIOS/DMI tables on Linux systems.

---
## Features

- **SMBIOS Version Detection** - Supports SMBIOS 2.x and 3.x
- **Clean C API** - Simple, intuitive function calls
- **Zero Dependencies** - Pure C standard library
- **Memory Safe** - Proper allocation and cleanup

---

## About this project:
### 1. This library started as a personal learning project to improve my C skills. There may be bugs, and contributions are welcome!

### 2. No arbitrary version limits: this library does not stop at a specific SMBIOS version. It implements whatever the latest version of the DMTF specification provides.

### 3. When implementing new structures, this project follows the latest [DMTF SMBIOS 3.9.0 specification](https://www.dmtf.org/sites/default/files/standards/documents/DSP0134_3.9.0.pdf).


---

## In here are all of the currently supported SMBIOS types:

<details>
  <summary><h3 style="display:inline;">Implemented Types:</h3></summary>
<h3>Type 0: BIOS Information</h3>
<h3>Type 1: System Information</h3>
<h3>Type 2: Baseboard (Motherboard) Information</h3>
<h3>Type 3: Chassis (Enclosure) Information</h3>
<h3>Type 4: Processor Information</h3>
<h3>Type 7: Cache Information</h3>
<h3>Type 8: Port Connector Information</h3>
<h3>Type 10: Onboard Device Information</h3>
<h3>Type 11: OEM Strings Information</h3>
<h3>Type 16: Physical Memory Array</h3>
<h3>Type 17: Memory Devices</h3>
</details>

---
## Installation
[![aur.archlinux.org.io](https://img.shields.io/aur/version/lib-lazybios.svg)](https://aur.archlinux.org/packages/lib-lazybios)
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

