
	; DSP Partial DFT function
	
	; asmPartialDFT( int Log2N, fractional * pData, int Log2K, fractionl * pDest, fractcomplex * pTwid, int psvPage );
	
	.include "dspcommon.inc"
	
	.global _asmSlidingDFT
	
	
.text


.bss Nlen,2
.bss localBuf,16
.bss kValue,16
.bss kMags,8		; must be contiguous after kValue
.bss wFactor,4
.bss trigger,2

; W register arguments:
; W0 - N
; W1 - bufAdr
; W2 - re(W)
; W3 - im(W)
; W4 - trigger

_asmSlidingDFT:

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

	
	mov		W0,Nlen			; store number of points
	mov.d	W2,wFactor		; store complex wFactor
	mov		W4,trigger		; store trigger value
	
	mov		W0,W4			; move N out of the way
	mov		W1,W0			; store bufPtr(0)
	add		W0,W4,W1		; generate bufPtr(1)
	add		W1,W4,W2		; generate bufPtr(2)
	add		W2,W4,W3		; generate bufPtr(3)
	
	
BigLoop:
	
	; Wait for data to arrive
	btsc	D_AV_PIN
	bra		BigLoop
	
	
	; >> 32 INSTRUCTIONS
	; Collect the data
	bclr	RD_PIN
	mov		DATA_PORT,W4
	bset	RD_PIN
	mov		W4,localBuf
	bclr	RD_PIN
	mov		DATA_PORT,W4
	bset	RD_PIN
	mov		W4,localBuf+1
	bclr	RD_PIN
	mov		DATA_PORT,W4
	bset	RD_PIN
	mov		W4,localBuf+2
	bclr	RD_PIN
	mov		DATA_PORT,W4
	bset	RD_PIN
	mov		W4,localBuf+3
	bclr	RD_PIN
	mov		DATA_PORT,W4
	bset	RD_PIN
	mov		W4,localBuf+4
	bclr	RD_PIN
	mov		DATA_PORT,W4
	bset	RD_PIN
	mov		W4,localBuf+5
	bclr	RD_PIN
	mov		DATA_PORT,W4
	bset	RD_PIN
	mov		W4,localBuf+6
	bclr	RD_PIN
	mov		DATA_PORT,W4
	bset	RD_PIN
	mov		W4,localBuf+7
	
	
	
	; >>> 21 INSTRUCTIONS
	; Subtract the old data
	; old data is stored in location bufferPtr
	
	; Load kValue pointer
	mov		&kValue,W7
	
	; channel 0
	mov		[W7],W4			; get the real part of the current k value
	sub		W4,[W0],W4		; subtract old data stored in buffer @ W4
	mov		localBuf+0,W5	; get new data, put in W1
	mov		W5,[W0++]		; store new data in buffer @ W4, increment W4.  Need to do circular buffering later.
	add		W4,W5,[W7++]	; add new data to kValue and store
	
	; channel 1
	mov		[++W7],W4		; get the real part of the current k value
	sub		W4,[W1],W4		; subtract old data stored in buffer @ W4
	mov		localBuf+2,W5	; get new data, put in W1
	mov		W5,[W1++]		; store new data in buffer @ W4, increment W4.  Need to do circular buffering later.
	add		W4,W5,[W7++]	; add new data to kValue and store
	
	; channel 2
	mov		[++W7],W4		; get the real part of the current k value
	sub		W4,[W2],W4		; subtract old data stored in buffer @ W4
	mov		localBuf+4,W5	; get new data, put in W1
	mov		W5,[W2++]		; store new data in buffer @ W4, increment W4.  Need to do circular buffering later.
	add		W4,W5,[W7++]	; add new data to kValue and store
	
	; channel 3
	mov		[++W7],W4		; get the real part of the current k value
	sub		W4,[W3],W4		; subtract old data stored in buffer @ W4
	mov		localBuf+6,W5	; get new data, put in W1
	mov		W5,[W3++]		; store new data in buffer @ W4, increment W4.  Need to do circular buffering later.
	add		W4,W5,[W7]		; add new data to kValue and store
	
	
	; >>> 14 INSTRUCTIONS
	; perform loop addressing
	mov		NLen,W4			; get buffer length in W0
	dec		W4,W5			; mask for looping bits in W1
	
	; channel 0
	and		W5,W0,W6		; check buffer 0
	btsc	Z				; check if lower bits are zero
	sub		W0,W4,W0		; subtract loop N from pointer if overflowed
	
	; channel 1
	and		W5,W1,W6		; check buffer 1
	btsc	Z				; check if lower bits are zero
	sub		W1,W4,W1		; subtract loop N from pointer if overflowed
	
	; channel 2
	and		W5,W2,W6		; check buffer 2
	btsc	Z				; check if lower bits are zero
	sub		W2,W4,W2		; subtract loop N from pointer if overflowed
	
	; channel 3
	and		W5,W3,W6		; check buffer 3
	btsc	Z				; check if lower bits are zero
	sub		W3,W4,W3		; subtract loop N from pointer if overflowed
	
	
	
	; >>> 26 INSTRUCTIONS
	; Multiply new k values by wFactor
	; Both numebers are complex
	; The formula we need is a*c-b*d, a*d+b*c
	; We are lucky because one operand is at a constant address, and the others are in contiguous addresses.
	
	; fetch wFactor
	mov.d	wFactor+0,W4		; real and imag part of wFactor (2 cycles)
	
	; set operand addresses
	mov		&kValue,W8			; real part address of kValues
	mov		W8,W13				; write-back address of kValue
	
	; prefetch channel 0 operands
	mov.d	[W8++],W6			; real and imag part of kValue(0) (2 cycles)
	
	; channel 0 multiply
	mpy.n	W5,W7,A							; real = -b*d
	mpy		W5,W6,B							; imag = b*c
	mac		W4,W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(1)) to W6
	mac		W4,W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(1)) to W6, write back real part to kValue
	clr		A,[W13]+=2						; write back imag part to kValue
	
	; channel 1 multiply
	mpy.n	W5,W7,A							; real = -b*d
	mpy		W5,W6,B							; imag = b*c
	mac		W4,W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(2)) to W6
	mac		W4,W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(2)) to W6, write back real part to kValue
	clr		A,[W13]+=2						; write back imag part to kValue
	
	; channel 2 multiply
	mpy.n	W5,W7,A							; real = -b*d
	mpy		W5,W6,B							; imag = b*c
	mac		W4,W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(3)) to W6
	mac		W4,W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(3)) to W6, write back real part to kValue
	clr		A,[W13]+=2						; write back imag part to kValue
	
	; channel 3 multiply
	mpy.n	W5,W7,A							; real = -b*d
	mpy		W5,W6,B							; imag = b*c
	mac		W4,W6,A							; real += a*c
	mac		W4,W7,B,[W13]+=2				; imag += a*d, write back real part to kValue
	clr		A,[W13]							; write back imag part to kValue
	
	

	; Add second batch of samples
	; >>> 21 INSTRUCTIONS
	; Subtract the old data
	; old data is stored in location bufferPtr
	
	; Load kValue pointer
	mov		&kValue,W7
	
	; channel 0
	mov		[W7],W4			; get the real part of the current k value
	sub		W4,[W0],W4		; subtract old data stored in buffer @ W4
	mov		localBuf+0,W5	; get new data, put in W1
	mov		W5,[W0++]		; store new data in buffer @ W4, increment W4.  Need to do circular buffering later.
	add		W4,W5,[W7++]	; add new data to kValue and store
	
	; channel 1
	mov		[++W7],W4		; get the real part of the current k value (post and pre-increment to skip imag part in array)
	sub		W4,[W1],W4		; subtract old data stored in buffer @ W4
	mov		localBuf+2,W5	; get new data, put in W1
	mov		W5,[W1++]		; store new data in buffer @ W4, increment W4.  Need to do circular buffering later.
	add		W4,W5,[W7++]	; add new data to kValue and store
	
	; channel 2
	mov		[++W7],W4		; get the real part of the current k value (post and pre-increment to skip imag part in array)
	sub		W4,[W2],W4		; subtract old data stored in buffer @ W4
	mov		localBuf+4,W5	; get new data, put in W1
	mov		W5,[W2++]		; store new data in buffer @ W4, increment W4.  Need to do circular buffering later.
	add		W4,W5,[W7++]	; add new data to kValue and store
	
	; channel 3
	mov		[++W7],W4		; get the real part of the current k value (post and pre-increment to skip imag part in array)
	sub		W4,[W3],W4		; subtract old data stored in buffer @ W4
	mov		localBuf+6,W5	; get new data, put in W1
	mov		W5,[W3++]		; store new data in buffer @ W4, increment W4.  Need to do circular buffering later.
	add		W4,W5,[W7]		; add new data to kValue and store
	
	
	; >>> 14 INSTRUCTIONS
	; perform loop addressing
	mov		NLen,W4			; get buffer length in W4
	dec		W4,W5			; mask for looping bits in W5
	
	; channel 0
	and		W5,W0,W6		; check buffer 0
	btsc	Z				; check if lower bits are zero
	sub		W0,W4,W0		; subtract N from pointer if overflowed
	
	; channel 1
	and		W5,W1,W6		; check buffer 1
	btsc	Z				; check if lower bits are zero
	sub		W1,W4,W1		; subtract N from pointer if overflowed
	
	; channel 2
	and		W5,W2,W6		; check buffer 2
	btsc	Z				; check if lower bits are zero
	sub		W2,W4,W2		; subtract N from pointer if overflowed
	
	; channel 3
	and		W5,W3,W6		; check buffer 3
	btsc	Z				; check if lower bits are zero
	sub		W3,W4,W3		; subtract N from pointer if overflowed
	
	
	
	; >>> 26 INSTRUCTIONS
	; Multiply new k values by wFactor
	; Both numebers are complex
	; The formula we need is a*c-b*d, a*d+b*c
	; We are lucky because one operand is at a constant address, and the others are in contiguous addresses.
	
	; fetch wFactor
	mov.d	wFactor+0,W4		; real and imag part of wFactor (2 cycles)
	
	; set operand addresses
	mov		&kValue,W8			; real part address of kValues
	mov		W8,W13				; write-back address of kValue
	
	; prefetch channel 0 operands
	mov.d	[W8++],W6			; real and imag part of kValue(0) (2 cycles)
	
	; channel 0 multiply
	mpy.n	W5,W7,A							; real = -b*d
	mpy		W5,W6,B							; imag = b*c
	mac		W4,W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(1)) to W6
	mac		W4,W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(1)) to W6, write back real part to kValue
	clr		A,[W13]+=2						; write back imag part to kValue
	
	; channel 1 multiply
	mpy.n	W5,W7,A							; real = -b*d
	mpy		W5,W6,B							; imag = b*c
	mac		W4,W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(2)) to W6
	mac		W4,W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(2)) to W6, write back real part to kValue
	clr		A,[W13]+=2						; write back imag part to kValue
	
	; channel 2 multiply
	mpy.n	W5,W7,A							; real = -b*d
	mpy		W5,W6,B							; imag = b*c
	mac		W4,W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(3)) to W6
	mac		W4,W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(3)) to W6, write back real part to kValue
	clr		A,[W13]+=2						; write back imag part to kValue
	
	; channel 3 multiply
	mpy.n	W5,W7,A							; real = -b*d
	mpy		W5,W6,B							; imag = b*c
	mac		W4,W6,A							; real += a*c
	mac		W4,W7,B,[W13]+=2				; imag += a*d, write back real part to kValue
	clr		A,[W13]+=2						; write back imag part to kValue
	
	
	
	; >>> 16 INSTRUCTIONS
	; Now norm the vectors
	
	; load addresses
	mov		&kValues,W8			; address of kValue array
	mov		&kMags,W13			; write-back address for magnitudes
	
	; fetch first values
	mov.d	[W8++],W4			; kValues(0) real and imag part (2 cycles)

	; channel 0 norm
	mpy		W4*W4,A,[W8]+=2,W4				; square real part, prefetch next real part
	mac		W5*W5,A,[W8]+=2,W5				; square and sum imag part, prefetch next imag part
	clr		A,[W13]+=2						; write back to kMags
	
	; channel 1 norm
	mpy		W4*W4,A,[W8]+=2,W4				; square real part, prefetch next real part
	mac		W5*W5,A,[W8]+=2,W5				; square and sum imag part, prefetch next imag part
	clr		A,[W13]+=2						; write back to kMags
		
	; channel 2 norm
	mpy		W4*W4,A,[W8]+=2,W4				; square real part, prefetch next real part
	mac		W5*W5,A,[W8]+=2,W5				; square and sum imag part, prefetch next imag part
	clr		A,[W13]+=2						; write back to kMags
	
	; channel 3 norm
	mpy		W4*W4,A							; square real part
	mac		W5*W5,A							; square and sum imag part
	clr		A,[W13]							; write back to kMags
	

	; ^^^ 170 instruction so far above


	; Compare the norms to the trigger level
	
	
	
	
	
	goto	BigLoop
	
	
	

	

	; Restore config registers
	pop		PSVPAG
	pop		CORCON
	; Restore upper W registers
	pop.d	W12
	pop.d	W10
	pop.d	W8
	
	return

.end

