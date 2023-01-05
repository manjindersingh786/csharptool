
; --- Standard definitions of mode bits and interrupt (I & F) flags in PSRs
Mode_USR        EQU     0x10 
Mode_FIQ        EQU     0x11 
Mode_IRQ        EQU     0x12 
Mode_SVC        EQU     0x13 
Mode_ABT        EQU     0x17 
Mode_UND        EQU     0x1B 
Mode_SYS        EQU     0x1F				; available on ARM Arch 4 and later

I_Bit           EQU     0x80 				; when I bit is set, IRQ is disabled
F_Bit           EQU     0x40 				; when F bit is set, FIQ is disabled

; --- Disable MPU first
MPUDisable	    EQU     0xFFFFFFFE	    	; Disable MPU

; --- System memory locations
RAM_Start       EQU     0x20000000	        ; For THC8000
RAM_Limit       EQU     0x20002A00	        ; For THC80F17BD_V40
;SVC_Stack       EQU     RAM_Limit        	; followed by SVC stack
;TMR_IRQ_Stack	EQU     SVC_Stack-48
;FLASH_IRQ_Stack	EQU     SVC_Stack-40
;DES_IRQ_Stack	EQU     SVC_Stack-40
;FIQ_Stack       EQU     DES_IRQ_Stack-64   ; 64 byte IRQ stack,followed by FIQ stack
;ABT_Stack		EQU		FIQ_Stack-64		; 64 bytes of FIQ stack followed by Abort Stack
; add UNDEF_Stack here if you need them
;SYS_Stack       EQU     ABT_Stack-128       ; 128 byte ABT stack,followed by SYS stack

;Abort_Flag		EQU		ABT_Stack-64	    ; Abort flag address

Flash_Start		EQU		0x000000			; Base address of flash
Stack_Size      EQU     0x00000200			; 1KB stack
Heap_Size       EQU     0x00000000			; 1KB heap
                
                IMPORT	||ZISection$$Table$$Limit||	
                IMPORT	||Image$$RAM$$ZI$$Base||
                IMPORT	||Image$$RAM$$ZI$$Limit||

    END