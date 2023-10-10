#!/bin/bash

gcc -O2 -o send send.c serial.c
gcc -O2 -o recv recv.c serial.c
