# HorizonOS-v4
 A hobby operating system written in C for the x86 (i386 and newer) cpu

## Dependencies
 To build this os, you need to install (on Ubuntu):
 ```bash
 sudo apt-get update            # Update the packages

 sudo apt install gcc           # GCC
 sudo apt install nasm          # NASM
 sudo apt install grub2-common  # GRUB
 sudo apt install qemu          # Qemu (optionally for running the os)
 ```

## Building
 After resolving the dependencies, simply run the makefile :
 ```bash
 make all -s
 ```

## Running
 To run it, you can write the disk image to an usb key and boot it on a real computer or just run it in a virtual machine