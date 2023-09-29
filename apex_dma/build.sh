#!/bin/bash

# Execute the Python script to update offsets.h
python3 updater.py

# Check if the Python script was successful
if [ $? -eq 0 ]; then
    cd memflow_lib/memflow-win32-ffi/
    
    if cargo build --release ; then
        cd ../memflow-qemu-procfs
    
        if cargo build --release --all-features ; then
            cd ../../
            make
        else
            echo "Error while building memflow-qemu-procfs"
        fi
    
    else
        echo "Error while building memflow-win32-ffi"
    fi
else
    echo "Error while running updater.py"
fi
