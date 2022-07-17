# Baremetal-NeXT
This repository includes some bare metal programming experiments for 68040-based NeXT systems. While there some open source operating systems for 68030/40-based NeXT hardware exist (e.g. NetBSD and Plan 9, there also was an early Linux port), documentation on low-level programming of the NeXT hardware is not widely available and information has to be extracted from existing OS sources or the code of the excellent [Previous emulator](http://previous.alternative-system.com). This little project is a small steps towards providing more complete information and might help to create a NeXT hardware clone or a replacement Mach kernel in the future.

The code currently only supports a mono non-Turbo 68040-based NeXTstation in Previous. It is based on NetBSD's next68k port as well as the second edition Plan 9 NeXT sources. 
The [Retro68 cross-compiler toolchain](https://github.com/autc04/Retro68) is used to build a 68k a.out *boot* file which is directly bootable via *ben* in the NeXT monitor using the emulated network in Previous 2.5.

Current functionality includes:

- Character output the the 2 bits/pixel bitmapped screen
- Interrupt handling for the timer interrupt (marked as vertical blank, auto interrupt 3 ISR bit 5), displaying a seconds counter
- Interrupt handling for keyboard and mouse (auto interrupt 3 ISR bit 22)
- Keyboard code to ASCII translation (lowercase only at the moment)
- Mouse pointer drawn using XOR

Network booting is possible since the NeXT firmware still supports the old a.out executable format in addition to Mach-O. The method to generate an a.out-file from the gcc/ld-generated ELF binary was copied from NetBSD's NeXT bootloader and is a big horrible hack...

