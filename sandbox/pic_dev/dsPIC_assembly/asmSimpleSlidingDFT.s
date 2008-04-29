
	; DSP Sliding DFT function
	
	; int asmSlidingDFT3( unsigned int N, unsigned int M, unsigned int P, 
	;					 fractional trigger, fractcomplex wFactor, 
	;					 fractional * pData, int * pIndex, 
	;					 unsigned long * pBufferTime, unsigned long * pTimestamps, 
	;					 unsigned long Timeout );
	; N - number of words (samples) in each channel buffer
	; M - number of words (samples) in SDFT window
	; P - number of samples to take after first/last trigger is acquired
	; trigger - DFT term magnitude that constitutes a trigger (term must be greater than this number)
	; wFactor - complex number representing the DFT term to look for.  Is found by exp(2*pi*i*k/M).
	; pData - address of start of sample data buffer of size 4*N words
	; pIndex - address at which to store the buffer index of oldest sample
	; pBufferTime - address at which to store the timestamp of the newest sample in the buffer
	; pTimestamps - address of buffer in which to return timestamp values
	; Timeout - 2 * number of samples after which to exit without receiving triggers.
	
	
	; Pinouts...
	; Configure THS1206 in signed integer mode.  Connect 12 data lines to upper bits of port.
	
	
	
	; MEGA NOTE:
	; ***  Previous versions assumed that the buffer was declared like "int buf[4][256]", 
	;         so that channel buffers are contiguous in memory.
	; ***  THIS VERSION now assumes that the buffer is declared like "int buf[256][4]",
	;         so that the channels are interleaved in the buffer.  This simplifies sampling and processing. 
	
	
	
	.include "dspcommon.inc"
	.include "p33FJ256GP710.inc"
	
	.global _asmInitDFT
	.global _asmUpdateDFT
	
	
.text


.section mydata,bss,near

.bss coeff_ptr,2		; buffer of complex fractionals
.bss window_ptr,2		; buffer of complex long fractionals
.bss window_len,2
.bss sum_ptr,2

.bss coeff_ptr_base,2
.bss window_ptr_base,2

.bss coeff_ptr_max,2
.bss window_ptr_max,2



.text



; W register arguments:
; W0 - coefficient buffer pointer
; W1 - window buffer pointer
; W2 - sum buffer pointer
; W3 - window length (num samples)

_asmInitDFT:

	mov W0,coeff_ptr
	mov	W0,coeff_ptr_base
	mov W1,window_ptr
	mov W1,window_ptr_base
	mov W2,sum_ptr
	mov W3,window_len
	
	; coeff_ptr_max = coeff_ptr + 4*window_len
	sl	W3,#2,W4
	add	W0,W4,W4
	mov	W4,coeff_ptr_max
	
	; window_ptr_max = window_prt + 8*window_len
	sl	W3,#3,W4
	add	W1,W4,W4
	mov	W4,window_ptr_max
	
	; zero out the window
me_loop:
	clr	[W1++]
	cpsne W1,W4
	bra me_done
	bra me_loop
me_done:

	return







; W register arguments:
; W0 = new sample (fractional)

; W register contents:
; W2 = window pointer
; W3 = scratch
; W4 = coefficient value
; W5 = sum pointer
; W6 = sample value
; W7 = coefficient pointer

_asmUpdateDFT:

	; Save config registers
	push	CORCON
	
	
	; Configure core for fractional operations
	fractsetup	W3
	
	
	; operations to complete:

	mov		sum_ptr,W5			; load sum pointer
	
	mov		window_ptr,W2		; load window pointer
	mov		coeff_ptr,W7		; load coefficient pointer

	mov		[W2],W3				; fetch LSW windowreal
	subr	W3,[W5],[W5++]		; subtract LSW from sumreal
	mov		[W2+2],W3			; fetch MSW windowreal
	subbr	W3,[W5],[W5--]		; subtract MSW from sumreal
	
	mov 	[W7++],W4			; prefetch real coefficient
	
	mov		W0,W6				; prefetch sample data

	mpy 	W4*W6,B				; multiply for new windowreal
	
	mov		ACCBL,W3			; fetch new windowreal LSW
	mov		W3,[W2++]			; store windowreal LSW
	add 	W3,[W5],[W5++]		; add windowreal LSW to sumreal
	mov		ACCBH,W3			; fetch new windowreal MSW
	mov		W3,[W2++]			; store windowreal MSW
	addc	W3,[W5],[W5++]		; add windowreal MSW to sumreal
	
	mov		[W2],W3				; fetch old windowimag LSW
	subr	W3,[W5],[W5++]		; subtract imag LSW from sumimag
	mov		[W2+2],W3			; fetch old windowimag MSW
	subbr	W3,[W5],[W5--]		; subtract imag MSW from sumimag
	
	mov		[W7++],W4			; prefetch imag coefficient
	
	mpy 	W4*W6,B				; multiply for new windowimag
	
	mov		ACCBL,W3			; fetch new windowimag LSW
	mov		W3,[W2++]			; store windowimag LSW
	add		W3,[W5],[W5++]		; add windowimag LSW to sumimag
	mov		ACCBH,W3			; fetch new windowimag MSW
	mov		W3,[W2++]			; store wimdowimag MSW
	addc	W3,[W5],[W5]		; add windowimag MSW to sumimag
	
	; Check for window pointer overflow
	mov window_ptr_max,W0
	cpsne W0,W2
	mov	window_ptr_base,W2
	
	; Check for coefficient pointer overflow
	mov	coeff_ptr_max,W0
	cpsne W0,W7
	mov coeff_ptr_max,W7
	mov	W2,window_ptr		; this goes here to improve pipelining
	mov W7,coeff_ptr
	
	; Restore config registers
	pop		CORCON
	
	return



.end

