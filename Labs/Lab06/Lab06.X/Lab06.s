;Archivo:	lab06.S
;Dispositivo:	PIC16f887
;Autor:		Alejandro Garcia Aguirre
;Compilador:	pic-as (v2.30), MPLABX V5.45

;Programa:	contadores con 7seg
;Hardware:	Led TMR2 en RA0
;		7seg TMR1 conectador en RC0-7 
;		Transistores de RD0-1
;		
;		
;Creado: 2 mar, 2021
;Última modificación:  mar 6, 2021

;-------------------------------------------------------
;   Librerias incluidas
;-------------------------------------------------------
PROCESSOR 16F887
#include <xc.inc>

;-------------------------------------------------------
;   Palabras de configuración
;-------------------------------------------------------
; CONFIG1
  CONFIG  FOSC = INTRC_NOCLKOUT ; Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
  CONFIG  WDTE = OFF            ; Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
  CONFIG  PWRTE = OFF           ; Power-up Timer Enable bit (PWRT disabled)
  CONFIG  MCLRE = OFF           ; RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
  CONFIG  CP = OFF              ; Code Protection bit (Program memory code protection is disabled)
  CONFIG  CPD = OFF             ; Data Code Protection bit (Data memory code protection is disabled)
  CONFIG  BOREN = OFF           ; Brown Out Reset Selection bits (BOR disabled)
  CONFIG  IESO = OFF            ; Internal External Switchover bit (Internal/External Switchover mode is disabled)
  CONFIG  FCMEN = OFF           ; Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
  CONFIG  LVP = ON              ; Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

; CONFIG2
  CONFIG  BOR4V = BOR40V        ; Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
  CONFIG  WRT = OFF             ; Flash Program Memory Self Write Enable bits (Write protection off)
    
;-------------------------------------------------------
;   Macro
;-------------------------------------------------------
reset_tmr0 macro		
    banksel PORTA		
    movlw   247	 
    movwf   TMR0
    bcf	    T0IF	   ;Bandera TMR0
    endm
  
reset_tmr1 macro	
    banksel PORTA
    movlw   0xDC	    
    movwf   TMR1L	    
    movlw   0x0B
    movwf   TMR1H	    
    bcf	    PIR1, 0	   ;Bandera TMR1
  endm
  
reset_tmr2 macro
    banksel PORTA
    clrf    TMR2	   ;Se limpia el registro del TMR2
    bcf	    TMR2IF	   ;Bandera TMR2
    endm
  
;-------------------------------------------------------
;   Variables
;-------------------------------------------------------
 
PSECT udata_bank0  ;common memory	
  cont:			    DS 1 
  cont1:		    DS 1 
  var:			    DS 1   
  decena:		    DS 1
  unidad:		    DS 1
  dividir:		    DS 1
  banderas:		    DS 1	
  display_var:		    DS 2
 
PSECT udata_shr  
  W_TEMP:		    DS 1    
  temporal_status:	    DS 1    
  segm:			    DS 1    

;-------------------------------------------------------
;   Vector de reset
;-------------------------------------------------------  
PSECT resVect, class=CODE, abs, delta=2
ORG 00h			    ;Posición 0000h para el reset
resetVec:
    PAGESEL main
    goto main
    
PSECT code delta = 2, abs  
ORG 100h  

;-------------------------------------------------------
;   Vector de interrupción
;-------------------------------------------------------

ORG 04h		;Posicion para las interrupciones
push:
    movwf   W_TEMP	    ;Guardar valores en las status
    swapf   STATUS, W	    
    movwf   temporal_status
    
isr:
   btfsc   T0IF	    ;TMR0IF
   call    INT_TMR0
   
   btfsc   PIR1, 0  ;TMR1IF
   call    INT_TMR1
   
   btfsc   PIR1, 1  ;TMR2IF
   call    INT_TMR2

pop:
    swapf   temporal_status, W
    movwf   STATUS
    swapf   W_TEMP, F
    swapf   W_TEMP, W
    retfie

;-------------------------------------------------------
;   subrutina de interrupción
;-------------------------------------------------------

INT_TMR0:
    reset_tmr0  
    clrf    PORTD
    btfss   banderas, 0 
    goto    display_0
    btfss   banderas, 1
    goto    display_1
  

display_0:
    btfss   PORTA, 0
    bsf	    banderas, 0
    movf    display_var+0, W
    movwf   PORTC
    bsf	    PORTD, 1
    return
	
display_1:
    btfss   PORTA, 0
    bcf	    banderas, 0
    movf    display_var+1, W
    movwf   PORTC
    bsf	    PORTD, 0
    return

