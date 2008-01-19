
	; DSP Sliding DFT function
	
	; int asmSlidingDFT( unsigned int N, unsigned int M, unsigned int P, 
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
	
	.global _asmSlidingDFT
	
	
.text

.bss nLen,2
.bss bufAdr,2
.bss kValue,16		; must be in near memory
.bss kMags,8		; must be contiguous after kValue
.bss wFactor,4
.bss trigger,2
.bss mLen,2
.bss pLen,2
.bss timestampAdr,2
.bss timeout,4
.bss timestamp,16
.bss retIndexAdr,2
.bss bufTimeAdr,2
.bss trigFlag,2




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
	mov		[W14-14],W0
	mov		W0,timeout
	mov		[W14-12],W0
	mov		W0,timeout+2
	
	
	; Persistent WREG assignments:
	; W0 is scratch register ^
	; W1 is trigger value *
	; W2 is not used
	; W3 is not used
	; W4 is wFactor real, dsp *
	; W5 is wFactor imag, dsp *
	; W6 is scratch register, dsp ^
	; W7 is scratch register, dsp ^
	; W8 is kValue scratch address, dsp ^
	; W9 is buffer pointer max *
	; W10 is write pointer in data buffer *
	; W11 is read pointer in data buffer *
	; W12 is base address *
	; W13 is dsp ^
	; W14 is frame pointer &
	; W15 is stack pointer &
	
	
	
	mov		bufAdr,W12		; buffer base address
	
	mov		trigger,W1		; trigger value
	
	mov		wFactor,W4		; wFactor real part
	mov		wFactor+2,W5	; wFactor imag part
	
	; Generate buffer pointer maximum register. When this is reached, the pointer resets to base address
	sl		nLen,WREG		; buffer length, byte addressed
	sl		W0,#2,W6		; 4*N = total buffer length
	add		W12,W6,W9		; base addr + total length = pointer max
	
	; Prepare buffer write pointer. Always start at index 0
	mov		W12,W10			; buffer write index starts at beginning of buffer
	
	; Prepare window read pointer. Always start lagging behind write pointer by M
	sl		mLen,WREG		; window length, byte addressed
	sl		W0,#2,W7		; 4*M = total window length
	sub		W9,W7,W11		; pointer max - window length = read pointer lagging behind write pointer by one window
	
	; Prepare counter. This gets decremented to zero when the time comes.
	asr		pLen			; divide post-trigger sample count by 2 to get number of loops
	
	; Initialize trigger flags
	mov		#0xF,W0
	mov		W0,trigFlag		; trigger flags, all set to watch for trigger

	; Prepare timeout value
	; timeout will contain 2's complement of number of loop iterations before timing out
	neg		timeout			; negate number of loops to get incrementing counter start value
	neg		timeout+2

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
	
	mov		W12,W0			; get address of data buffer
	dec		W6,W7			; decrement total buffer length to get REPEAT argument
	repeat	W7				; clear entire array
	clr		[W0++]
	



BigLoop:
	
	; Wait for data to arrive
	; >> 5 INSTRUCTIONS MAX
	btsc	PORTA,#5
	bra		BigLoop
	
	; TOTAL INSTRUCTIONS EXECUTED AFTER THIS POINT:
	; MAX: 5 + 29 + 22 + 29 + 22 + 14 + 30 + 9 = 160
	; MIN: 2 + 29 + 22 + 29 + 22 + 14 + 22 + 7 = 147
	
	
	
	; >> 29 INSTRUCTIONS
	; Collect new data, add new data and subtract old data from kValue(r), and store new data in buffer	
	mov		#kValue,W8		; initialize kValue pointer

	bclr	PORTA,#4		; clock data out of ADC
	mov		PORTC,W0		; get data
	bset	PORTA,#4		; clock inactive
	mov		W0,[W10++]		; store data at write pointer, overwrite old data and increment pointer
	sub		W0,[W11++],W0	; subtract old window data from new window data, increment pointer
	add		W0,[W8],[W8++]	; add difference to kValue(r), double increment finishes next cycle
	
	bclr	PORTA,#4		; clock data out of ADC
	mov		PORTC,W0		; get data
	bset	PORTA,#4		; clock inactive
	mov		W0,[W10++]		; store data at write pointer, overwrite old data and increment pointer
	sub		W0,[W11++],W0	; subtract old window data from new window data, increment pointer
	add		W0,[++W8],[W8++]	; add difference to kValue(r), double increment finishes next cycle
	
	bclr	PORTA,#4		; clock data out of ADC
	mov		PORTC,W0		; get data
	bset	PORTA,#4		; clock inactive
	mov		W0,[W10++]		; store data at write pointer, overwrite old data and increment pointer
	sub		W0,[W11++],W0	; subtract old window data from new window data, increment pointer
	add		W0,[++W8],[W8++]	; add difference to kValue(r), double increment finishes next cycle
	
	bclr	PORTA,#4		; clock data out of ADC
	mov		PORTC,W0		; get data
	bset	PORTA,#4		; clock inactive
	mov		W0,[W10++]		; store data at write pointer, overwrite old data and increment pointer
	sub		W0,[W11++],W0	; subtract old window data from new window data, increment pointer
	add		W0,[++W8],[W8]	; add difference to kValue(r), double increment finishes next cycle
	
	; loop pointers
	cpsne	W9,W10
	mov		W12,W10
	cpsne	W9,W11
	mov		W12,W11
	
	
	; >>> 22 INSTRUCTIONS
	; Multiply new k values by wFactor
	; Both numebers are complex
	; The formula we need is a*c-b*d, a*d+b*c
	; We are lucky because one operand is at a constant address, and the others are in contiguous addresses.
	
	; set operand addresses
	mov		#kValue,W8						; load address of kValues for prefetches
	
	; prefetch channel 0 operands
	movsac	A,[W8]+=2,W6					; prefetch kValue(0r)
	mov		#kValue,W13						; load address of kValue for write-backs
	
	; channel 0 multiply
	mpy		W5*W6,B,[W8]+=2,W7				; imag = b*c, prefetch kValue(0i)
	mpy.n	W5*W7,A							; real = -b*d
	mac		W4*W6,A							; real += a*c, prefetch real(kValue(1)) to W6
	mac		W4*W7,B,[W13]+=2				; imag += a*d, prefetch imag(kValue(1)) to W6, write back real part to kValue
	movsac	A,[W8]+=2,W6,[W13]+=2			; write back imag part to kValue
	
	; channel 1 multiply
	mpy		W5*W6,B,[W8]+=2,W7				; imag = b*c, prefetch kValue(0i)
	mpy.n	W5*W7,A							; real = -b*d
	mac		W4*W6,A							; real += a*c, prefetch real(kValue(1)) to W6
	mac		W4*W7,B,[W13]+=2				; imag += a*d, prefetch imag(kValue(1)) to W6, write back real part to kValue
	movsac	A,[W8]+=2,W6,[W13]+=2			; write back imag part to kValue
	
	; channel 2 multiply
	mpy		W5*W6,B,[W8]+=2,W7				; imag = b*c, prefetch kValue(0i)
	mpy.n	W5*W7,A							; real = -b*d
	mac		W4*W6,A							; real += a*c, prefetch real(kValue(1)) to W6
	mac		W4*W7,B,[W13]+=2				; imag += a*d, prefetch imag(kValue(1)) to W6, write back real part to kValue
	movsac	A,[W8]+=2,W6,[W13]+=2			; write back imag part to kValue
	
	; channel 3 multiply
	mpy		W5*W6,B,[W8]+=2,W7				; imag = b*c, prefetch kValue(0i)
	mpy.n	W5*W7,A							; real = -b*d
	mac		W4*W6,A							; real += a*c, prefetch real(kValue(1)) to W6
	mac		W4*W7,B,[W13]+=2				; imag += a*d, prefetch imag(kValue(1)) to W6, write back real part to kValue
											; write back imag part in next movsac instruction
	
	; >> 29 INSTRUCTIONS
	; Collect new data, add new data and subtract old data from kValue(r), and store new data in buffer	
	mov		#kValue,W8		; initialize kValue pointer

	bclr	PORTA,#4		; clock data out of ADC
	mov		PORTC,W0		; get data
	bset	PORTA,#4		; clock inactive
	mov		W0,[W10++]		; store data at write pointer, overwrite old data and increment pointer
	sub		W0,[W11++],W0	; subtract old window data from new window data, increment pointer
	add		W0,[W8],[W8++]	; add difference to kValue(r), double increment finishes next cycle
	
	bclr	PORTA,#4		; clock data out of ADC
	mov		PORTC,W0		; get data
	bset	PORTA,#4		; clock inactive
	mov		W0,[W10++]		; store data at write pointer, overwrite old data and increment pointer
	sub		W0,[W11++],W0	; subtract old window data from new window data, increment pointer
	add		W0,[++W8],[W8++]	; add difference to kValue(r), double increment finishes next cycle
	
	bclr	PORTA,#4		; clock data out of ADC
	mov		PORTC,W0		; get data
	bset	PORTA,#4		; clock inactive
	mov		W0,[W10++]		; store data at write pointer, overwrite old data and increment pointer
	sub		W0,[W11++],W0	; subtract old window data from new window data, increment pointer
	add		W0,[++W8],[W8++]	; add difference to kValue(r), double increment finishes next cycle
	
	bclr	PORTA,#4		; clock data out of ADC
	mov		PORTC,W0		; get data
	bset	PORTA,#4		; clock inactive
	mov		W0,[W10++]		; store data at write pointer, overwrite old data and increment pointer
	sub		W0,[W11++],W0	; subtract old window data from new window data, increment pointer
	add		W0,[++W8],[W8]	; add difference to kValue(r), double increment finishes next cycle
	
	; loop pointers
	cpsne	W9,W10
	mov		W12,W10
	cpsne	W9,W11
	mov		W12,W11
	
	
	; >>> 22 INSTRUCTIONS
	; Multiply new k values by wFactor
	; Both numebers are complex
	; The formula we need is a*c-b*d, a*d+b*c
	; We are lucky because one operand is at a constant address, and the others are in contiguous addresses.
	
	; set operand addresses
	mov		#kValue,W8						; load real part address of kValues
	
	; prefetch channel 0 operands
	movsac	A,[W8]+=2,W6,[W13]+=2			; prefetch kValue(0r), writeback imag part from last calculation
	mov		#kValue,W13						; load write-back address of kValue
	
	; channel 0 multiply
	mpy		W5*W6,B,[W8]+=2,W7				; imag = b*c, prefetch kValue(0i)
	mpy.n	W5*W7,A							; real = -b*d
	mac		W4*W6,A							; real += a*c, prefetch real(kValue(1)) to W6
	mac		W4*W7,B,[W13]+=2				; imag += a*d, prefetch imag(kValue(1)) to W6, write back real part to kValue
	movsac	A,[W8]+=2,W6,[W13]+=2			; write back imag part to kValue
	
	; channel 1 multiply
	mpy		W5*W6,B,[W8]+=2,W7				; imag = b*c, prefetch kValue(0i)
	mpy.n	W5*W7,A							; real = -b*d
	mac		W4*W6,A							; real += a*c, prefetch real(kValue(1)) to W6
	mac		W4*W7,B,[W13]+=2				; imag += a*d, prefetch imag(kValue(1)) to W6, write back real part to kValue
	movsac	A,[W8]+=2,W6,[W13]+=2			; write back imag part to kValue
	
	; channel 2 multiply
	mpy		W5*W6,B,[W8]+=2,W7				; imag = b*c, prefetch kValue(0i)
	mpy.n	W5*W7,A							; real = -b*d
	mac		W4*W6,A							; real += a*c, prefetch real(kValue(1)) to W6
	mac		W4*W7,B,[W13]+=2				; imag += a*d, prefetch imag(kValue(1)) to W6, write back real part to kValue
	movsac	A,[W8]+=2,W6,[W13]+=2			; write back imag part to kValue
	
	; channel 3 multiply
	mpy		W5*W6,B,[W8]+=2,W7				; imag = b*c, prefetch kValue(0i)
	mpy.n	W5*W7,A							; real = -b*d
	mac		W4*W6,A							; real += a*c, prefetch real(kValue(1)) to W6
	mac		W4*W7,B,[W13]+=2				; imag += a*d, prefetch imag(kValue(1)) to W6, write back real part to kValue


											
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
	


	; >>> 22-30 INSTRUCTIONS
	; Compare the norms to the trigger level AND end-of-loop overhead
	mov		TMR8,W6				; get timer value at this iteration
	mov		TMR9HLD,W7
	
	mov		kMags+0,W8			; get mag and increment pointer
	btsc	trigFlag,#0				; check if this channel has triggered already
	cpslt	W1,W8				; compare trigger to kMag for channel 0
	bra		_apdCh0Done			; returns at _apdChXDone label
	mov		W6,timestamp+0		; get timestamp low word
	mov		W7,timestamp+2		; get timestamp high word
	bclr	trigFlag,#0				; decrement trigger counter
_apdCh0Done:

	mov		kMags+2,W8			; get mag and increment pointer
	btsc	trigFlag,#1				; check if this channel has triggered already
	cpslt	W1,W8				; compare trigger to kMag for channel 0
	bra		_apdCh1Done			; returns at _apdChXDone label
	mov		W6,timestamp+4		; get timestamp low word
	mov		W7,timestamp+6		; get timestamp high word
	bclr	trigFlag,#1				; decrement trigger counter
_apdCh1Done:

	mov		kMags+4,W8			; get mag and increment pointer
	btsc	trigFlag,#2				; check if this channel has triggered already
	cpslt	W1,W8				; compare trigger to kMag for channel 0
	bra		_apdCh2Done			; returns at _apdChXDone label
	mov		W6,timestamp+8		; get timestamp low word
	mov		W7,timestamp+10		; get timestamp high word
	bclr	trigFlag,#2				; decrement trigger counter
_apdCh2Done:

	mov		kMags+6,W8			; get mag and increment pointer
	btsc	trigFlag,#3				; check if this channel has triggered already
	cpslt	W1,W8				; compare trigger to kMag for channel 0
	bra		_apdCh3Done			; returns at _apdChXDone label
	mov		W6,timestamp+12		; get timestamp low word
	mov		W7,timestamp+14		; get timestamp high word
	bclr	trigFlag,#3				; decrement trigger counter
_apdCh3Done:
	
	
	; >> LOOP OVERHEAD: 7-9 INSTRUCTIONS
	; increment timeout
	mov		#0,W0				; load 0 into W0
	inc		timeout				; adds 1 to timeoutL, initializes zero flag to state of result
	addc	timeout+2			; adds carry to timeoutH, if high byte is not zero, clears zero flag
	bra		Z,_apdTimeout		; both bytes are zero after incrementing, timeout occurred
	
	; Check if we have all four triggers, once that happens we go to post-trigger sampling
	cp0		trigFlag					; check if trigger flags are zero (W0 is already 0x0000)
	bra		NZ,BigLoop
	dec		pLen				; if trigFlag==0, decrement post-trigger sample count
	bra		NZ,BigLoop
	
	
	; OUT OF BIG LOOP!!! GOT ALL FOUR TRIGGERS!! OR TIMEOUT...
	
	
_apdFinished:
_apdTimeout:

	
	; Generate return values
	
	; Save timestamp corresponding to last point in buffer to location specified in argument
	mov		bufTimeAdr,W0
	mov.d	W6,[W0]
		
	; Return value contains error flags
	mov		trigFlag,W0			; if one or more triggers were not found, those bits are high
	cp0		pLen				; check post-trigger samples
	btsc	STATUS,#Z			; if not all post-trigger samples were taken, bit 4 goes high
	bset	W0,#4

	; Copy out timestamp values for return data
	mov		#timestamp,W0
	mov		timestampAdr,W8
	repeat	#7					; four long moves
	mov		[W0],[W8]
	
	; Copy out write index to show where buffer starts in time
	mov		retIndexAdr,W0
	sub		W10,W12,W8			; subtract buffer base address
	lsr		W8,#2,W8			; divide by 4 to give array index
	mov		W8,[W0]				; store at output location
	
	
	
	; Restore config registers
	pop		CORCON
	; Restore upper W registers
	pop.d	W12
	pop.d	W10
	pop.d	W8
	
	ulnk
	
	return



.end

