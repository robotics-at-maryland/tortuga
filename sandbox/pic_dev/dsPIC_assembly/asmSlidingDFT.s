
	; DSP Sliding DFT function
	
	; asmSlidingDFT( int Log2N, int Log2M, fractional * pData, fractional trigger, fractcomplex wFactor );
	
	
	
	; Pinouts...
	; Configure THS1206 in signed integer mode.  Connect 12 data lines to upper bits of port.
	
	
	
	
	
	
	.include "dspcommon.inc"
	.include "p33FJ256GP710.inc"
	
	.global _asmSlidingDFT
	
	
.text

.bss nLen,2
.bss bufAdr,2
.bss localBuf,16
.bss kValue,16		; must be in near memory
.bss kMags,8		; must be contiguous after kValue
.bss wFactor,4
.bss trigger,2
.bss timestamp,16
.bss trigFlag,2
.bss mLen,2




; W register arguments:
; W0 - N (buffer length)
; W1 - M (window length)
; W2 - bufAdr
; W3 - trigger
; W4 - re(W)
; W5 - im(W)

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
	; W3 is buffer write index, range 0 to N-1
	; W4 is wFactor real, dsp
	; W5 is wFactor imag, dsp
	; W6 is scratch register, dsp
	; W7 is scratch register, dsp
	; W8 is scratch register, dsp
	; W9 is index loop mask (N-1)
	; W10 is scratch register
	; W11 is not used
	; W12 is buffer read index
	; W13 is scratch register, dsp
	; W14 is frame pointer
	; W15 is stack pointer
	
	
	; Store all the arguments in RAM before loading them back into registers
	mov		W0,nLen
	mov		W1,mLen
	mov		W2,bufAdr
	mov		W3,trigger
	mov		W4,wFactor		; wFactor stays here
	mov		W5,wFactor+2	; wFactor stays here
	
	mov		bufAdr,W1		; buffer base address
	mov		nLen,W2
	sl		W2,#1,W2		; buffer length, byte addressed
	clr		W3				; buffer write index
	dec		W2,W9			; buffer index looping mask
	sl		mLen,WREG
	mov		W0,mLen			; window length, byte addressed
	neg		W0,W0
	and		W0,W9,W12		; buffer read index
	
	mov		#0xF,W0
	mov		W0,trigFlag		; trigger flags, all set
	
	; Initialize the DFT
	; All we have to do is zero out the entire data buffer and the dft terms
	;   so that the dft terms are consistent with the data.  All zeros satisfies that.
	mov		#kValue,W0
	repeat	#7				; clear out all 8 kValue registers
	clr		[W0++]
	mov		W1,W0			; get address of data buffer
	sl		W2,#2,W6		; multiply N by 4 to get total length of buffer
	dec		W6,W6			; decrement to get REPEAT argument
	repeat	W6				; clear entire array
	clr		[W0++]






