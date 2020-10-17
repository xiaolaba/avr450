#!/bin/sh
avra -fI -m nicd.map -l nicd.lst -o nicd nicd.asm
avra lion.asm
avra nimh.asm
