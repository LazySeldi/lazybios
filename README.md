# lazybios 

### A lightweight C library for parsing SMBIOS/DMI tables on Linux systems.

## Features

- **SMBIOS Version Detection** - Supports SMBIOS 2.x and 3.x
- **Hardware Information** - BIOS, System, Chassis, Processor, Memory
- **Comprehensive CPU Detection** - Intel Xeon, Core series, AMD Ryzen, and more
- **Clean C API** - Simple, intuitive function calls
- **Zero Dependencies** - Pure C standard library
- **Memory Safe** - Proper allocation and cleanup

## About this project:
This library was first made as a learning experience for me to learn C and get better at it, there are probably bugs in this program so please point them out so I can fix them or just provide me with the fix if you want. I really don't know how to manage contributions so tell me in issues. Thank you. 

------------------
## Installation
<details>
<summary>Arch Linux</summary>

You can install lazybios from the AUR.

```c
yay -S lib-lazybios # Or use your prefeered AUR helper.
```
</details>

<details>
<summary>Other distros? (Community packages welcome!)</summary>

Yet to come!😞

</details>

<details>
<summary>Manual Building and installing with CMake.</summary>

Clone and build it yourself!

```c
git clone https://github.com/LazySeldi/lazybios.git

cd lazybios

mkdir build

cd build

cmake ..

make

sudo make install # Optional and will install to /usr unless specified
```
</details>

---------------------
## For complete usage instructions and API reference guide, see:
- **[DOCS.md](DOCS.md)** - Complete usage documentation and API reference!

## Contributing:
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - Complete guide on how to contribute!

## Issues:
- **[ISSUES.md](ISSUES.md)** - Complete guide for opening issues!

## License

MIT License - see [LICENSE](LICENSE) file for details.

## Thanks 
Big thanks to @caleb(calebh101) on Discord for being my first ever user and github star!