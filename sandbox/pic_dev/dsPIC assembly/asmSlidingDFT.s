
	; DSP Sliding DFT function
	
	; asmSlidingDFT( int Log2N, fractional * pData, fractcomplex wFactor, fractional trigger );
	
	.include "dspcommon.inc"
	
	.global _asmSlidingDFT
	
	
.text


.bss localBuf,16
.bss kValue,16		; must be in near memory
.bss kMags,8		; must be contiguous after kValue
.bss wFactor,4
.bss trigger,2
.bss timestamp,16
.bss trigFlag,2
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
	
	
	
	
	; Persistent WREG assignments:
	; W0 is scratch register
	; W1 is buffer base address
	; W2 is buffer length N
	; W3 is buffer index, range 0 to N-1
	; W4 is wFactor real, dsp
	; W5 is wFactor imag, dsp
	; W6 is scratch register, dsp
	; W7 is scratch register, dsp
	; W8 is scratch register, dsp
	; W9 is index loop mask (N-1)
	; W10 is not used
	; W11 is not used
	; W12 is not used
	; W13 is scratch register, dsp
	; W14 is frame pointer
	; W15 is stack pointer
	
	
	
	mov.d	W2,W4		; store complex wFactor
	
	
	clr		W3				; start at buffer index 0
	sl		W0,W2			; shift left one bit to get number of bytes instead of words
	dec		W2,W9			; generate bit mask for buffer index looping
	mov		W4,trigger		; store trigger value
	
	mov		#0xF,W0
	mov		W0,trigFlag		; trigger count, counting down
		
BigLoop:
	
	; Wait for data to arrive
	btsc	D_AV_PIN
	bra		BigLoop
	
	; TOTAL INSTRUCTIONS EXECUTED AFTER THIS POINT:
	; >>> 177
	
	
	; >> 32 INSTRUCTIONS
	; >> uses W0 scratch
	; Collect the data
	bclr	RD_PIN
	mov		DATA_PORT,W0
	bset	RD_PIN
	mov		W0,localBuf
	bclr	RD_PIN
	mov		DATA_PORT,W0
	bset	RD_PIN
	mov		W0,localBuf+1
	bclr	RD_PIN
	mov		DATA_PORT,W0
	bset	RD_PIN
	mov		W0,localBuf+2
	bclr	RD_PIN
	mov		DATA_PORT,W0
	bset	RD_PIN
	mov		W0,localBuf+3
	bclr	RD_PIN
	mov		DATA_PORT,W0
	bset	RD_PIN
	mov		W0,localBuf+4
	bclr	RD_PIN
	mov		DATA_PORT,W0
	bset	RD_PIN
	mov		W0,localBuf+5
	bclr	RD_PIN
	mov		DATA_PORT,W0
	bset	RD_PIN
	mov		W0,localBuf+6
	bclr	RD_PIN
	mov		DATA_PORT,W0
	bset	RD_PIN
	mov		W0,localBuf+7
	
	
	
	; >>> 23 INSTRUCTIONS
	; Subtract the old data and loop buffer index
	; old data is stored in location bufferPtr
	
	; W0 is new data
	; W1 is base pointer
	; W2 is N
	; W3 is index
	; W13 is current buffer address
	; W8 is new data - old data
	; W7 is address of kValue
	
	; Load kValue pointer
	mov		&kValue,W7
	add		W1,W3,W13			; index off of base address for channel 0 buffer
	
	; channel 0
	mov		localBuf+0,W0		; get new data
	sub		W0,[W13],W8		; subtract old data from new data
	add		W8,[W7],[W7++]		; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer address by N to next channel buffer
	
	; channel 1
	mov		localBuf+2,W0		; get new data
	sub		W0,[W13],W8		; subtract old data from new data
	add		W8,[++W7],[W7++]	; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer address by N to next channel buffer
	
	; channel 2
	mov		localBuf+4,W0		; get new data
	sub		W0,[W13],W8		; subtract old data from new data
	add		W8,[++W7],[W7++]	; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer address by N to next channel buffer
	
	; channel 3
	mov		localBuf+6,W0		; get new data
	sub		W0,[W13],W8		; subtract old data from new data
	add		W8,[++W7],[W7]		; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	
	
	; Perform looping
	inc2	W3					; increment buffer index
	and		W3,W9,W3			; clear high bits
	
	
	
	; >>> 24 INSTRUCTIONS
	; Multiply new k values by wFactor
	; Both numebers are complex
	; The formula we need is a*c-b*d, a*d+b*c
	; We are lucky because one operand is at a constant address, and the others are in contiguous addresses.
	
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
	
	

	; Add second batch of samples
	; >>> 23 INSTRUCTIONS
	; Subtract the old data and loop buffer index
	; old data is stored in location bufferPtr
	
	; W0 is new data
	; W1 is base pointer
	; W2 is N
	; W3 is index
	; W13 is current buffer address
	; W8 is new data - old data
	; W7 is address of kValue
	
	; Load kValue pointer
	mov		&kValue,W7
	add		W1,W3,W13			; index off of base address for channel 0 buffer
	
	; channel 0
	mov		localBuf+8,W0		; get new data
	sub		W0,[W13],W8		; subtract old data from new data
	add		W8,[W7],[W7++]		; add difference to kValue
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer address by N to next channel buffer
	
	; channel 1
	mov		localBuf+10,W0		; get new data
	sub		W0,[W13],W8		; subtract old data from new data
	add		W8,[++W7],[W7++]	; add difference to kValue
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer address by N to next channel buffer
	
	; channel 2
	mov		localBuf+12,W0		; get new data
	sub		W0,[W13],W8		; subtract old data from new data
	add		W8,[++W7],[W7++]	; add difference to kValue
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer address by N to next channel buffer
	
	; channel 3
	mov		localBuf+14,W0		; get new data
	sub		W0,[W13],W8		; subtract old data from new data
	add		W8,[++W7],[W7]		; add difference to kValue
	mov		W0,[W13]			; store new data in buffer
	
	
	; Perform looping
	inc2	W3					; increment buffer index
	and		W3,W9,W3			; clear high bits
	
	
	
	; >>> 24 INSTRUCTIONS
	; Multiply new k values by wFactor
	; Both numebers are complex
	; The formula we need is a*c-b*d, a*d+b*c
	; We are lucky because one operand is at a constant address, and the others are in contiguous addresses.
	
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
	
	
	
	; >>> 15 INSTRUCTIONS
	; Now norm the vectors
	
	; load addresses
	mov		&kValues,W8			; address of kValue array
	
	; fetch first values
	mov.d	[W8++],W6			; kValues(0) real and imag part (2 cycles)

	; channel 0 norm
	mpy		W6*W6,A,[W8]+=2,W6				; square real part, prefetch next real part
	mac		W7*W7,A,[W8]+=2,W7				; square and sum imag part, prefetch next imag part
	clr		A,[W13]+=2						; write back to kMags
	
	; channel 1 norm
	mpy		W6*W6,A,[W8]+=2,W6				; square real part, prefetch next real part
	mac		W7*W7,A,[W8]+=2,W7				; square and sum imag part, prefetch next imag part
	clr		A,[W13]+=2						; write back to kMags
		
	; channel 2 norm
	mpy		W6*W6,A,[W8]+=2,W6				; square real part, prefetch next real part
	mac		W7*W7,A,[W8]+=2,W7				; square and sum imag part, prefetch next imag part
	clr		A,[W13]+=2						; write back to kMags
	
	; channel 3 norm
	mpy		W6*W6,A							; square real part
	mac		W7*W7,A							; square and sum imag part
	clr		A,[W13]							; write back to kMags
	


	; >>> 33 INSTRUCTIONS
	; Compare the norms to the trigger level
	mov		&kMags,W8			; address of kMags in W8
	mov		&TMR8,W6
	mov		&TMR9HLD,W7
	mov		&timestamp,W13
	mov		trigger,W0

	btsc	trigFlag,0			; check if this channel has triggered already
	cpslt	W0,[W8++]			; compare trigger to kMag for channel 0
	bra		_apdCh0Skip			; returns at _apdChXDone label
	mov		[W6],[W13++]		; get timestamp low word
	mov		[W7],[W13++]		; get timestamp high word
	bclr	trigFlag,0			; decrement trigger counter
	nop
