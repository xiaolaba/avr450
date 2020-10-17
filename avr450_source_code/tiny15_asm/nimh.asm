
;  Last change:  JB    00/07/11 (YY/MM/DD)
 
;**************************** MODULE INFO  *******************************
;
;   File name   :  nimh.asm
;   AUTHOR      :  Atmel
;   VERSION     :  1.2
;   DATE        :  July 10th 2000
;
;   Compiler    :  Atmel AVR Assembler 1.30
;   Contents    :  Travel Battery Charger Reference Design with ATtiny15
;		   - Battery specific functions:
;                  - fast_charge
;                  - trickle_charge
;   Options:
;	- Watchdog timer 
;	- No battery detection
;
;   Please send comments on this code to avr@atmel.com
;
;	Change log
;	V1.1		JB	Changed fast I_fast measurement to measure without gain
;								
;	V1.2		JB	Fixed bug when stopping PWM output

;*************************[ Included Define Files ]***********************
; Define files included here 

.include	"tn15def.inc"
.include   	"bc.inc" 
.include 	"nimh.inc"
 
;*************************[ Interrupt Vector Table ]**********************
; Execution always starts here

	.org 0x00
	ldi			temp, 0x05
 	out			TCCR0, temp				; TCNT0 CK/1024
 	ldi			temp, 0x41				; TCNT1 8-bit PWM @ 100 kHz
 	out			TCCR1, temp				; output DISCONNECTED
	rjmp		fast_charge

	.org OVF0addr

;*****************[ Timer 0 Overflow Interrupt Handler ]******************
Timer0_OVF_Int:
	in			SREG_Storage, SREG		; Store SREG Contents

	dec			tick_cnt				; Decrement tick Counter
	brne		t0ovf_exit
	; This code is executed once a second	
	ldi			tick_cnt, sec_duration	; Reset Tick Counter
	; *** Insert user code here (approx 1 second)
	inc			t_sec					; Increment second counter
	cpi			t_sec, min_duration
	brne		t0ovf_exit				; Test second overflow
	; This code is executed once a minute
	clr			t_sec					; Clear Second Counter
	; *** Insert user code here (approx 1 minute)
	inc			t_min					; Increment Minutes Counter
	
t0ovf_exit:
	out			SREG, SREG_Storage		; Restore SREG
	reti

;***********************[ PWM Control ]*********************************
; Functions to start and stop PWM output
start_PWM:
 	ldi			temp, 0x61
 	out			TCCR1, temp				; TCNT1 8-bit PWM @ 100 kHz
	ret

stop_PWM:
	cbi			PORTB, PWM_PIN			; Lock current to zero
	sbi			DDRB, PWM_PIN			
	ldi			temp, 0x41
	out			TCCR1, temp				; Disconnect PWM output
	ret


;*******************[ Fast Charge: Adjust Charge Current ]****************
;  Function to Adjust Charge Current
adj_current:	
	cp			Il, temp
	cpc			Ih, temp2				; if( I = I_fast )
	breq		adj_charge_end			; skip	
	cp			Il, temp
	cpc			Ih, temp2				; if( I < I_fast )
	brsh		f_dec		
					
f_inc:	
	in			temp, OCR1A				; increment charge current
	subi		temp, 0xFF				; if( OCR1A < 0xFF )
	brcc		adj_charge_end	
	out			OCR1A, temp
	rjmp		adj_charge_end			; Exit if limit value

f_dec:	
	in			temp, OCR1A				; decrement charge current
	subi		temp, 0x01				; if( OCR1A > 0x00 )
	brcs		adj_charge_end
	out			OCR1A, temp				; Exit if limit value
adj_charge_end:
	ret

	
