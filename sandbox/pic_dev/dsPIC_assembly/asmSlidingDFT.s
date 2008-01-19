
	; DSP Sliding DFT function
	
	; int asmSlidingDFT( int N, int M, int P, fractional trigger, fractcomplex wFactor, fractional * pData, int * pIndex, long * pBufferTime, long * pTimestamps, long Timeout );
	; N - number of words (samples) in each channel buffer
	; M - number of words (samples) in SDFT window
	; P - number of samples to take after first/last trigger is acquired
	; trigger - DFT term magnitude that constitutes a trigger (term must be greater than this number)
	; wFactor - complex number representing the DFT term to look for.  Is found by exp(2*pi*i*k/M).
	; pData - address of start of sample data buffer of size 4*N words
	; pTimestamps - address of buffer in which to return timestamp values
	; Timeout - number of timestamp periods after which to exit without receiving triggers.
	
	
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
.bss trigFlag,2
.bss mLen,2
.bss pLen,2
.bss timestampAdr,2
.bss timeout,4
.bss timeoutVal,4
.bss timestamp,16
.bss retIndexAdr,2
.bss bufTimeAdr,2




; W register arguments:
; W0 - N (buffer length)
; W1 - M (window length)
; W2 - P (post-trigger samples)
; W3 - trigger
; W4 - re(W)
; W5 - im(W)
; W6 - bufAdr
; W7 - retIndexAdr
; W7 - timestampAdr


