;Archivo:	lab03.S
;Dispositivo:	PIC16f887
;Autor:		Alejandro Garcia Aguirre
;Compilador:	pic-as (v2.30), MPLABX V5.45

;Programa:	contadores con Leds y 7seg
;Hardware:	Push buttons en para binario en RB0-1 
;		7seg de contador hexadecimal RC0-8
;		7seg de contador hexadecimal RD0-8
;		
;Creado: 23 feb, 2021
;Última modificación:  feb 27, 2021

;-------------------------------------------------------
;   Librerias incluidas
;-------------------------------------------------------
PROCESSOR 16f887
#include <xc.inc>
    
;-------------------------------------------------------
;   Palabras de configuración
;-------------------------------------------------------

; CONFIG1
  CONFIG  FOSC = INTRC_NOCLKOUT ; Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
  CONFIG  WDTE = OFF             ; Watchdog Timer Enable bit (WDT enabled)
  CONFIG  PWRTE = OFF           ; Power-up Timer Enable bit (PWRT disabled)
  CONFIG  MCLRE = OFF            ; RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
  CONFIG  CP = OFF              ; Code Protection bit (Program memory code protection is disabled)
  CONFIG  CPD = OFF             ; Data Code Protection bit (Data memory code protection is disabled)
  CONFIG  BOREN = OFF            ; Brown Out Reset Selection bits (BOR enabled)
  CONFIG  IESO = OFF             ; Internal External Switchover bit (Internal/External Switchover mode is enabled)
  CONFIG  FCMEN = OFF            ; Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
  CONFIG  LVP = ON              ; Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

; CONFIG2
  CONFIG  BOR4V = BOR40V        ; Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
  CONFIG  WRT = OFF             ; Flash Program Memory Self Write Enable bits (Write protection off)
 
;-------------------------------------------------------
;   Macro
;-------------------------------------------------------

  
;-------------------------------------------------------
;   Variables
;------------------------------------------------------- 
PSECT udata_bank0
    cont: DS 1	;1 byte contador
    
PSECT resVect, class = CODE, abs, delta = 2 

;-------------------------------------------------------
;   Vector de reset
;-------------------------------------------------------  

ORG 00h
resectVec:
    PAGESEL main
    goto main
    
PSECT code delta = 2, abs   ; 2 bytes instrucción
ORG 100h   

;-------------------------------------------------------
;   Tabla codificada para display 
;-------------------------------------------------------

tabla: 
    clrf    PCLATH
    bsf	    PCLATH, 0	;PCLATH = 01
    andwf   0x0f	;4 bites menos s.   
    addwf   PCL, F	;PCL + PCLATH + W
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
    retlw   01110111B	;A  
    retlw   01111100B	;B  
    retlw   00111001B	;C  
    retlw   01011110B	;D  
    retlw   01111001B	;E  
    retlw   01110001B	;F
;-------------------------------------------------------
;   Conf PIC
;-------------------------------------------------------
main:
    call    config_io    
    call    config_clock    ;
    call    config_tmr0	    ;
    banksel PORTA	;Me asegure que empiece en cero
    
    clrf    PORTB	;Limpia puetos B y D
    clrf    PORTD
    movwf   00111111B	;Tomo el 0 de la tabla como valor inicial del puerto C
    movwf   PORTC
    movwf   0x0
    movwf   cont
;-------------------------------------------------------
;   Loop principal
;-------------------------------------------------------
loop:			    
;---------------------Parte 1 --------------------------
    call    cont_1
;---------------------Parte 2 --------------------------
/*    btfsc   PORTA,  0	    ;Boton de incremento
    call    suma	    ;F suma
    btfsc   PORTA,  1	    ;Boton de resta
    call    resta	    ;F Resta
    call    reinicio_cont   ;F reinicio_cont
 ;--------------------Parte 3 --------------------------
    call    alerta	    ;F Alerta
 */
 
    goto    loop	    ;Ciclo
 
 
;-------------------------------------------------------
;   Subrutinas
;-------------------------------------------------------
config_io:
    banksel ANSEL	;Selecciono el banco donde esta ANSEL
    clrf    ANSEL	;Entrada digital
    clrf    ANSELH	;Entrada analógica
    
    banksel TRISA
    bsf     TRISA, 1	;Confi. pines del puerto A como entrada
    bsf	    TRISB, 2
    movlw   11110000B	;Salida pines del puerto B Leds
    movwf   TRISC	
    clrf    TRISC	;Salida pines del puerto C 7seg
    
config_clock:
    banksel OSCCON	    ;Oscilador
    bcf	    IRCF2	    ;Trabaja 250kHz (010)
    bsf	    IRCF1	    ;
    bcf	    IRCF0	    ; 
    bsf	    SCS		    ;Reloj interno 
    return
    
config_tmr0:
    banksel TRISA	    ;
    bcf	    T0CS	    ;
    bcf	    PSA		    ;Preescalares
    bsf	    PS2		    ;111
    bsf	    PS1		    ;
    bsf	    PS0		    ; 
    call    reinicio_tmr0   ;
    return
    
reinicio_tmr0:
    banksel PORTA	    ;
    movlw   50		    ;
    movwf   TMR0	    ;
    bcf	    T0IF	    ;
    return

cont_1:
    btfss   T0IF	    ;Contador en puerto B
    goto    $-1		    ;
    call    reinicio_tmr0   ;Reinicio contador B
    incf    PORTB
    return
    
/*     
suma:
    btfsc   PORTA, 0
    goto    $-1
    incf    cont, F
    movf    cont, W
    call    tabla
    movwf   PORTC
    return

resta:
    btfsc   PORTA, 1
    goto    $-1
    decf    cont, F
    movf    cont, W
    call    tabla
    movwf   PORTC
    return

alerta:
    movf    cont, W
    subwf   PORTB, W
    btfsc   STATUS, 2
    bsf	    PORTD, 0
    call    reinicio_tmr0
    bcf	    PORTD, 0
    return
    
reinicio_cont:
    btfsc   T0IF
    goto    $-1
    call    reinicio_tmr0
    movwf   00111111B
    andwf   PORTB
    return
*/
    END