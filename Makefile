CC := i386-elf-gcc
ASM := nasm
CFLAGS := -std=gnu99 -nostdlib -ffreestanding -Wall -masm=intel -m32 # -O3 -lgcc
DATE := `date +"%Y-%m-%d"`

all: horizonos.iso

run: horizonos.iso
	qemu-system-i386                               		\
	-accel tcg,thread=single                       		\
	-cpu core2duo                                  		\
	-debugcon file:debug/${DATE}.log					\
	-m 4096                                         	\
	-drive format=raw,media=cdrom,file=horizonos.iso    \
	-serial stdio                                  		\
	-smp 1                                         		\
	-usb                                           		\
	-vga std 

horizonos.iso: src/kernel/kernelentry.asm Makefile rmBin
	 
	$(ASM) -f elf32 -o "bin/kernelentry.o" "src/kernel/kernelentry.asm"
	$(ASM) -f elf32 -o "bin/gdt.o" "src/kernel/GDT/gdt.asm"
	$(ASM) -f elf32 -o "bin/idt.o" "src/kernel/IDT/idt.asm"
	$(ASM) -f elf32 -o "bin/paging.o" "src/kernel/paging/paging.asm"
	 
	$(CC) -c "src/kernel/kmain.c" -o "bin/kmain.o" $(CFLAGS)
	 
	ld -T src/link.ld -m elf_i386 
	 
	mkdir -p root/boot/grub
	
	cp bin/kernel.elf root/boot/kernel.elf
	cp src/grub.cfg root/boot/grub/grub.cfg
	 
	grub-mkrescue -o horizonos.iso root

rmBin:
	rm -rf bin/*
	rm -rf root
	rm -rf src/libc/lib/*
	rm -rf initrd.tar

clean: rmBin
	rm -f horizonos.iso