;*******************[function F_measure: Measure all Parameters ]***************
; Measure Charge Parameters
f_measure:

	ldi			temp,ADC_V_channel		; Select Voltage Channel
	rcall		convert_average			; Measure Battery Charge Voltage
	mov			Vl, av_l
	mov			Vh, av_h				; Record value

	ldi			temp,ADC_T_channel		; Select Temperature Channel
	rcall		convert_average			; Measure Battery Temperature
	mov			Tl, av_l
	mov			Th, av_h				; Record value

;	ldi			temp,ADC_gain_channel	; Select Current Channel
;	rcall		convert_average			; Measure Battery Current
;	mov			Il, av_l
;	mov			Ih, av_h				; Record value
	ret

	
;*************************[ ADC Handler ]*********************************
; This function measures the channel selected by [temp], and returns the 
; converted value in [av_h:av_l]
convert_average:
	out			ADMUX, temp				; Set ADC Channel
	clr			av_l
	clr			av_h					; Clear Average Registers
	ldi			temp2,(1<<avg_loop_cnt) ; Set loop counter
convert_start:
	ldi		 	temp,0xD4
	out			ADCSR,temp				; Start A/D Conversions
convert_wait:
	sbis		ADCSR, ADIF
	rjmp		convert_wait			; Wait for Conversion to finish
	in			temp, ADCL
	add			av_l, temp
	in			temp, ADCH
	adc			av_h, temp				; Add measured value to average
	dec			temp2
	brne		convert_start			; Repeat (1<<avg_loop_cnt) times
	ldi			temp2, avg_loop_cnt		; Set shift counter
convert_avg:	
	lsr			av_h
	ror			av_l					; av_h:av_l = av_h:av_l/2
	dec			temp2
	brne		convert_avg				; Repeat (avg_loop_cnt) times
	cbi			ADCSR,ADEN
	ret									; return to measure

;*******************[Check Temperature ]********************
; checking temperature, terminate if outside range

check_temperature:
	ldi			temp, high(max_T_abs)
	cpi			Tl, low(max_T_abs)		; Temperature too high? (T_max)
	cpc			Th, temp
	brlo		f_terminate_error		; Yes, terminate and flag error
	
	ldi			temp, high(min_T_abs)
	cpi			Tl, low(min_T_abs)		; Temperature too low? (T_min)
	cpc			Th, temp
	brsh		f_terminate_error		; Yes, terminate and flag error
	ret
	
;*******************[ Error handling ]***************************************
; If error, the program ends up here

f_terminate_error:
	rcall		stop_PWM	
error_loop:
	rjmp		error_loop


;*******************[ Fast Charge: Reset ]********************************
; Start Fast Charge Cycle
; Reset Charge Parameters
fast_charge:
	ldi			temp, 0x65
	out			0x3D, temp				; Initialize Stack (emulator only)
 	ldi			temp,0xFF
 	out 		OCR1B,temp
 	ldi			temp, 0x02
 	out			TIMSK, temp				; Enable TCNT0 overflow interrupt
  	cbi			PORTB, PWM_PIN			; Set Port B levels
  	sbi			DDRB, PWM_PIN			; Set Port B directions
	ldi			tick_cnt, sec_duration	; Tick counter = 0
	clr			last_t_min
	sei									; global interrupt enable
	ldi			t_min, 0x00
	ldi			t_sec, 0x00				; Reset Fast Charge Timer
	out			OCR1A, t_sec			; Reset PWM counter

	rcall		start_PWM
	rcall 		f_measure

	
;*******************[ Fast Charge: initialize temperature and voltage ]********
minute_init:
	mov			last_t_min,t_min		; initialize values for checking every minute
	mov			last_min_Vl,Vl
	mov			last_min_Vh,Vh
	mov			last_min_Tl,Tl
	mov			last_min_Th,Th
	
;*******************[ Fast Charge: Check Temperature ]********************
charge_loop:
	rcall 		f_measure				; Measure battery
	ldi			temp,ADC_diff_channel	; Select Current Channel
	rcall		convert_average			; Measure Battery Current
	mov			Il, av_l
	mov			Ih, av_h				; Record value

	rcall		check_temperature
	
