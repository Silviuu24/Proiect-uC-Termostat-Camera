del EQU 99h

ORG 0000h
	JMP MAIN
	
ORG 0050h
	text_temp: DB "Temperature:",0	
	
ORG 0100h	
	
MAIN:
	MOV P1, #0FFh
	MOV P2, #00h
	CLR P0.1
	SETB P3.2
	
	ACALL SETUP_LCD
	
	MOV P2, #55h
	SETB P0.0
	
	LOOP:
		MOV A, #8Ch
		ACALL COMMAND
		ACALL READ_ADC
		ACALL SHOW_TEMP
		
		MOV A,#2Ch
		MOV B, #01h
		ACALL DELAY
	JMP LOOP



DELAY:
	PUSH 07
	PUSH ACC
	ORL A, B
	JNZ OK
	
	POP ACC
	POP 07
	RET
	OK:
		POP ACC
	DEC_LSB:
		ACALL ONEMIL
		DJNZ ACC, DEC_LSB
	
		CJNE A, B, DEC_MSB
		POP 07
		RET
	DEC_MSB:
		DEC B
	
	SJMP DEC_LSB
	

ONEMIL:
	MOV R7, #del
	TIMER:
		NOP
		NOP
		NOP
		NOP
		DJNZ R7, TIMER
		NOP
RET

SETUP_LCD:

	MOV A, #38h
	ACALL COMMAND
	
	MOV A, #0Eh
	ACALL COMMAND
	
	MOV A, #01h
	ACALL COMMAND
	
	MOV A, #06h
	ACALL COMMAND
	
	MOV DPTR, #text_temp
	SHOW_TEXT:
		CLR A
		MOVC A, @A+DPTR    
		JZ END_PRINT        
		ACALL SHOW_CHAR       
		INC DPTR          
		JMP SHOW_TEXT
	END_PRINT:
RET

COMMAND:
	CLR P0.0
	MOV P2, A
	SETB P0.2
	ACALL ONEMIL
	ACALL ONEMIL
	CLR P0.2
RET

SHOW_CHAR:
	SETB P0.0
	MOV P2, A
	SETB P0.2
	MOV A,#0Ah
	MOV B, #00h
	ACALL DELAY
	CLR P0.2
RET

READ_ADC:
	CLR P3.6
	NOP
	NOP
	SETB P3.6

	WAIT: JB P3.2, WAIT
	CLR P3.7
	NOP
	NOP
	MOV A, P1
	SETB P3.7
RET

SHOW_TEMP:
	MOV B, #50h
	MUL AB
	
	MOV R0, B
	MOV R1, A
	
	MOV R2, #00h
	MOV R3, #00h
	
	DIV_51:
		CLR C
		
		MOV A, R1
		SUBB A, #33h
		MOV R1, A
		
		MOV A, R0
		SUBB A, #00h
		MOV R0, A
		
		JC DIV_DONE
		INC R3
		MOV A, R3
		JNZ DIV_51
		INC R2
		JMP DIV_51
	DIV_DONE:
		MOV A, R2
		MOV R0, A
		MOV A, R3
		MOV R1, A
		
		MOV R4, #00h
		MOV R5, #00h
		MOV R6, #00h
		
	LOOP_TENS:
		CLR C
		
		MOV A, R3
		SUBB A, #64h
		MOV R3, A
		
		MOV A, R2
		SUBB A, #00h
		MOV R2, A
		
		JC LOOP_TENS_DONE
		INC R4
		JMP LOOP_TENS
	LOOP_TENS_DONE:
		MOV A, R3
		ADD A, #64h
		MOV R3, A
	
	ONES_AND_TENTH:
		MOV A, R3
		MOV B, #0Ah
		DIV AB
		MOV R5, A
		MOV R6, B
	PRINT_TEMP:
		MOV A, R4
		JZ SPACE
		ADD A, #30h
		JMP PRINT
		SPACE:
			MOV A, #20h
		PRINT:
		ACALL SHOW_CHAR
		
		MOV A, R5
		ADD A, #30h
		ACALL SHOW_CHAR
		
		MOV A, #2Eh
		ACALL SHOW_CHAR
		
		MOV A, R6
		ADD A, #30h
		ACALL SHOW_CHAR	
RET

END