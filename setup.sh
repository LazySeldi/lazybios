#!/usr/bin/env bash

set -e

show_help() {
    echo "Usage: $0 {build|run|install|uninstall}"
    exit 1
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

    cmake .. \
      -DCMAKE_INSTALL_PREFIX="$INSTALL_PATH" \
      -DCMAKE_INSTALL_LIBDIR="$LIBDIR"

    make -j"$(nproc)"
    cd ..
    echo "[+] Build done"
}


run() {
    echo "[*] Running..."
    cd build || { 
        echo "Build First!"
        return 1
    }
    sudo ./lazybios_test
    echo "[+] Run finished"
}

install() {
    echo "[*] Installing..."
    cd build || { 
        echo "Build First!"
        return 1
    }
    sudo make install
    cd ..
    echo "[+] Install done"
}

uninstall() {
    echo "[*] Uninstalling..."
    cd build
    sudo make uninstall
    cd ..
    echo "[+] Uninstall done"
}

case "$1" in
    build)
        build
        ;;
    run)
        run
        ;;
    install)
        install
        ;;
    uninstall)
        uninstall
        ;;
    *)
        show_help
        ;;
esac
