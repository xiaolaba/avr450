
;  Last change:  JB    00/07/11 (YY/MM/DD)
 
;**************************** MODULE INFO  *******************************
;
;   File name   :  bclion.asm
;   AUTHOR      :  Atmel
;   VERSION     :  1.2
;   DATE        :  July 10th 2000
;
;   Compiler    :  Atmel AVR Assembler 1.30
;   Contents    :  Travel Battery Charger Reference Design with ATtiny15
;                    
;	Change log
;	V1.2		JB	Fixed bug when stopping PWM output

;*************************[ Included Define Files ]***********************
; Define files included here 
; Note: Battery Specific Functions are included at the end of this file

.include	"tn15def.inc"
.include    	"bc.inc"
.include 	"liion.inc"				;Battery spec.


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

;*************************[ PWM Control ]*********************************
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

;*******************[function F_measure: Measure all Parameters ]***************
; Measure Charge Parameters

f_measure:
	ldi			temp,ADC_T_channel		; Select Temperature Channel
	rcall		convert_average			; Measure Battery Temperature
	mov			Tl, av_l
	mov			Th, av_h				; Record value

	ldi			temp,ADC_gain_channel		; Select Current Channel
	rcall		convert_average			; Measure Battery Current
	mov			Il, av_l
	mov			Ih, av_h				; Record value

	sbrc		control,NO_PWM			; Check control 
	rcall		stop_PWM				; Stop PWM to measure battery voltage, 
	ldi			temp,ADC_V_channel		; Select Voltage Channel
	rcall		convert_average			; Measure Battery Charge Voltage
	mov			Vl, av_l
	mov			Vh, av_h				; Record value
	rcall		start_PWM

;	ldi			temp,ADC_g_offset_channel		; Select Current Channel
;	rcall		convert_average	
	
	ret

	
;*************************[ ADC Handler ]*********************************
; This function measures the channel selected by [temp], and returns the 
; converted value in [av_h:av_l]

convert_average:
	out			ADMUX, temp				; Set ADC Channel

	clr			av_l
	clr			av_h					; Clear Average Registers
	ldi			temp2,(1<<avg_loop_cnt) 	; Set loop counter
 	ldi		 	temp,0xD4
	out			ADCSR,temp

convert_start:
	sbi			ADCSR, ADSC				; Start A/D Conversions

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
	
f_terminate_error:
	rcall		stop_PWM	
error_loop:
	rjmp		error_loop


;*******************[ Adjust Charge Current  ]****************
;  Adjust Charge Current

adj_charge:
	cp			temp, chargel
	cpc			temp2,chargeh
	brlo		f_dec	

	cp			chargel,temp			; if( I < I_fast )
	cpc			chargeh,temp2	
	brsh		f_end					; increase charge current
f_inc:	
	in			temp, OCR1A				; increment charge current
	subi		temp, 0xFF				; if( OCR1A < 0xFF )
	brcc		f_end
	out			OCR1A, temp
	rjmp		f_end
f_dec:
	in			temp, OCR1A				; decrement charge current
	subi		temp, 0x01				; if( OCR1A > 0x00 )
	brcs		f_end
	out			OCR1A, temp
f_end:ret


;*******************[Check Temperature ]********************
; checking temperature and time, terminate if outside range

check_temperature:
	ldi			temp, high(max_T_abs)
	cpi			Tl, low(max_T_abs)		; Temperature too high? (T_max)
	cpc			Th, temp
	brlo		f_terminate_error		; Yes, terminate and flag error
	
	ldi			temp, high(min_T_abs)
	cpi			Tl, low(min_T_abs)		; Temperature too low? (T_min)
	cpc			Th, temp
	brsh		f_terminate_error		; Yes, terminate and flag error
	
	cpi			t_min, max_time_fast
	brsh		f_terminate_error		; timeout (test minutes only)

	ret
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
	ldi			control, 0x00			; No events active
	ldi			tick_cnt, sec_duration		; Tick counter = 0
	clr			last_t_min
	sei									; global interrupt enable
	ldi			control, NO_PWM			; Set charge control to measure battery voltage
	ldi			t_min, 0x00
	ldi			t_sec, 0x00				; Reset Fast Charge Timer
	ldi			temp, 0x00
	out			OCR1A, temp				; Minimum Duty Cycle

	rcall		start_PWM
	
