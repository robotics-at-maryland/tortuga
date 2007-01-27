; *******************************************************************
; Ultrasound Beacon
;
; The purpose of this code is to oscillate a pin on a PIC16F690 in such
; a way as to simulate both the competition pinger and the practice 
; pinger that will be going during teh competition.  This pin can be used
; to drive a piezo device that we can use here at school to test our 
; robot with
;
#include <p16F690.inc>
	__config (_INTRC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _CP_OFF & _BOR_OFF & _IESO_OFF & _FCMEN_OFF)

	cblock 0x20
Delay1			; Define two file registers for the
Delay2			; delay loop
	endc

	org 0
Start
;this block takes care of disabling the analog functions on port C pins
	BANKSEL ANSEL
	movlw 7
	movwf ANSEL
	movlw h'c'
	movwf ANSELH

;this block enables the port C for digital output
	BANKSEL TRISC
	bcf	TRISC,0		; make IO Pin C0 an output
	bcf	TRISC,1		; make IO Pin C1 an output
	bcf	TRISC,2		; make IO Pin C2 an output
	bcf	TRISC,3		; make IO Pin C3 an output
	bcf	TRISC,4		; make IO Pin C4 an output
	bcf	TRISC,5		; make IO Pin C5 an output
	banksel PORTC
	clrf PORTC

;start of main program execution
loop
	
	bcf	PORTC,0		; turn off LED C0
	bsf	PORTC,0		; turn on LED C0 

	goto loop		; repeat 


OndelayLoop
	decfsz Delay1,f		; Waste time.  
	goto OndelayLoop	; The Inner loop takes 3 instructions per loop * 256 loopss = 768 instructions
	decfsz Delay2,f		; The outer loop takes and additional 3 instructions per lap * 256 loops
	goto OndelayLoop	; (768+3) * 256 = 197376 instructions / 1M instructions per second = 0.197 sec.
					; call it a two-tenths of a second.
	return

	end

