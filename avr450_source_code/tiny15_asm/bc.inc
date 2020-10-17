
;  Last change:  JB    00/07/11 (YY/MM/DD)

;**************************** MODULE INFO  *******************************
;
;   File name   :  BC.INC
;   AUTHOR      :  Atmel
;   VERSION     :  1.2
;   DATE        :  July 10th 2000
;
;   Compiler    :  Atmel AVR Assembler 1.30
;   Contents    :  Header file for Travel Charger Reference Design
;


;*************************[ Charger  Characteristics ]********************

.equ	sec_duration		= 6	; 1 Second (#Timer0 Overflow Interrupts)
.equ	min_duration		= 61	; ~1 Minute (1.6 MHz = 59,965 sec)
;.equ	min_duration		= 6	; ~1 Minute (1.6 MHz = 6 sec), debug

.equ	avg_loop_cnt		= 4	; ADC Measurement Average Loop Count
								; NOTE! Count = (1<<avr_loop_cnt) !!!

;*************************[ Hardware Assignments ]************************

.equ	PWM_PIN			= PB1	; PWM pin

.equ	ADC_T_channel		=0x41	; Battery Temperature Channel
.equ	ADC_I_channel		=0x42	; Battery Current Channel
.equ	ADC_V_channel		=0x43	; Battery Voltage Channel

.equ	ADC_offset_channel	=0x44	; ADC Offset Channel
.equ	ADC_g_offset_channel	=0x45	; ADC gain Offset Channel
.equ	ADC_diff_channel	=0x46	; ADC Diff Channel
.equ	ADC_gain_channel	=0x47	; ADC gain Channel

;*************************[ Register Assignments ]************************

.def	SREG_Storage		=r1	; Temporary Storage During Interrupts

.def	av_l			=r5	; Average Low Byte (ADC Measurement)
.def	av_h			=r6	; Average High Byte (ADC Measurement)
.def	last_min_Vl		=r10	; Last minute Voltage Low Byte
.def	last_min_Vh		=r11	; Last minute Voltage High Byte
.def	last_min_Tl		=r12	; Last minute Temperature Low Byte
.def	last_min_Th		=r13	; Last minute Temperature High Byte
.def	last_t_min		=r14	; Last minute tick counter

.def	temp			=r16	; Temporary Storage Register 1
.def	temp2			=r17	; Temporary Storage Register 2
.def	tick_cnt		=r18	; Tick Counter
.def	t_sec			=r19	; Time_seconds
.def	t_min			=r20	; Time_minutes
.def	control			=r21	; Charge control register
.def	Vl			=r22	; Voltage Low Byte
.def	Vh			=r23	; Voltage High Byte
.def	Il			=r24	; Current Low Byte
.def	Ih			=r25	; Current High Byte
.def	Tl			=r26	; Temperature Low Byte
.def	Th			=r27	; Temperature High Byte
.def	chargel			=r28	; Temp variable, used only in Liion
.def	chargeh			=r29	; Temp variable, used only in Liion


;*************************[ Charge Control ]******************************

.equ	NO_PWM			=0x01
