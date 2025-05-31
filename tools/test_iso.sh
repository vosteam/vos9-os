#!/bin/bash

# Test script for MyOS ISO
# This script runs the ISO and captures output for testing

echo "Starting MyOS ISO test..."

# Create a temporary file for QEMU monitor commands
MONITOR_CMDS=$(mktemp)
echo "system_powerdown" > $MONITOR_CMDS

# Run QEMU with auto-boot and capture output
timeout 30 qemu-system-i386 \
    -cdrom build/myos.iso \
    -nographic \
    -no-reboot \
    -boot menu=off \
    -monitor stdio < /dev/null

rm -f $MONITOR_CMDS
echo "ISO test completed."
