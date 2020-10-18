;******************************************************
; 38 kHz switching and 19 kHz pilot carrier generator for FM stereo modulator.
	
;******************************************************
.nolist						;Don't list defitiions 
;.include "tn25def.inc" 
;.include "2323def.inc"   
;.include "tn12def.inc"
;.include    "tn15def.inc"
.include    "tn13def.inc"
.list
;******************************************************

;sttn12

;************* MEMO OF I/O PORT ASSIGNMENTS *************

;Port B pin assignments
	;B0		inverted 38 kHz switch to ground
	;B1		19 kHz output
	;B2		38 KHz switch to ground


;************* DEFINE FEGISTER USAGE *************
.def	temp =	r16						 	;Working register.
				

;************* ESTABLISH CONSTANTS ************* 

	.equ	ISwitch38	= 0				;Bit 4 on portb is the inverted 38 kHz switch to ground.
	.equ	Switch38 	= 2				;Bit 6 of portb will have 38 kHz switch to ground.
	.equ	Sig19 		= 1 				;Bit 5 of portb is the 19 kHz pilot signal.


;************* START EXECUTABLE CODE ************* 

.org $0000
	rjmp 	start    				;Vector not needed but it is here for uniformity of implimentation.


start:									;COMES HERE AFTER POWER-ON RESET
;	ldi 	r16,RAMEND      			;Init Stack Pointer
;	out 	SPL,r16						;****NOTE - NO STACK POINTER FOR ATTINY12. BE SURE TO INIT STACK FOR OTHERS!
	ldi		temp,0b00000010
	out		ddrb,temp
	out		portb,temp
	
		
main:									;Start main loop - generate 38 kHz switching and 19 kHz signal.										
	sbi portb,Sig19						;2 cycles	19 kHz high
	cbi	ddrb,Switch38					;2 cycles	38kHz open
	sbi	ddrb,ISwitch38					;2 cycles	Inverted 38 kHz closed

	ldi	temp, 24										
dly1:	
	dec temp
	brne dly1
	nop
	nop
	nop
	sbi	ddrb,Switch38					;2 cycles   38 kHz closed
	cbi	ddrb,ISwitch38					;2 cycles	Inverted 3 kHz open


									
	ldi	temp,23										
dly2:	
	dec temp
	brne dly2
	nop
	nop
	nop
	nop

	cbi portb,Sig19						;2 cycles	19 kHz low
	cbi	ddrb,Switch38					;2 cycles   38 kHz open
	sbi	ddrb,ISwitch38					;2 cycles	Inverted 38 kHz closed
	nop

	ldi	temp, 24										
dly3:	
	dec temp
	brne dly3
	nop
	nop
	sbi	ddrb,Switch38					;2 cycles   38 kHz closed
	cbi	ddrb,ISwitch38					;2 cycles	Inverted 3 kHz open

									
ldi	temp, 23										
dly4:	
	dec temp
	brne dly4
	nop
	nop	
	rjmp	main;						;2 Cycles


.exit									;Assembler will stop at this line.