;*******************[ Fast Charge: Check battery]********************
charge_loop:
	rcall 		f_measure
	rcall		check_temperature	

;*******************[ Fast Charge: Adjust Charge Current ]****************
;  Adjust Charge Voltage and Current
		
	ldi			temp,low(I_fast)
	ldi			temp2, high(I_fast)
	mov			chargel,Il
	mov			chargeh,Ih 
	rcall		adj_charge
	 
;*******************[ Fast Charge: Terminate Test ]***********************
f_terminate_test:

	ldi			temp, high(max_V_fast)
	cpi			Vl, low(max_V_fast)
	cpc			Vh, temp					; if( V > V_fast )
	brlo		charge_loop					; terminate fast charge

;*******************[ Fast Charge: Constant voltage loop ]***********************

	ldi			control, 0x00				; Clear Charge Control Register
fV_voltage_loop:
	rcall		f_measure			

	ldi			temp,ADC_g_offset_channel	; Select Current offset Channel
	rcall		convert_average				; Measure offset value
	sub			Il, av_l					; Subtract offset from measured value
	sbc			Ih, av_h
	brpl		pos					
	clr			Il							; Reset if negative value
	clr			Ih	
pos:	
	ldi			temp,low(max_V_fast)
	ldi			temp2,high(max_V_fast) 		; Load constant V value
	mov			chargel,Vl			
	mov			chargeh,Vh
	rcall		adj_charge					; Adjust charge voltage

;*******************[ Fast Charge: Terminate test, constant voltage loop ]***********************

	rcall		check_temperature
		
	ldi			temp,high(min_I_fast) 		; if( I < I_min )
	cpi			Il,low(min_I_fast)
	cpc			Ih,temp
	brsh		fV_voltage_loop	
	
;*******************[ Fast Charge: Terminate ]****************************
;  Terminate Fast Charge Cycle

	rcall		stop_PWM					; Disable PWM Output
											; Continue with Trickle Charge
;*******************[ Trickle Charge ]************************************
; Reset Charge Parameters
	ldi			control, NO_PWM				; Set charge control to measure battery voltage
	ldi			t_min, 0x00
	out			OCR1A, t_min				; Minimum Duty Cycle

	rcall		start_PWM					; Start PWM

;*******************[ Trickle Charge: Check Temperature ]*****************

t_charge_loop:
	rcall 		f_measure
	rcall		check_temperature	

;*******************[ Trickle Charge: Adjust Charge Current ]*************
;  Adjust charge Current

	ldi			temp, low(I_trickle)
	ldi			temp2, high(I_trickle)		; Load trickle charge current
	mov			chargel,Il
	mov			chargeh,Ih			
	rcall		adj_charge					; Adjust charge current
	
;*******************[ trickle Charge: Terminate Test ]***********************
t_terminate_test:

	ldi			temp, high(max_V_trickle)
	cpi			Vl, low(max_V_trickle)
	cpc			Vh, temp					; if( V > V_trickle )
	brlo		t_charge_loop				; terminate trickle charge

;*******************[ trickle Charge: Constant voltage loop ]***********************

	ldi			control, 0x00				; Clear Charge Control Register
tV_voltage_loop:
	rcall		f_measure		 	

	ldi			temp,low(max_V_trickle)
	ldi			temp2,high(max_V_trickle)
	mov			chargel,Vl
	mov			chargeh,Vh
	rcall		adj_charge	

;*******************[ Trickle Charge: Terminate test, constant voltage loop ]***********************

tV_terminate_test:
	
	ldi			temp,high(min_I_fast) 		; if( I < I_fast )
	cpi			Il,low(min_I_fast)
	cpc			Ih,temp
	brsh		tV_voltage_loop	

t_terminate:
	rcall		stop_PWM
done:	rjmp 	done		
	
