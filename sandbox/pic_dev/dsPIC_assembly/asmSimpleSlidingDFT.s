
	; DSP Sliding DFT function
	
; MEGA NOTE:
;  * Sample buffer must be in the X data space	
	
	
	.include "dspcommon.inc"
	.include "p33FJ256GP710.inc"
	
	.global _asmInitDFT
	.global _asmUpdateDFT
	
	
	.def FRACT_MODE
	;.def INT_MODE
	
	
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

.bss corcon_save,2



.text


; W register arguments:
; W0 - coefficient buffer pointer
; W1 - window buffer pointer
; W2 - sum buffer pointer
; W3 - window length (num samples)
; W4 - num channels
; W5 - math mode

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
	sl		W4,#0x3,W0
se_loop:
	clr		[W2++]
	dec		W0,W0
	bra		NZ,se_loop
	
	; set up corecon
	mov		CORCON,W0
	mov		W0,corcon_save
	; CORCON = xxxm0xxx 1110ss0m
	and		#0x00C,W0
	ior		#0x0E0,W0
	ior		W0,W5,W0
	mov		W0,CORCON

	return


_asmExitDFT:
	; restore corcon
	mov		corcon_save,W0
	mov		W0,CORCON
	
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

	; Initialize working registers
	mov		W0,W8				; load sample pointer
	mov		num_channels,W3		; load loop counter
	mov		sum_ptr,W1			; load sum pointer
	mov		coeff_ptr,W10		; load coefficient pointer
	mov		window_ptr,W2		; load window pointer
	mov		[W10++],W6			; prefetch real coefficient
	mov		[W8++],W4			; prefetch sample
	mov		[W10++],W7			; prefect imag coefficient
	
	; Loop for each channel
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
	mov		window_ptr_max,W0	; get max window address
	cpsne	W0,W2				; compare
	mov 	coeff_ptr_base,W10	; reset coefficient pointer
	cpsne	W0,W2				; compare
	mov		window_ptr_base,W2	; reset window pointer
not_end:
	mov		W2,window_ptr		; store window pointer
	mov 	W10,coeff_ptr		; store coefficient pointer
	
	; Restore working registers
	pop.d	W10
	pop.d	W8
	
	return



.end