_asmSlidingDFT:

	lnk		#0		; set frame pointer so we can access stack arguments

	; Save upper W registers
	push.d	W8
	push.d	W10
	push.d	W12
	; Save config registers
	push	CORCON
	push	PSVPAG
	
	; Configure core for fractional operations
	fractsetup	W7
	
	
	
	; Store all the arguments in RAM before loading them back into registers
	mov		W0,nLen
	mov		W1,mLen
	mov		W2,pLen
	mov		W3,trigger
	mov		W4,wFactor
	mov		W5,wFactor+2
	mov		W6,bufAdr
	mov		W7,retIndexAdr
	mov		[W14-8],W0
	mov		W0,bufTimeAdr
	mov		[W14-10],W0
	mov		W0,timestampAdr
	mov.d	[W14-14],W0
	mov		W0,timeoutVal
	mov		W1,timeoutVal+2
	
	
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
	; W10 is not used
	; W11 is not used
	; W12 is buffer read index
	; W13 is scratch register, dsp
	; W14 is frame pointer
	; W15 is stack pointer
	
	
	
	mov		bufAdr,W1		; buffer base address
	
	mov		nLen,W2
	sl		W2,#1,W2		; buffer length, byte addressed
	
	clr		W3				; buffer write index
	
	; this is where they are already
	;mov		wFactor,W4		; wFactor real part
	;mov		wFactor+2,W5	; wFactor imag part
	
	dec		W2,W9			; buffer index looping mask
	
	sl		mLen,WREG
	mov		W0,mLen			; window length, byte addressed
	sub		W2,W0,W12		; calculate read index lagging behind write index my M (write=0,read=N-M)
	
	asr		pLen			; divide sample count by 2 to get number of loops
	
	mov		#0xF,W0
	mov		W0,trigFlag		; trigger flags, all set
	
	mov		TMR8,W0			; get current time LSB
	add		timeoutVal,WREG	; add LSB
	mov		W0,timeout		; store timeout value LSB
	mov		TMR9HLD,W0		; get current time MSB
	addc	timeoutVal+2,WREG	; add MSB with carry
	mov		W0,timeout+2	; store timeout value MSB
	
	
	; Clear timestamp registers
	mov		#timestamp,W0
	repeat	#7
	clr		[W0++]
	
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
	; >> 5 INSTRUCTIONS MAX
	btsc	PORTA,#5
	bra		BigLoop
	
	; TOTAL INSTRUCTIONS EXECUTED AFTER THIS POINT:
	; 5 + 26 + 29 + 23 + 29 + 22 + 14 + 36 + 5 = 189
	
	
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
	; W6 is read pointer
	
	; Load kValue pointer
	mov		#kValue,W7
	add		W1,W3,W13			; generate current write address
	add		W1,W12,W6			; generate current read address
	
	; channel 0
	mov		localBuf+0,W0		; get new data
	sub		W0,[W6],W8			; subtract old data from new data
	add		W8,[W7],[W7++]		; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer write address by N to next channel buffer
	add		W6,W2,W6			; increment buffer read address to next channel
	
	; channel 1
	mov		localBuf+2,W0		; get new data
	sub		W0,[W6],W8			; subtract old data from new data
	add		W8,[++W7],[W7++]	; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer address by N to next channel buffer
	add		W6,W2,W6			; increment buffer read address to next channel
	
	; channel 2
	mov		localBuf+4,W0		; get new data
	sub		W0,[W6],W8			; subtract old data from new data
	add		W8,[++W7],[W7++]	; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer address by N to next channel buffer
	add		W6,W2,W6			; increment buffer read address to next channel
	
	; channel 3
	mov		localBuf+6,W0		; get new data
	sub		W0,[W6],W8			; subtract old data from new data
	add		W8,[++W7],[W7]		; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	
	; Perform looping
	inc2	W3,W3				; increment buffer write index
	and		W3,W9,W3			; clear high bits
	inc2	W12,W12				; increment buffer read index
	and		W12,W9,W12			; clear high bits
	
	
	; >>> 23 INSTRUCTIONS
	; Multiply new k values by wFactor
	; Both numebers are complex
	; The formula we need is a*c-b*d, a*d+b*c
	; We are lucky because one operand is at a constant address, and the others are in contiguous addresses.
	
	; set operand addresses
	mov		#kValue,W8			; real part address of kValues
	mov		W8,W13				; write-back address of kValue
	
	; prefetch channel 0 operands
	mov		[W8++],W6						; prefetch kValue(0r)
	
	; channel 0 multiply
	mpy		W5*W6,B,[W8]+=2,W7				; imag = b*c, prefetch kValue(0i)
	mpy.n	W5*W7,A							; real = -b*d
	mac		W4*W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(1)) to W6
	mac		W4*W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(1)) to W6, write back real part to kValue
	movsac	A,[W13]+=2						; write back imag part to kValue
	
	; channel 1 multiply
	mpy.n	W5*W7,A							; real = -b*d
	mpy		W5*W6,B							; imag = b*c
	mac		W4*W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(2)) to W6
	mac		W4*W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(2)) to W6, write back real part to kValue
	movsac	A,[W13]+=2						; write back imag part to kValue
	
	; channel 2 multiply
	mpy.n	W5*W7,A							; real = -b*d
	mpy		W5*W6,B							; imag = b*c
	mac		W4*W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(3)) to W6
	mac		W4*W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(3)) to W6, write back real part to kValue
	movsac	A,[W13]+=2						; write back imag part to kValue
	
	; channel 3 multiply
	mpy.n	W5*W7,A							; real = -b*d
	mpy		W5*W6,B							; imag = b*c
	mac		W4*W6,A							; real += a*c
	mac		W4*W7,B,[W13]+=2				; imag += a*d, write back real part to kValue
	movsac	A,[W13]+=2						; write back imag part to kValue
	
	

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
	; W6 is read pointer
	
	; Load kValue pointer
	mov		#kValue,W7
	add		W1,W3,W13			; generate current write address
	add		W1,W12,W6			; generate current read address
	
	; channel 0
	mov		localBuf+8,W0		; get new data
	sub		W0,[W6],W8			; subtract old data from new data
	add		W8,[W7],[W7++]		; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer write address by N to next channel buffer
	add		W6,W2,W6			; increment buffer read address to next channel
	
	; channel 1
	mov		localBuf+10,W0		; get new data
	sub		W0,[W6],W8			; subtract old data from new data
	add		W8,[++W7],[W7++]	; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer address by N to next channel buffer
	add		W6,W2,W6			; increment buffer read address to next channel
	
	; channel 2
	mov		localBuf+12,W0		; get new data
	sub		W0,[W6],W8			; subtract old data from new data
	add		W8,[++W7],[W7++]	; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	add		W13,W2,W13			; increment buffer address by N to next channel buffer
	add		W6,W2,W6			; increment buffer read address to next channel
	
	; channel 3
	mov		localBuf+14,W0		; get new data
	sub		W0,[W6],W8			; subtract old data from new data
	add		W8,[++W7],[W7]		; add difference to kValue (pre and post increment ot skip imag part)
	mov		W0,[W13]			; store new data in buffer
	
	; Perform looping
	inc2	W3,W3				; increment buffer write index
	and		W3,W9,W3			; clear high bits
	inc2	W12,W12				; increment buffer read index
	and		W12,W9,W12			; clear high bits
	
		
	
	; >>> 22 INSTRUCTIONS
	; Multiply new k values by wFactor
	; AND NORM THE VECTORS
	; Both numebers are complex
	; The formula we need is a*c-b*d, a*d+b*c
	; We are lucky because one operand is at a constant address, and the others are in contiguous addresses.
	
	; set operand addresses
	mov		#kValue,W8			; real part address of kValues
	mov		W8,W13				; write-back address of kValue
	
	; prefetch channel 0 operands
	mov		[W8++],W6						; prefetch kValue(0r)
	
	; channel 0 multiply
	mpy		W5*W6,B,[W8]+=2,W7				; imag = b*c, prefetch kValue(0i)
	mpy.n	W5*W7,A							; real = -b*d
	mac		W4*W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(1)) to W6
	mac		W4*W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(1)) to W6, write back real part to kValue
	movsac	A,[W13]+=2						; write back imag part to kValue
	
	; channel 1 multiply
	mpy.n	W5*W7,A							; real = -b*d
	mpy		W5*W6,B							; imag = b*c
	mac		W4*W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(2)) to W6
	mac		W4*W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(2)) to W6, write back real part to kValue
	movsac	A,[W13]+=2						; write back imag part to kValue
	
	; channel 2 multiply
	mpy.n	W5*W7,A							; real = -b*d
	mpy		W5*W6,B							; imag = b*c
	mac		W4*W6,A,[W8]+=2,W6				; real += a*c, prefetch real(kValue(3)) to W6
	mac		W4*W7,B,[W8]+=2,W7,[W13]+=2		; imag += a*d, prefetch imag(kValue(3)) to W6, write back real part to kValue
	movsac	A,[W13]+=2						; write back imag part to kValue
	
	; channel 3 multiply
	mpy.n	W5*W7,A							; real = -b*d
	mpy		W5*W6,B							; imag = b*c
	mac		W4*W6,A							; real += a*c
	mac		W4*W7,B,[W13]+=2				; imag += a*d, write back real part to kValue
											; real writeback in Norming section	
											
	; >> 14 INSTRUCTIONS
	; Norm Vectors
	
	; load addresses
	mov		#kValues,W8						; address of kValue array in prefetch register
	; fetch first values
	movsac	A,[W8]+=2,W6,[W13]+=2			; prefetch kValue(0r) to W6 and writeback kValue(3) imag
	
	; channel 0 norm
	mpy		W6*W6,A,[W8]+=2,W7				; square real part, prefetch kValue(0i) to W7
	mac		W7*W7,A,[W8]+=2,W6				; square and sum imag part, prefetch kValue(1r) to W6
	movsac	B,[W13]+=2						; write back to kMags
	
	; channel 1 norm
	mpy		W6*W6,A,[W8]+=2,W6				; square real part, prefetch kValue(1i) to W6
	mac		W7*W7,A,[W8]+=2,W7				; square and sum imag part, prefetch next real part
	movsac	B,[W13]+=2						; write back to kMags
		
	; channel 2 norm
	mpy		W6*W6,A,[W8]+=2,W6				; square real part, prefetch next imag part
	mac		W7*W7,A,[W8]+=2,W7				; square and sum imag part, prefetch next real part
	movsac	B,[W13]+=2						; write back to kMags
	
	; channel 3 norm
	mpy		W6*W6,A,[W8],W7					; square real part, prefetch kValue(3i)
	mac		W7*W7,A							; square and sum imag part
	movsac	B,[W13]+=2						; write back to kMags
	


	; >>> 36 INSTRUCTIONS
	; Compare the norms to the trigger level
	mov		TMR8,W6				; get timer value at this iteration
	mov		TMR9HLD,W7
	
	; Load trigger value
	mov		trigger,W0

	mov		kMags+0,W8			; get mag and increment pointer
	btsc	trigFlag,#0			; check if this channel has triggered already
	cpslt	W0,W8				; compare trigger to kMag for channel 0
	bra		_apdCh0Done			; returns at _apdChXDone label
	mov		W6,timestamp+0		; get timestamp low word
	mov		W7,timestamp+2		; get timestamp high word
	bclr	trigFlag,#0			; decrement trigger counter