INT_TMR1: 
    reset_tmr1		    ;500ms
    incf    cont
    movwf   cont, W
    sublw   2		    ;500ms * 2 = 1s
    btfss   ZERO
    goto    rtrn_tmr1		;Regresar
    clrf    cont	
    incf    var	
    return
rtrn_tmr1:
    return			;Regresar
    
INT_TMR2:
    reset_tmr2
    clrf    TMR2    
    bcf	    PIR1, 1 
    incf    cont1
    movwf   cont1, W
    sublw   5		    ;25ms * 10 = 250ms
    btfss   ZERO
    goto    $+5	
    clrf    cont1	
    btfsc   PORTA, 0
    goto    next
    bsf	    PORTA, 0   
    return
    
next:
    bcf	    PORTA,0
    return
;-------------------------------------------------------
;   Tabla codificada para display 
;-------------------------------------------------------
    
PSECT code, delta=2, abs
ORG 100h		    ; Posicion para el código
tabla: 
    clrf    PCLATH
    bsf	    PCLATH, 0	;PCLATH = 01
    andwf   0x0f	;4 bites menos s.   
    addwf   PCL, f	;PCL + PCLATH + W
    retlw   00111111B	;0
    retlw   00000110B	;1  
    retlw   01011011B	;2  
    retlw   01001111B	;3  
    retlw   01100110B	;4  
    retlw   01101101B	;5  
    retlw   01111101B	;6  
    retlw   00000111B	;7  
    retlw   01111111B	;8  
    retlw   01101111B	;9
 
;-------------------------------------------------------
;   config PIC
;-------------------------------------------------------
 
main:
    banksel ANSEL	;Selecciono el banco donde esta ANSEL
    clrf    ANSEL	;Entrada digital
    clrf    ANSELH	;Entrada analógica
   
    banksel TRISA	;encender puertos
    clrf    TRISB	
    clrf    TRISA
    clrf    TRISC
    clrf    TRISD
    banksel PORTA
    
       
    banksel PORTA	;Me asegure que empiece en cero
    clrf    PORTA
    clrf    PORTB
    clrf    PORTC
    clrf    PORTD
    clrf    banderas
    clrf    var
    
    call    config_clock    
    call    config_int_ena
    call    config_tmr0 
    call    config_tmr1 
    call    config_tmr2
    banksel PORTA
    
;-------------------------------------------------------
;   Loop principal
;-------------------------------------------------------
 
loop:
    movf    var, W
    movwf   dividir
    call    division_decena
    call    preparar_display1
    
    goto loop
;-------------------------------------------------------
;   Subrutinas
;-------------------------------------------------------
config_clock:
    banksel OSCCON	    ;Oscilador
    bsf	    IRCF2	    ;Trabaja 4MHz (110)
    bsf	    IRCF1	    ;
    bcf	    IRCF0	    ; 
    bsf	    SCS		    ;Reloj interno 
    return
    
config_tmr0:
    banksel TRISA
    bcf	    T0CS	    ;reloj interno
    bcf	    PSA		    ;preescalar(111)
    bsf	    PS2	    
    bsf	    PS1
    bsf	    PS0		    ;PS 256
    reset_tmr0		    ;Llamamos el reset del timer0
    return
 
config_int_ena:
    bsf	    GIE
    bsf	    T0IE
    bcf	    T0IF
    bsf	    TMR1IE 
    bsf	    TMR2IE 
    bcf	    TMR1IF
    bcf	    TMR2IF 
    return
    
config_tmr1:
    banksel T1CON
    bsf	    T1CON, 5	    ;preescaler 1:8
    bsf	    T1CON, 4	    
    bcf	    T1CON, 1	    ;TEMPORIZADOR
    bsf	    T1CON, 0	    ;tmr 1 ON
    reset_tmr1
    return
	
config_tmr2:
    BANKSEL PORTA
    movlw   11111111B	;preescalar 16, postscaler 1:16
    movwf   T2CON
    BANKSEL TRISA
    movlw   98		    ;0.025s
    movwf   PR2
    BANKSEL PORTA
    clrf    TMR2
    bcf	    PIR1, 1
    reset_tmr2
    return

division_decena:
    clrf    decena
    movlw   10
    subwf   dividir, F
    btfsc   CARRY	
    incf    decena	
    btfsc   CARRY
    goto    $-5 
    movlw   10
    addwf   dividir, F
    movlw   10
    subwf   decena, W
    btfsc   ZERO    
    clrf    decena
    return

division_unidad:
    clrf    unidad
    movlw   1
    subwf   dividir, f
    btfsc   CARRY
    incf    unidad
    btfsc   CARRY
    goto    $-5
    movlw   1
    addwf   dividir, f
    return
    
preparar_display1:
    movf    decena, W	    
    call    tabla
    movwf   display_var+1
    movf    dividir, W
    call    tabla
    movwf   display_var
    return
END