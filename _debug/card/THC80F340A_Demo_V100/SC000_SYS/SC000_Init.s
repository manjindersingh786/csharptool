

; On reset, the ARM core starts up in Supervisor (SVC) mode, in ARM state, with IRQ and FIQ disabled.

		PRESERVE8
        AREA    Init, CODE, READONLY
        GET	THC80ASM.H

;		ENTRY
        EXPORT  Reset_Handler
		IMPORT	__initial_sp

Reset_Handler


; --- Change clock to 30MHz
;	LDR   r0, = 0x40000010
;	LDR	  r1, = 0x00000080
;	STR   r1, [r0, #0]

; --- Initialise memory system,RW & ZI
	PUSH	{r0 - r4}						; For Cortex-M0
	LDR		r0, = ||ZISection$$Table$$Limit||
	LDR		r1, = RAM_Start					; RAM start
	LDR		r3, = ||Image$$RAM$$ZI$$Base||
	LDR		r4,	= 0

	LDR   r3, = ||Image$$RAM$$ZI$$Base||
	
	CMP		r1, r3							; There is no RW
    BEQ		%F1
    
0   CMP		r1, r3
	BEQ		%F1
	MRS		r2,	APSR						; For Cortex-M0,to save APSR in the stack
	PUSH	{r2}

	LDR		r2,	[r0,	r4]
	STR		r2,	[r1,	r4]
	ADDS	r0,	r0,	#4						; Source added with 4
	ADDS	r1,	r1,	#4						; Destination added with 4

	POP		{r2}
	MSR		APSR,	r2
	BNE		%B0

	LDR		r1,	= 0x01000200
	ANDS	r2,	r2,	r1						; Bit 24 and 9 of APSR reserved,other bits cleared
	MSR		APSR,	r2
    
1  	LDR		r1, = ||Image$$RAM$$ZI$$Limit||
	LDR		r2, = Stack_Size
	SUBS	r1,	r1, r2
	LDR		r2, = Heap_Size
	SUBS	r1,	r1, r2
	LDR		r2,	= 0
	LDR		r4,	= 0
    
2   CMP		r3, r1
	BEQ		JumpToMain
	MRS		r0,	APSR						; For Cortex-M0,to save APSR in the stack
	PUSH	{r0}
	STR		r2,	[r3,	r4]
	ADDS	r3,	r3,	#4						; Destination added with 4
	POP		{r0}
	MSR		APSR,	r0
	BNE		%B2
	
	POP		{r0 - r4}						; For Cortex-M0	

;	Branch to real main function BLmain() instead of __main provided by lib
	IMPORT	Testmain

; --- Now enter the C code
JumpToMain

;	Enable interrupt of sc000
	LDR		R0,	= 0xE000E100				; Interrupt Set-Enable register,ISER
	LDR		R1,	= 0xFFFFFFFF
	STR		R1,	[R0,	#0]					; All interrupt enabled

	LDR		R0, =0x00000000
	MSR		PRIMASK,  R0

;	B	Testmain							; note use B not BL, because an application will never return this way
	LDR     R0, =Testmain
	BX      R0

	END

