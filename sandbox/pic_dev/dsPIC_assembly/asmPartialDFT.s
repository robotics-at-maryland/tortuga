
	; DSP Partial DFT function
	
	; asmPartialDFT( int Log2N, fractional * pData, int Log2K, fractionl * pDest, fractcomplex * pTwid, int psvPage );
	
	.include "dspcommon.inc"
	
	.global _asmPartialDFT
	
	
.text

.bss Wsave,12
.bss Nlen,2
.bss Klen,2
.bss dataAddr,2
.bss destAddr,2
.bss twidAddr,2

; W register arguments:
; W0 - N
; W1 - dataAddr
; W2 - K
; W3 - destAddr
; W4 - twidAddr
; W5 - twidPage

_asmPartialDFT:

	; Save upper W registers
	push.d	W8
	push.d	W10
	push.d	W12
	; Save config registers
	push	CORCON
	push	PSVPAG
	
	; Configure core for fractional operations
	fractsetup	W7
	; Configure core for PSV X data
	psvaccess	W7
	mov		W5,PSVPAG		; store psv page
	mov		W4,twidAddr			; store twid offset
	
	; Set Y pointer
	mov		W1,dataAddr
	
	; Set other parameters
	mov		W0,Nlen
	mov		W2,Klen
	mov		W3,destAddr
	
	
	
		
	
	; Loop for each point in the DFT
;	mov		Klen,W4			; number of DFT points
	mov		destAddr,W13	; destination write back address
	mov		twidAddr,W8		; twid factor address
	
_DFTLoop1:
	DO		#16,_DFTLoop1end

	; Clear accumulators for this point
	clr		A
	clr		B
	
	; Go to beginning of data set
	mov		dataAddr,W10
	
	; Prefetch arguments for first loop
	mov		[W8++],W5
	mov		[W10++],W7
	
	; Loop for each point in the data set
;	mov		Nlen,W6
;	dec		W6,W6		; leave last point for last
_DFTLoop2:
	DO		#256,_DFTLoop2end	
	mac		W5*W7,A,[W8]+=2,W5				; multiply real part
_DFTLoop2end:
	mac		W5*W7,B,[W8]+=2,W5,[W10]+=2,W7	; multiply imag part

;	dec		W6,W6
;	bra		NZ,_DFTLoop2
	
	; Do last point
	mac		W5*W7,A,[W8]+=2,W5
	mac		W5*W7,B,[W13]+=2		; write back real part
_DFTLoop1end:
	sac.r	B,[W13++]				; write back imag part
	
	; loop
;	dec		W4,W4
;	bra		NZ,_DFTLoop1
	
	

	

	; Restore config registers
	pop		PSVPAG
	pop		CORCON
	; Restore upper W registers
	pop.d	W12
	pop.d	W10
	pop.d	W8
	
	return

.end

