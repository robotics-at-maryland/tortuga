
	; DSP Sliding DFT function
	
; MEGA NOTE:
;  * Sample buffer must be in the X data space	
	
	
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

.bss num_channels,2



.text



; W register arguments:
; W0 - coefficient buffer pointer
; W1 - window buffer pointer
; W2 - sum buffer pointer
; W3 - window length (num samples)
; W4 - num channels

_asmInitDFT:

	mov 	W0,coeff_ptr
	mov		W0,coeff_ptr_base
	mov 	W1,window_ptr
	mov 	W1,window_ptr_base
	mov 	W2,sum_ptr
	mov 	W3,window_len
	mov 	W4,num_channels
	
	; coeff_ptr_max = coeff_ptr + 4*window_len
	mul.uu	W3,#4,W6
	add		W0,W6,W6
	mov		W6,coeff_ptr_max
	
	; window_ptr_max = window_prt + 8*window_len*num_channels
	mul.uu	W3,#8,W6
	mul.uu	W6,W4,W6
	add		W1,W6,W6
	mov		W6,window_ptr_max
	
	; zero out the windows
we_loop:
	clr		[W1++]
	cp		W1,W6
	bra		EQ,we_loop

	; zero out the sums
	sl		W4,#3,W0
se_loop:
	clr		[W2++]
	dec		W0,W0
	bra		NZ,se_loop

	return
	
	
	


; W register arguments:
; W0 = new sample address

; W register contents:
; W0 = scratch
; W1 = sum pointer
; W2 = window pointer
; W3 = loop counter
; W4 = sample value
; W5 = scratch
; W6 = real coefficient
; W7 = imag coefficient
; W8 = sample pointer
; W9 = scratch
; W10 = coefficient pointer
; W11 = scratch

_asmUpdateDFT:

	; save wregs
	push.d W8
	push.d W10

	; Save config registers
	push	CORCON
	
	
	; Configure core for fractional operations
	fractsetup	W7
	
	
	; operations to complete:
	mov		W0,W8				; load sample pointer
	mov		num_channels,W3		; load loop counter
	mov		sum_ptr,W1			; load sum pointer
	mov		coeff_ptr,W10		; load coefficient pointer
	mov		window_ptr,W2		; load window pointer
	mov		[W10++],W6			; prefetch real coefficient
	mov		[W8++],W4			; prefetch sample
	mov		[W10++],W7			; prefect imag coefficient
	
channel_loop:
	
	mpy 	W4*W6,A				; multiply for new windowreal
	mpy 	W4*W7,B,[W8]+=2,W4	; multiply for new windowimag, prefetch next sample
	
	mov		ACCAL,W0			; fetch new windowreal LSW
	mov		ACCAH,W5			; fetch new windowreal MSW
	sub		W0,[W2],W9			; subtract old windowreal LSW from new windowreal LSW
	mov		W0,[W2]				; store windowreal LSW
	subb	W5,[++W2],W11		; subtract old windowreal MSW from new windowreal MSW
	add 	W9,[W1],[W1++]		; add windowsum LSW to sumreal
	mov		W0,[W2++]			; store windowreal MSW
	addc	W11,[W1],[W1++]		; add windowsum MSW to sumreal
	
	mov		ACCBL,W0			; fetch new windowimag LSW
	mov		ACCBH,W5			; fetch new windowimag MSW
	sub		W0,[W2],W9			; subtract old windowimag LSW from new
	mov		W0,[W2]				; store windowimag LSW
	subb	W5,[++W2],W11		; subtract old windowimag MSW from new
	add		W9,[W1],[W1++]		; add windowimag LSW to sumimag
	mov		W0,[W2++]			; store wimdowimag MSW
	addc	W11,[W1],[W1++]		; add windowimag MSW to sumimag
	
	dec		W3,W3				; decrement loop counter
	bra		NZ,channel_loop		; branch if counter not zero
	
	; Check for window pointer overflow
	mov		window_ptr_max,W0
	cpsne	W0,W2
	mov 	coeff_ptr_max,W10
	cpsne	W0,W2
	mov		window_ptr_base,W2
not_end:
	mov		W2,window_ptr		; this goes here to improve pipelining
	mov 	W10,coeff_ptr
	
	; Restore config registers
	pop		CORCON
	
	pop.d	W10
	pop.d	W8
	
	return



.end