BigLoop:
	
	; Wait for data to arrive
	; >> 2 INSTRUCTIONS MINIMUM, 3 INSTRUCTIONS IF WE JUST MISS THE EDGE
	btsc	PORTA,#5
	bra		BigLoop
	
	; TOTAL INSTRUCTIONS EXECUTED AFTER THIS POINT:
	; 3 + 26 + 29 + 24 + 29 + 24 + 15 + 37 + 3 = 190
	
	
	; >> 26 INSTRUCTIONS
	; >> uses W0 scratch
	; Collect the data
	mov		#DATA_PORT,W6
	mov		#localBuf,W7
	
	bclr	PORTA,#4
	mov		[W6],[W7++]
	bset	PORTA,#4
	bclr	PORTA,#4
	mov		[W6],[W7++]
	bset	PORTA,#4
	bclr	PORTA,#4
	mov		[W6],[W7++]
	bset	PORTA,#4
	bclr	PORTA,#4
	mov		[W6],[W7++]
	bset	PORTA,#4
	bclr	PORTA,#4
	mov		[W6],[W7++]
	bset	PORTA,#4
	bclr	PORTA,#4
	mov		[W6],[W7++]
	bset	PORTA,#4
	bclr	PORTA,#4
	mov		[W6],[W7++]
	bset	PORTA,#4
	bclr	PORTA,#4
	mov		[W6],[W7++]
	bset	PORTA,#4
	
	
	
	; >>> 29 INSTRUCTIONS
	; Subtract the old data and loop buffer index
	; old data is stored in location bufferPtr
	
	; W0 is new data
	; W1 is base pointer
	; W2 is N
	; W3 is write index
	; W13 is write pointer
	; W8 is (new data)-(old data)
	; W7 is address of kValue
	; W12 is read index
	; W10 is read pointer
	
	; Load kValue pointer
	mov		#kValue,W7
	add		W1,W3,W13			; generate current write address
	add		W1,W12,W10			; generate current read address
	
	; channel 0
	mov		localBuf+0,W0		; get new data
	sub		W0,[W10],W8			; subtract old data from new data
	add		W8,[W7],[W7++]		; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer write address by N to next channel buffer
	add		W10,W2,W10			; increment buffer read address to next channel
	
	; channel 1
	mov		localBuf+2,W0		; get new data
	sub		W0,[W10],W8			; subtract old data from new data
	add		W8,[++W7],[W7++]	; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer address by N to next channel buffer
	add		W10,W2,W10			; increment buffer read address to next channel
	
	; channel 2
	mov		localBuf+4,W0		; get new data
	sub		W0,[W10],W8			; subtract old data from new data
	add		W8,[++W7],[W7++]	; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer address by N to next channel buffer
	add		W10,W2,W10			; increment buffer read address to next channel
	
	; channel 3
	mov		localBuf+6,W0		; get new data
	sub		W0,[W10],W8			; subtract old data from new data
	add		W8,[++W7],[W7]		; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	
	; Perform looping
	inc2	W3,W3				; increment buffer write index
	and		W3,W9,W3			; clear high bits
	inc2	W12,W12				; increment buffer read index
	and		W12,W9,W12			; clear high bits
	
	
	; >>> 24 INSTRUCTIONS
	; Multiply new k values by wFactor
	; Both numebers are complex
	; The formula we need is a*c-b*d, a*d+b*c
	; We are lucky because one operand is at a constant address, and the others are in contiguous addresses.
	
	; set operand addresses
	mov		#kValue,W8			; real part address of kValues
	mov		W8,W13				; write-back address of kValue
	
	; prefetch channel 0 operands
	mov.d	[W8++],W6			; real and imag part of kValue(0) (2 cycles)
	
	; channel 0 multiply
	mpy.n	W5*W7,A							; real = -b*d
	mpy		W5*W6,B							; imag = b*c
	mac		W4*W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(1)) to W6
	mac		W4*W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(1)) to W6, write back real part to kValue
	clr		A,[W13]+=2						; write back imag part to kValue
	
	; channel 1 multiply
	mpy.n	W5*W7,A							; real = -b*d
	mpy		W5*W6,B							; imag = b*c
	mac		W4*W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(2)) to W6
	mac		W4*W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(2)) to W6, write back real part to kValue
	clr		A,[W13]+=2						; write back imag part to kValue
	
	; channel 2 multiply
	mpy.n	W5*W7,A							; real = -b*d
	mpy		W5*W6,B							; imag = b*c
	mac		W4*W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(3)) to W6
	mac		W4*W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(3)) to W6, write back real part to kValue
	clr		A,[W13]+=2						; write back imag part to kValue
	
	; channel 3 multiply
	mpy.n	W5*W7,A							; real = -b*d
	mpy		W5*W6,B							; imag = b*c
	mac		W4*W6,A							; real += a*c
	mac		W4*W7,B,[W13]+=2				; imag += a*d, write back real part to kValue
	clr		A,[W13]+=2						; write back imag part to kValue
	
	

	; Add second batch of samples
	; >>> 29 INSTRUCTIONS
	; Subtract the old data and loop buffer index
	; old data is stored in location bufferPtr
	
	; W0 is new data
	; W1 is base pointer
	; W2 is N
	; W3 is write index
	; W13 is write pointer
	; W8 is (new data)-(old data)
	; W7 is address of kValue
	; W12 is read index
	; W10 is read pointer
	
	; Load kValue pointer
	mov		#kValue,W7
	add		W1,W3,W13			; generate current write address
	add		W1,W12,W10			; generate current read address
	
	; channel 0
	mov		localBuf+8,W0		; get new data
	sub		W0,[W10],W8			; subtract old data from new data
	add		W8,[W7],[W7++]		; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer write address by N to next channel buffer
	add		W10,W2,W10			; increment buffer read address to next channel
	
	; channel 1
	mov		localBuf+10,W0		; get new data
	sub		W0,[W10],W8			; subtract old data from new data
	add		W8,[++W7],[W7++]	; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer address by N to next channel buffer
	add		W10,W2,W10			; increment buffer read address to next channel
	
	; channel 2
	mov		localBuf+12,W0		; get new data
	sub		W0,[W10],W8			; subtract old data from new data
	add		W8,[++W7],[W7++]	; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer address by N to next channel buffer
	add		W10,W2,W10			; increment buffer read address to next channel
	
	; channel 3
	mov		localBuf+14,W0		; get new data
	sub		W0,[W10],W8			; subtract old data from new data
	add		W8,[++W7],[W7]		; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	
	; Perform looping
	inc2	W3,W3				; increment buffer write index
	and		W3,W9,W3			; clear high bits
	inc2	W12,W12				; increment buffer read index
	and		W12,W9,W12			; clear high bits
	
		
	
	; >>> 24 INSTRUCTIONS
	; Multiply new k values by wFactor
	; Both numebers are complex
	; The formula we need is a*c-b*d, a*d+b*c
	; We are lucky because one operand is at a constant address, and the others are in contiguous addresses.
	
	; set operand addresses
	mov		#kValue,W8			; real part address of kValues
	mov		W8,W13				; write-back address of kValue
	
	; prefetch channel 0 operands
	mov.d	[W8++],W6			; real and imag part of kValue(0) (2 cycles)
	
	; channel 0 multiply
	mpy.n	W5*W7,A							; real = -b*d
	mpy		W5*W6,B							; imag = b*c
	mac		W4*W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(1)) to W6
	mac		W4*W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(1)) to W6, write back real part to kValue
	clr		A,[W13]+=2						; write back imag part to kValue
	
	; channel 1 multiply
	mpy.n	W5*W7,A							; real = -b*d
	mpy		W5*W6,B							; imag = b*c
	mac		W4*W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(2)) to W6
	mac		W4*W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(2)) to W6, write back real part to kValue
	clr		A,[W13]+=2						; write back imag part to kValue
	
	; channel 2 multiply
	mpy.n	W5*W7,A							; real = -b*d
	mpy		W5*W6,B							; imag = b*c
	mac		W4*W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(3)) to W6
	mac		W4*W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(3)) to W6, write back real part to kValue
	clr		A,[W13]+=2						; write back imag part to kValue
	
	; channel 3 multiply
	mpy.n	W5*W7,A							; real = -b*d
	mpy		W5*W6,B							; imag = b*c
	mac		W4*W6,A							; real += a*c
	mac		W4*W7,B,[W13]+=2				; imag += a*d, write back real part to kValue
	clr		A,[W13]+=2						; write back imag part to kValue
	
	
	
	; >>> 15 INSTRUCTIONS
	; Now norm the vectors
	
	; load addresses
	mov		#kValues,W8			; address of kValue array
	
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
	clr		A,[W13]+=2						; write back to kMags
	


	; >>> 33-37 INSTRUCTIONS
	; Compare the norms to the trigger level
	mov		#kMags,W8			; address of kMags in W8
	mov		#TMR8,W6
	mov		#TMR9HLD,W7
	mov		#timestamp,W13
	mov		trigger,W0

	mov		[W8++],W10			; get mag and increment pointer
	btsc	trigFlag,#0			; check if this channel has triggered already
	cpslt	W0,W10				; compare trigger to kMag for channel 0
	bra		_apdCh0Skip			; returns at _apdChXDone label
	mov		[W6],[W13++]		; get timestamp low word
	mov		[W7],[W13++]		; get timestamp high word
	bclr	trigFlag,#0			; decrement trigger counter