_apdCh0Done:

	mov		kMags+2,W8			; get mag and increment pointer
	btsc	trigFlag,#1			; check if this channel has triggered already
	cpslt	W0,W8				; compare trigger to kMag for channel 0
	bra		_apdCh1Done			; returns at _apdChXDone label
	mov		W6,timestamp+4		; get timestamp low word
	mov		W7,timestamp+6		; get timestamp high word
	bclr	trigFlag,#1			; decrement trigger counter
_apdCh1Done:

	mov		kMags+4,W8			; get mag and increment pointer
	btsc	trigFlag,#2			; check if this channel has triggered already
	cpslt	W0,W8				; compare trigger to kMag for channel 0
	bra		_apdCh2Done			; returns at _apdChXDone label
	mov		W6,timestamp+8		; get timestamp low word
	mov		W7,timestamp+10		; get timestamp high word
	bclr	trigFlag,#2			; decrement trigger counter
_apdCh2Done:

	mov		kMags+6,W8			; get mag and increment pointer
	btsc	trigFlag,#3			; check if this channel has triggered already
	cpslt	W0,W8				; compare trigger to kMag for channel 0
	bra		_apdCh3Done			; returns at _apdChXDone label
	mov		W6,timestamp+12		; get timestamp low word
	mov		W7,timestamp+14		; get timestamp high word
	bclr	trigFlag,#3			; decrement trigger counter
