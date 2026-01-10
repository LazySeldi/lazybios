#!/usr/bin/env bash

set -e

while true; do
    read -p "Is this script being ran from the root of lazybios(IMPORTANT)? (y/n): " yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) return 1 2>/dev/null || exit 1;;
        * ) echo "Please answer 'y' or 'n'.";;
    esac
done

show_help() {
    echo "Usage: $0 {build|run|run_dump|install|uninstall|clean|source_test|source_test_valgrind}"
    exit 1
}

clean() {
    echo "[*] Cleaning..."
    if [ -e "build/lazybios_test" ]; then
        cd build
        rm -rf ./*
        cd ..
    else 
        echo "Build First!"
        return 1
    fi
    echo "[+] Clean done"
}

build() {
    echo "[*] Building..."
    mkdir -p build 
    cd build 

    read -rp "Enter install path for CMake [/usr]: " INSTALL_PATH
    INSTALL_PATH=${INSTALL_PATH:-/usr}

    echo "[*] Configuring with:"
    echo "    PREFIX  = $INSTALL_PATH"
    echo

    cmake .. -DCMAKE_INSTALL_PREFIX="$INSTALL_PATH"
    make -j"$(nproc)"
    cd ..
    echo "[+] Build done"
}

run() {
    echo "[*] Running..."
    if [ -e "build/lazybios_test" ]; then
        sudo build/lazybios_test
    else
        echo "Build First!"
        return 1
    fi
    echo "[+] Run finished"
}

run_dump() {
    echo "[*] Dumping SMBIOS data..."
    if [ -e "build/lazybios_test" ]; then
        sudo build/lazybios_test -dump
    else
        echo "Build First!"
        return 1
    fi
    echo "[+] Dumped SMBIOS data to the current directory!"
}

install() {
    echo "[*] Installing..."
    if [ -e "build/lazybios_test" ]; then
        cd build
        sudo make install
        cd ..
    else 
        echo "Build First!"
        return 1
    fi
    echo "[+] Install done"
}

uninstall() {
    echo "[*] Uninstalling..."
    if [ -e "build/lazybios_test" ]; then
        cd build
        sudo make uninstall
        cd ..
    else 
        echo "Build First!"
        return 1
    fi
    echo "[+] Uninstall done"
}

source_test() {
    echo "[*] Running source tests..."
    if [ ! -e "build/lazybios_test" ]; then
        echo "Build First!"
        return 1
    fi

    for dir in test-dumps/*; do
        if [ -d "$dir" ]; then
            smbios_entry_point="$dir/smbios_entry_point"
            DMI="$dir/DMI"
            if [ -f "$smbios_entry_point" ] && [ -f "$DMI" ]; then
                echo "[*] Testing folder $dir..."
                build/lazybios_test --sources "$smbios_entry_point" "$DMI"
            fi
        fi
    done
    echo "[+] Source tests finished"
}

source_test_valgrind() {
    echo "[*] Running source tests under Valgrind..."
    if [ ! -e "build/lazybios_test" ]; then
        echo "Build First!"
        return 1
    fi

    for dir in test-dumps/*; do
        if [ -d "$dir" ]; then
            smbios_entry_point="$dir/smbios_entry_point"
            DMI="$dir/DMI"
            if [ -f "$smbios_entry_point" ] && [ -f "$DMI" ]; then
                echo "[*] Testing folder $dir..."
                VALGRIND_OUT=$(valgrind --leak-check=full --error-exitcode=1 build/lazybios_test --sources "$smbios_entry_point" "$DMI" 2>&1 || true)
                
                if echo "$VALGRIND_OUT" | grep -q "ERROR SUMMARY: 0 errors"; then
                    echo "$dir TESTED: NO ERRORS"
                else
                    echo "$dir TESTED: ERRORS DETECTED"
                    echo "$VALGRIND_OUT"
                fi
            fi
        fi
    done
    echo "[+] Valgrind source tests finished"
}

case "$1" in
    build) build ;;
    clean) clean ;;
    run) run ;;
    run_dump) run_dump ;;
    install) install ;;
    uninstall) uninstall ;;
    source_test) source_test ;;
    source_test_valgrind) source_test_valgrind ;;
    *) show_help ;;
esac