;*******************[ Fast Charge: Check Voltage ]********************
	ldi			temp, high(max_V_fast)
	cpi			Vl, low(max_V_fast)
	cpc			Vh, temp				; if( V > max_V_fast )
	brsh		f_terminate_error		; Yes, terminate and flag error

;*******************[ Fast Charge: Adjust Charge Current ]****************
;  Load the charge current values and call the adjust charge current function

	ldi			temp,low(I_fast)
	ldi			temp2,high(I_fast) 		; Load current value and adjust current 
	rcall		adj_current
	
;*******************[ Fast Charge: Timeout Test ]***********************
; Check timeout value
f_terminate_test:
	cpi			t_min, max_time_fast
	brsh		f_terminate_error		; timeout (test minutes only)

;*******************[ Fast Charge: Minute loop ]***********************

	cp			last_t_min,t_min		; Check for 60 sec overflow
	brsh 		charge_loop				; If new minute
	
;	cbi 		PORTB,PWM_PIN			; Included for debug with emulator
	
	sub			last_min_Tl,Tl			; Check for temperature change
	sbc			last_min_Th,Th	
		
	ldi			temp,low(delta_T)		; Load max temperature change value
	ldi			temp2,high(delta_T)	
	sub			last_min_Tl,temp		; Terminate if temperature rise more than 1C/min
	sbc			last_min_Th,temp2
	brge		f_terminate
	
	sub			last_min_Vl,Vl			; Subtract last minutes voltage
	sbc			last_min_Vh,Vh
	
	ldi			temp,low(neg_dV)		; Check if voltage drops
	ldi				temp2,high(neg_dV)
	sub			last_min_Vl,temp
	sbc			last_min_Vh,temp2
	brge		f_terminate
	
	rjmp		minute_init				; Reload last minute values
		
;*******************[ Fast Charge: Terminate ]****************************
;  Terminate Fast Charge Cycle
f_terminate:
	rcall		stop_PWM				; Disable PWM Output
										; Continue with Trickle Charge
;*******************[ Trickle Charge: Reset ]*****************************
; Reset Charge Parameters
	ldi			temp, 0x00
	out			OCR1A, temp				; Minimum Duty Cycle

;*******************[ Trickle Charge: Measure all Parameters ]************
; Measure Charge Parameters

	ldi			t_min, 0x00
	ldi			t_sec, 0x00				; Reset Trickle Charge Timer
	rcall		start_PWM				; Start PWM

;*******************[ Trickle Charge: Check Temperature ]*****************
t_charge_loop:
	rcall 		f_measure
	ldi			temp,ADC_gain_channel	; Select Current Channel
	rcall		convert_average			; Measure Battery Current
	mov			Il, av_l
	mov			Ih, av_h				; Record value

	ldi			temp,ADC_g_offset_channel		; Select Current offset Channel
	rcall		convert_average			; Measure offset value
	sub			Il, av_l				; Subtract offset from measured value
	sbc			Ih, av_h
	brpl		pos					
	clr			Il						; Reset if negative value
	clr			Ih
pos:rcall		check_temperature

;*******************[ Trickle Charge: Check Voltage ]*****************

	ldi			temp, high(max_V_trickle)
	cpi			Vl, low(max_V_trickle)
	cpc			Vh, temp				; V>(max_V_fast)?
	brsh		t_terminate_error		; Yes, terminate and flag error


;*******************[ Trickle Charge: Adjust Charge Current ]*************
;  Adjust Charge Current

	ldi			temp, low(I_trickle)
	ldi			temp2, high(I_trickle)
	rcall		adj_current
	rjmp		t_charge_loop

;*******************[ Trickle Charge: Check timeout ]*************
;  Check timeout 
	cpi			t_min, max_time_fast
	brsh		t_terminate_error		; timeout (test minutes only)
	
t_terminate_error:
	rjmp		f_terminate_error
