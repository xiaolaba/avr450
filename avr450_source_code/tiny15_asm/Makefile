# --
# Copyright (c) 2020, xiaolaba
# --

MCU=attiny13

#FUSE_L=0x6A
#FUSE_H=0xFF

F_CPU=1200000

# assembler AVRA
AS=avra

AVRDUDE=avrdude

#TARGET=main

#ASFLAGS= -I ../avrasm/inc
# AVRA installed device header files
ASFLAGS= -I /usr/share/avra



# 3 files to be assembled
SRCS= nimh nicd lion sttn12

# make each asm file on pass arguments
all: $(SRCS)
$(SRCS): %: %.asm
	${AS} ${ASFLAGS} -fI -m $@.map -l $@.lst -o $@.hex $<
flash:
	${AVRDUDE} -p ${MCU} -c usbtiny -U flash:w:${TARGET}.hex:i

fuse:
	$(AVRDUDE) -p ${MCU} -c usbtiny -U hfuse:w:${FUSE_H}:m -U lfuse:w:${FUSE_L}:m

clean:
	rm -f *.asm~ *.hex *.obj *.cof *.lst *.map
