

Stack_Size      EQU     0x00000200			; 1KB stack

                AREA    STACK, NOINIT, READWRITE, ALIGN=3

				EXPORT	__initial_sp

Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000000		;	1KB heap

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB

; Vector Table Mapped to Address 0 at Reset

				AREA    VECT, DATA, READONLY
;				ENTRY
                IMPORT  Reset_Handler
				IMPORT	TMR_ISR
				IMPORT	FLASH_ISR
				EXPORT  __Vectors

__Vectors       DCD     __initial_sp              	; 0x00,Top of Stack
                DCD     Reset_Handler             	; 0x04,Reset Handler
				DCD		NMI_Handler					; 0x08
				DCD		HardFault_Handler			; 0x0C
				DCD		0							; 0x10~0x28 reserved
				DCD		0
				DCD		0
				DCD		0
				DCD		0
				DCD		0
				DCD		0
				DCD		SVCall_Handler				; 0x2C
				DCD		0							; 0x30~0x34 reserved
				DCD		0
				DCD		PendSV_Handler				; 0x38
				DCD		SysTick_Handler				; 0x3C
				DCD		FIQ_Handler					; 0x40		7816
				DCD		TMR_ISR						; 0x44	    timer
				DCD		FLASH_ISR					; 0x48	    flash
								
                AREA    |.text|, CODE, READONLY

NMI_Handler
	BX	LR

HardFault_Handler
	BX	LR

SVCall_Handler
	BX	LR

PendSV_Handler
	BX	LR

SysTick_Handler
	BX	LR

FIQ_Handler
	BX	LR
       
        END

