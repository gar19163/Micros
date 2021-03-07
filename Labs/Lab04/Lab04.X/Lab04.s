;Archivo:	main.S
;Dispositivo:	PIC16f887
;Autor:		Alejandro Garcia Aguirre
;Compilador:	pic-as (v2.30), MPLABX V5.45

;Programa:	contadores con Leds y 7seg
;Hardware:	Push buttons en para contador de 7seg en RA0-1 
;		Leds de contador binaro en RB0-3
;		7seg de contador hexadecimal RC0-8
;		
;Creado: 16 feb, 2021
;Última modificación:  feb 20, 2021

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
  
  
;------------------------------------------------------------------------------
;  Macro
;------------------------------------------------------------------------------
    
;------------------------------------------------------------------------------
;  Variables
;------------------------------------------------------------------------------  
     
 PSECT udata_shr  
    cont:	   DS 2 ;2 byte   
    var:	   DS 1 ;1 byte  
    W_TEMP:	   DS 1 ;1 byte
    temporal_status:   DS 1 ;1

;-------------------------------------------------------
;   Vector de reset
;-------------------------------------------------------
 PSECT resVect, class=CODE, abs, delta = 2 
 ORG 00h    ; 
 resetVec:
    PAGESEL main
    goto main

;-------------------------------------------------------
;   Vector de interrupción 
;-------------------------------------------------------
 PSECT intVect, class=CODE, abs, delta=2
 
ORG 04h			    ;Posición 04
 push:
    movwf   W_TEMP	    ;Guardar valores en las status
    swapf   STATUS, W	    
    movwf   temporal_status
    
 isr:
    btfsc   RBIF	    ;Revisar bandera
    call    int_oc	    ;interrup
    btfsc   T0IF	    ;Revisar bandera 
    call    int_T	    ;interrup
  
 pop:
    swapf   temporal_status, W
    movwf   STATUS
    swapf   W_TEMP, F
    swapf   W_TEMP, W
    retfie
;------------------------------------------------------------------------------
;	sub rutinas de interrupcion
;------------------------------------------------------------------------------
 int_T:
  call	  reset_tmr0  ;50ms
  incf    cont
  movwf   cont, W
  sublw   20	    ;50ms * 10 = 500ms
  btfss   ZERO
  goto    $+3
  clrf    cont
  incf    var
  return
 
 int_oc:
    banksel PORTB
    btfss   PORTB, 0
    incf    PORTA
    btfss   PORTB, 1
    decf    PORTA
    bcf	    RBIF
    
    return
    
 PSECT code, delta=2, abs
 ORG 100h   ;posicion para el código
;-------------------------------------------------------
;   Tabla codificada para display 
;-------------------------------------------------------
    
tabla: 
    clrf    PCLATH
    bsf	    PCLATH, 0	;PCLATH = 01
    andwf   0x0f	;4 bites para trabajar    
    addwf   PCL		;PCL + PCLATH + W
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
    call    config_pull  
    call    config_7seg   
    call    config_clock    
    call    config_tmr0 
    call    config_int_oc
    call    config_int_ena
    banksel PORTA
    clrf    PORTA
    
;-------------------------------------------------------
;   Loop principal
;-------------------------------------------------------
 
 loop:
    movf    PORTA, W
    call    tabla	;traduce
    movwf   PORTC	;muestra valor en display
        
    
    movf    var, W	;
    call    tabla	;traduce
    movwf   PORTD	;muestra valor en display
   
  goto    loop
    
;-------------------------------------------------------
;   Subrutinas
;-------------------------------------------------------
reset_tmr0:
    banksel PORTA
    movlw   60
    movwf   TMR0
    bcf	    T0IF 
    return
    
config_io:
    banksel ANSEL	;Selecciono el banco donde esta ANSEL
    clrf    ANSEL	;Entrada digital
    clrf    ANSELH	;Entrada analógica
    
    banksel TRISB   ;Entradas puertos B
    bsf	    TRISB, 0	
    bsf	    TRISB, 1
    clrf    TRISA   ;Apago pin de salida puerto A
    bsf	    TRISA, 4
    bsf	    TRISA, 5
    bsf	    TRISA, 6
    bsf	    TRISA, 7
    
    clrf    TRISC   ;Pines de salida puerto C y D
    clrf    TRISD
    clrf    PORTC
    clrf    PORTD
    return  
    
 config_pull:
    bcf	    OPTION_REG, 7   ;pull-ups internos
    bsf	    WPUB, 0
    bsf	    WPUB, 1
 
 config_clock:
    banksel OSCCON	    ;Oscilador
    bsf	    IRCF2	    ;Trabaja 4MHz (110)
    bsf	    IRCF1	    ;
    bcf	    IRCF0	    ; 
    bsf	    SCS		    ;Reloj interno 
    return
    
 config_tmr0:
    banksel TRISA
    bcf	    T0CS    ;reloj interno
    bcf	    PSA	    ;preescalar(111)
    bsf	    PS2	    
    bsf	    PS1
    bsf	    PS0	    ;PS 256
    banksel PORTA
    call    reset_tmr0
    return
 
 config_int_oc:
    banksel TRISB
    bsf	    IOCB, 0
    bsf	    IOCB, 1
    banksel PORTA
    movf    PORTB, W
    bcf	    RBIF
    return
 
 config_int_ena:
    bsf	    GIE
    bsf	    T0IE    
    bcf	    T0IF    
    bsf	    RBIE    ;habilito bandera
    bcf	    RBIF    ;cambio bandera
    return
 
 config_7seg:
    banksel PORTA	
    movlw   11111100B	; Dejo el valor 0 de la tabla para iniciar
    movwf   PORTC	
    movwf   PORTD
    movlw   0x00
    movwf   cont
    movwf   var
    return
END