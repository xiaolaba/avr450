# --
# Copyright (c) 2020, xiaolaba, test one more time
# --

MCU=attiny13

#FUSE_L=0x6A
#FUSE_H=0xFF

F_CPU=1200000

# assembler AVRA
AS=avra

AVRDUDE=avrdude

#TARGET=main
#ASFLAGS= -I ../../asm/include


# 3 files to be assembled
SRCS := nimh nicd lion sttn12

# make each asm file on pass arguments
all: $(SRCS)
$(SRCS): %: %.asm
	${AS} ${ASFLAGS} -fI -m $@.map -l $@.lst -o $@ $<
    
flash:
	${AVRDUDE} -p ${MCU} -c usbasp -B10 -U flash:w:${TARGET}.hex:i -F -P usb

fuse:
	$(AVRDUDE) -p ${MCU} -c usbasp -B10 -U hfuse:w:${FUSE_H}:m -U lfuse:w:${FUSE_L}:m

clean:
	rm -f *.asm~ *.hex *.obj *.cof *.lst *.map