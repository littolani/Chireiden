#!/bin/bash
action=$1

case $action in
    clean)
        rm -rf ./build
        mkdir ./build
        echo "Build directory cleaned and recreated."
        ;;
    build)
        mkdir -p ./build
        SRC_FILES=$(find ./src -name '*.cpp' -printf 'Z:\\\\%p ' | sed 's/\//\\/g')

        INCLUDE_FLAGS="/I Z:\\\\$(pwd | sed 's/\//\\\\/g')\\\\include"
        OUT_EXE="Z:\\\\$(pwd | sed 's/\//\\\\/g')\\\\build\\\\a.exe"

        ./cl.sh $INCLUDE_FLAGS /Fe:$OUT_EXE $SRC_FILES
        if [ $? -eq 0 ]; then
            echo "Build succeeded. Running program..."
            wine ./build/a.exe
        else
            echo "Build failed. Program not run."
            exit 1
        fi
        ;;
    *)
        echo "Usage: ./make.sh {clean|build}"
        ;;
esac