_apdCh0Done

	btsc	trigFlag,1			; check if this channel has triggered already
	cpslt	W0,[W8++]			; compare trigger to kMag for channel 0
	bra		_apdCh1Skip			; returns at _apdChXDone label
	mov		[W6],[W13++]		; get timestamp low word
	mov		[W7],[W13++]		; get timestamp high word
	bclr	trigFlag,1			; decrement trigger counter
	nop
_apdCh1Done

	btsc	trigFlag,2			; check if this channel has triggered already
	cpslt	W0,[W8++]			; compare trigger to kMag for channel 0
	bra		_apdCh2Skip			; returns at _apdChXDone label
	mov		[W6],[W13++]		; get timestamp low word
	mov		[W7],[W13++]		; get timestamp high word
	bclr	trigFlag,2			; decrement trigger counter
	nop
_apdCh2Done

	btsc	trigFlag,3			; check if this channel has triggered already
	cpslt	W0,[W8++]			; compare trigger to kMag for channel 0
	bra		_apdCh3Skip			; returns at _apdChXDone label
	mov		[W6],[W13++]		; get timestamp low word
	mov		[W7],[W13++]		; get timestamp high word
	bclr	trigFlag,3			; decrement trigger counter
	nop
_apdCh3Done

	; >>> LOOP OVERHEAD: 3 INSTRUCTIONS
	; Check if we have all four triggers
	cp0		trigFlag			; exit loop if all four triggers are there.
	bra		NZ,BigLoop
	
	
	; OUT OF BIG LOOP!!! GOT ALL FOUR TRIGGERS
	
	

	

	; Restore config registers
	pop		PSVPAG
	pop		CORCON
	; Restore upper W registers
	pop.d	W12
	pop.d	W10
	pop.d	W8
	
	return


;-----------------

_apdCh0Skip
	add		#4,W13,W13
	bra		_apdCh0Done
	
_apdCh1Skip
	add		#4,W13,W13
	bra		_apdCh1Done

_apdCh2Skip
	add		#4,W13,W13
	bra		_apdCh2Done

_apdCh3Skip
	add		#4,W13,W13
	bra		_apdCh3Done
	


.end