_apdCh3Done:

	; check for timeout
	mov		timeout,W0
	mov		timeout+2,W8
	sub		W0,W6,W0			; perform (timeoutL - TMRL)
	cpb		W8,W7				; perform (timeoutH - TMRH - B)
	bra		LT,_apdTimeout		; branch if timeout < TMR


	; >>> LOOP OVERHEAD: 5 INSTRUCTIONS MAX
	; Check if we have all four triggers
	cp0		trigFlag			; exit loop if all four triggers are there.
	bra		NZ,BigLoop
	dec		pLen				; decrement post-trigger sample count
	bra		NZ,BigLoop
	
	
	; OUT OF BIG LOOP!!! GOT ALL FOUR TRIGGERS!! OR TIMEOUT...
	
	
_apdFinished:
_apdTimeout:

	
	; Generate return value
	
	; if all four triggers were not found, return trigger flags
	mov		trigFlag,W0			; if one or more triggers were not found, those bits are high
	cp0		pLen				; check post-trigger samples
	bsw.z	W0,#4				; if not all post-trigger samples were taken, bit 4 goes high

	; Copy out timestamp values for return data
	mov		#timestamp,W6
	mov		timestampAdr,W7
	repeat	#3
	mov		[W6],[W7]
	
	; Copy out write index to show where buffer starts in time
	mov		retIndexAdr,W6
	mov		W3,[W6]
	
	; Save timestamp corresponding to last point in buffer
	mov		bufTimeAdr,W0
	mov.d	W6,[W0]
	

	; Restore config registers
	pop		PSVPAG
	pop		CORCON
	; Restore upper W registers
	pop.d	W12
	pop.d	W10
	pop.d	W8
	
	ulnk
	
	return



.end

