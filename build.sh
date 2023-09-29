#!/bin/sh

# Execute the Python script to update offsets.h
python3 updater.py

# Check if the Python script was successful
if [ $? -eq 0 ]; then
    path="apex_dma/build"
    if [ -d "$path" ]; then
        cd "$path" && ninja;
    else
        cd vmread && meson "../$path" && cd "../$path" && ninja;
    fi
else
    echo "Error while running updater.py"
fi

