
;  Last change:  JB    00/07/11 (YY/MM/DD)

;**************************** MODULE INFO  *******************************
;
;   File name   :  LIION.INC
;   AUTHOR      :  Atmel
;   VERSION     :  1.2
;   DATE        :  July 10th 2000
;
;   Compiler    :  Atmel AVR Assembler 1.30
;   Contents    :  Header file for liion.asm, containing the battery
;                  specific definitions, functions and subroutines
;                   
;
;                   20 * capacity * R16_17 * AD_resolution      
;   I_fast      =   ______________________________________
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
;                   20 * min_I * cells * R16_17 * AD_resolution      
;   min_I_fast  =   ___________________________________________
;                   R18 * (R8_9 + R16_17) * V_ref_voltage
;
;	Change log
;	V1.1	JB		Added calculations for measurements, changed charge parameters
;								


;*************************[ Battery Characteristics ]*********************
;Modify these values according to battery datasheet

.equ    cells           = 1     	; number of cells in the battery pack

.equ    capacity        = 750       ; battery pack capacity in mAh LiIon

.equ    cell_voltage    = 4100      ; change cell voltage between 4100 mV
                                    ; and 4200 mV
.equ    AD_ref_voltage  = 3670      ; A/D Converter reference voltage, 3670mV
.equ    AD_resolution   = 1024      ; A/D converter resolution, 10-bit, 1024 steps
 
.equ    min_T_abs       = 0x02C7    ; Termination Temperature = 5 C
.equ    max_T_abs       = 0x0175    ; Termination Temperature = 40 C

.equ    R8_9            = 2200      ; Scaling resistor R8 and R9 value(ohm)
.equ    R16_17          = 10000     ; Scaling resistor R17 and R18 value(ohm)
.equ    R18             = 4         ; Current sense resistor value 1/4 ohm(0.25)

.equ    min_I           = 50        ; Terminate charge when current drops below min_I, 50mA/cell

.equ    max_time_fast   = 30        ; Maximum Fast Charge Time = 30 min
.equ    max_time_trickle = 30       ; Maximum trickle Charge Time = 90 min at 1C current

;*************************[ Charge Conditions ]***************************

; Fast Charge Current Limit
.equ    I_fast          = 871       ; See formula above for calculations

; Fast Charge Voltage Limit
.equ    max_V_fast      = 957       ; See formula above for calculations

; Trickle Charge Current Limit
.equ    I_trickle       = 22        ; See formula above for calculations

; Trickle Charge Voltage Limit
.equ    max_V_trickle   = 957       ; max_V_trickle = max_V_fast
    
;*************************[ Charge Termination Conditions ]***************

; Minimum Fast Charge Current
.equ    min_I_fast      = 58
.equ    V_tolerance     = 11