_apdCh0Done:

	mov		[W8++],W10			; get mag and increment pointer
	btsc	trigFlag,#1			; check if this channel has triggered already
	cpslt	W0,W10				; compare trigger to kMag for channel 0
	bra		_apdCh1Skip			; returns at _apdChXDone label
	mov		[W6],[W13++]		; get timestamp low word
	mov		[W7],[W13++]		; get timestamp high word
	bclr	trigFlag,#1			; decrement trigger counter
_apdCh1Done:

	mov		[W8++],W10			; get mag and increment pointer
	btsc	trigFlag,#2			; check if this channel has triggered already
	cpslt	W0,W10				; compare trigger to kMag for channel 0
	bra		_apdCh2Skip			; returns at _apdChXDone label
	mov		[W6],[W13++]		; get timestamp low word
	mov		[W7],[W13++]		; get timestamp high word
	bclr	trigFlag,#2			; decrement trigger counter
_apdCh2Done:

	mov		[W8++],W10			; get mag and increment pointer
	btsc	trigFlag,#3			; check if this channel has triggered already
	cpslt	W0,W10				; compare trigger to kMag for channel 0
	bra		_apdCh3Skip			; returns at _apdChXDone label
	mov		[W6],[W13++]		; get timestamp low word
	mov		[W7],[W13++]		; get timestamp high word
	bclr	trigFlag,#3			; decrement trigger counter
_apdCh3Done:

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

_apdCh0Skip:
	add		W13,#4,W13
	bra		_apdCh0Done
	
_apdCh1Skip:
	add		W13,#4,W13
	bra		_apdCh1Done

_apdCh2Skip:
	add		W13,#4,W13
	bra		_apdCh2Done

_apdCh3Skip:
	add		W13,#4,W13
	bra		_apdCh3Done
	


.end

