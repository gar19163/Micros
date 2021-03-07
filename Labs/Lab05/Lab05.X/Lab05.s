;Archivo:	lab05.S
;Dispositivo:	PIC16f887
;Autor:		Alejandro Garcia Aguirre
;Compilador:	pic-as (v2.30), MPLABX V5.45

;Programa:	contadores con Leds y 7seg
;Hardware:	Push buttons en para contador  en RB0-1 
;		Leds de contador binaro en RA0-7
;		7seg conectador en RC0-7
;		Transistores de RD0-5
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
  CONFIG  FOSC = INTRC_NOCLKOUT ; Oscillator Selection bits (RC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
      CONFIG  WDTE = OFF            ; Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
      CONFIG  PWRTE = ON            ; Power-up Timer Enable bit (PWRT enabled)
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
reset_tmr0 macro		;Convertimos en reset timer0 en un macro
    banksel PORTA		
    movlw   180	 
    movwf   TMR0
    bcf	    T0IF 
    endm

;-------------------------------------------------------
;   Variables
;-------------------------------------------------------

PSECT udata_bank0
  unidad:		DS  1	;
  decena:		DS  1	;
  centena:		DS  1	;
  var:			DS  1	;
  bandera:		DS  1	;
  nibble:		DS  2	;
  display_var:		DS  5	;
  dividir:		DS  1	;
    
PSECT udata_shr  
  W_TEMP:		    DS	1    ;1 byte
  temporal_status:	    DS	1    ;1 byte

PSECT resVect, class=CODE, abs, delta=2
    
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
;   Vector de interrupción
;-------------------------------------------------------
    
ORG 04h		;Posicion para las interrupciones
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

;-------------------------------------------------------
;   subrutina de interrupción
;-------------------------------------------------------
int_oc:
    banksel PORTA
    btfss   PORTB, 0	    
    incf    PORTA 
    btfss   PORTB, 7	    
    decf    PORTA	    
    bcf	    RBIF
    return
    
int_T:
    reset_tmr0			
    clrf    PORTD		
    btfss   bandera, 0	
    goto    display_0	
    
    btfss   bandera, 1
    goto    display_1
    
    btfss   bandera, 2
    goto    display_2
    
    btfss   bandera, 3
    goto    display_3
    
    btfss   bandera, 4
    goto    display_4
   
    
display_0:
    clrf    bandera				 	
    bsf	    bandera, 0			
    movf    display_var+0, w	 
    movwf   PORTC				 
    bsf	    PORTD, 1			
    return
   
display_1:
   
    bsf	    bandera, 1		 
    movf    display_var+1, w	 
    movwf   PORTC		 
    bsf	    PORTD, 0		
    return
    
display_2:
    
    bsf	    bandera, 2		 
    movf    display_var+2, w	 
    movwf   PORTC		 
    bsf	    PORTD, 2		
    return
    
display_3:
     
    bsf	    bandera, 3		 
    movf    display_var+3, w	 
    movwf   PORTC		 
    bsf	    PORTD, 3		
    return

display_4:
  
    bsf	    bandera, 4		 
    movf    display_var+4, w	 
    movwf   PORTC		 
    bsf	    PORTD, 4
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
    call	config_io    
    call	config_pull
   
    
    banksel	PORTB     
    clrf	PORTA
    clrf	PORTC
    clrf	PORTD
  
    call         config_clock   
    call         config_int_ena
    call         onfig_int_oc
    call         config_tmr0
    banksel	 PORTA
   
;-------------------------------------------------------
;   Loop principal
;-------------------------------------------------------
loop:
    movf    PORTA, w	     
    movwf   var	    
    call    separar_nibbles	     
    call    preparar_display	    
    
    movf    PORTA, w	     
	
    call    division_centena
    movf    centena, w	    
    call    division_decena	    
    movf    decena, w	    
    call    division_unidad	
    movf    unidad, w	    
    
    goto    loop

;-------------------------------------------------------
;   Subrutinas
;-------------------------------------------------------
config_io:
    banksel ANSEL	;Selecciono el banco donde esta ANSEL
    clrf    ANSEL	;Entrada digital
    clrf    ANSELH	;Entrada analógica
   
    banksel TRISA	
    movlw   10000001B ;Declaramos 2 bits del puerto B como entradas
    movwf   TRISB
    movlw   11100000B ;Declaramos el puerto D como 5 bits de salida
    movwf   TRISD
    clrf    TRISC     ;Encendemos salida en A y C
    clrf    TRISA
    
config_pull:
    bcf	    OPTION_REG, 7   ;pull-ups internos
    bsf	    WPUB, 0
    bsf	    WPUB, 7
	return

separar_nibbles:
    movf    var, w	
    andlw   0x0f	
    movwf   nibble	
    swapf   var, w	
    andlw   0x0f	
    movwf   nibble+1	
    return
    
    return

preparar_display:
    movf    nibble, w	    
    call    tabla	    
    movwf   display_var	    ; 0
    
    movf    nibble+1, w	     
    call    tabla	    
    movwf   display_var+1   ; 1
    
    movf    centena, w	    
    call    tabla	    
    movwf   display_var+2   ; 2
    
    movf    decena, w	     
    call    tabla	    
    movwf   display_var+3   ; 3
    
    movf    unidad, w	     
    call    tabla	    
    movwf   display_var+4   ; 4
    
    return
    
config_clock:
    banksel OSCCON	    ;Oscilador
    bsf	    IRCF2	    ;Trabaja 4MHz (110)
    bsf	    IRCF1	    ;
    bcf	    IRCF0	    ; 
    bsf	    SCS		    ;Reloj interno 
    return
    
config_int_ena:
    bsf	    GIE
    bsf	    T0IE    
    bcf	    T0IF    
    bsf	    RBIE    ;habilito bandera
    bcf	    RBIF    ;cambio bandera
    return
    
onfig_int_oc:
    banksel TRISB
    bsf	    IOCB, 0
    bsf	    IOCB, 7
    banksel PORTA
    movf    PORTB, W
    bcf	    RBIF
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
	
division_decena:
    clrf    decena
    movlw   10
    subwf   dividir, f
    btfsc   CARRY
    incf    decena
    btfsc   CARRY
    goto    $-5
    movlw   10
    addwf   dividir, f
    return

division_centena:
    clrf    centena
    movlw   100
    subwf   dividir, f
    btfsc   CARRY
    incf    centena
    btfsc   CARRY
    goto    $-5
    movlw   100
    addwf   dividir, f
    return	
    
END