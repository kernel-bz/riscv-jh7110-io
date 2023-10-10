#!/bin/bash

mkdir /sys/kernel/config/device-tree/overlays/i2c1
cat vf2-overlay-i2c1.dtbo > /sys/kernel/config/device-tree/overlays/i2c1/dtbo

mkdir /sys/kernel/config/device-tree/overlays/uart3
cat vf2-overlay-uart3.dtbo > /sys/kernel/config/device-tree/overlays/uart3/dtbo
