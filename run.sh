#!/bin/bash
LOG_FILE="build.log"

echo "🚀 [DevOps] Starting automated build..." | tee $LOG_FILE

# Step 1: Linting/Check (Optional but professional)
if ! command -v nasm &> /dev/null || ! command -v gcc &> /dev/null; then
    echo "❌ Dependencies missing!" | tee -a $LOG_FILE
    exit 1
fi

        # Step 2: Build
make clean >> $LOG_FILE 2>&1
if make >> $LOG_FILE 2>&1; then
    echo "✅ Build Successful at $(date)" | tee -a $LOG_FILE
                
                    # Step 3: Deployment (Restarting QEMU)
    pkill -f qemu-system-i386
    qemu-system-i386 -drive file=om3os.bin,format=raw -vnc :1 &
    echo "🌐 OS Deployed to VNC." | tee -a $LOG_FILE
else
    echo "❌ BUILD FAILED! See build.log for details." | tee -a $LOG_FILE
    tail -n 10 $LOG_FILE
    exit 1
fi