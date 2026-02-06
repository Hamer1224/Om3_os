#!/bin/bash

# 1. Clean old files
echo "Cleaning old build files..."
make clean

# 2. Compile the OS
echo "Compiling Om3 OS..."
if make; then
    echo "Build Successful!"
else
    echo "Build Failed! Check for errors above."
    exit 1
fi

            # 3. Kill existing QEMU processes
echo "Killing old QEMU sessions..."
pkill -f qemu-system-i386
            # 4. Launch QEMU in the background
echo "Launching QEMU..."
qemu-system-i386 -drive file=om3os.bin,format=raw -vnc :1 &

echo "------------------------------------------------"
echo "Done! Refresh your noVNC tab and click Connect."
echo "------------------------------------------------"