#!/bin/bash

# Wine-style paths
VC_DIR="Z:\\mnt\\c\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC"
SDK_DIR="Z:\\mnt\\c\\Program Files (x86)\\Microsoft SDKs\\Windows\\v5.0"
IDE_DIR="Z:\\mnt\\c\\Program Files (x86)\\Microsoft Visual Studio 9.0\\Common7\\IDE"
SDK6A_LIB="Z:\\mnt\\c\\Program Files\\Microsoft SDKs\\Windows\\v6.0A\\Lib"

# These variables are used by cl.exe, not wine
export INCLUDE="$VC_DIR\\include;$SDK_DIR\\Include"
export LIB="$VC_DIR\\lib;$SDK_DIR\\Lib;$SDK6A_LIB"

# Setup Wine's DLL search path
export WINEPATH="$VC_DIR\\bin;$IDE_DIR"

echo "Running: wine $VC_DIR\\bin\\cl.exe $@"
wine "$VC_DIR\\bin\\cl.exe" "$@"

