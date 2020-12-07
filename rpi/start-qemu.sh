#!/bin/bash

# Start emulation with appropriate settings
sudo qemu-system-arm -kernel qemu-rpi-kernel-master/kernel-qemu-4.4.34-jessie \
-append "root=/dev/sda2 panic=1 rootfstype=ext4 rw" -hda raspbian-stretch-lite.qcow \
-cpu arm1176 -m 256 -M versatilepb -no-reboot -serial stdio \
-net nic,macaddr=00:16:3e:00:00:01 -net tap,ifname=tap0,script=no,downscript=no