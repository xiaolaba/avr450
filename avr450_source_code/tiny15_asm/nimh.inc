
;  Last change:  JB    00/07/11 (YY/MM/DD)

;**************************** MODULE INFO  *******************************
;
;   File name   :  nicd.inc
;   AUTHOR      :  Atmel
;   VERSION     :  1.2
;   DATE        :  July 10th 2000
;
;   Compiler    :  Atmel AVR Assembler 1.30
;   Contents    :  Header file for NiCD.asm, containing the battery
;                  specific definitions, functions and subroutines
;                   
;
;                   capacity * R16_17 * AD_resolution      
;   I_fast      =   _____________________________________
;                   R18 * (R8_9 + R16_17) * V_ref_voltage
;                   
;
;                   cell_voltage * cells * R16_17 * AD_resolution
;   max_V_Fast  =   _____________________________________________
;                   (R8_9 + R16_17) * V_ref_voltage                   
;
;                   
;   I_trickle   =   I_fast/40
;
;                    20mV * cells * R16_17 * AD_resolution
;   neg_dV  	=    _____________________________________________
;                   (R8_9 + R16_17) * V_ref_voltage                   


;*************************[ Battery Characteristics ]*********************
;Modify these values according to battery datasheet

.equ	cells			= 1		; number of cells in the battery pack

.equ    capacity        	= 750  		; battery pack capacity in mAh

.equ 	cell_voltage    	= 1500 		; cell voltage 1500mV

.equ    AD_ref_voltage  	= 3670      ; A/D Converter reference voltage, 3670mV
.equ    AD_resolution   	= 1024      ; A/D converter resolution, 10-bit, 1024 steps
 
.equ    min_T_abs       	= 0x02C7    ; Termination Temperature = 5 C
.equ    max_T_abs   	    = 0x0175    ; Termination Temperature = 40 C

.equ    R8_9   		        = 0		    ; Scaling resistor R8 and R9 value (short circuit)
.equ    R16_17          	= 1		    ; Scaling resistor R17 and R18 value(open)
.equ    R18             	= 4         ; Current sense resistor value 1/4 ohm(0.25)

.equ	max_time_fast		= 90		; Maximum Fast Charge Time = 1.5h
.equ	max_time_trickle	= 90		; Maximum Trickle Charge Time = 1.5h

;*************************[ Charge Conditions ]***************************

.equ	I_fast				= 52		; See formula above for calculations, no gain stage used

; Trickle Charge Current
.equ	I_trickle			= 26 		;20 * I_fast/40, 20 x gain stage used

; Fast Charge Voltage Limit
.equ	max_V_fast			= 418		; See formula above for calculations

; Trickle Charge Voltage Limit
.equ	max_V_trickle		= 418 		; Max_V_trickle = max_V_fast

; Maximum temperature rise/min
.equ	delta_T				= 10		; 0.5 C/min

;************************ General charge termination *******************

; Fast charge termination       
.equ  	neg_dV      		= 6 		; -dV = 20 mV per cell 
