;Archivo:	lab02.S
;Dispositivo:	PIC16f887
;Autor:		Alejandro Garcia Aguirre
;Compilador:	pic-as (v2.30), MPLABX V5.45

;Programa:	Contador de 4 bits
;Hardware:	Contador 1 en los puertos, con 2 push b en RA0-1 / LEDs en Puestos D 
;		Contador 2 en los puertos, con 2 push b en RA2-3 / LEDs en Puestos C
;		Sumador de contadores, con 1 push b en RA4 / LEDs en Puestos B 

;Creado: 9 feb, 2021
;Última modificación:  feb 13, 2021

;-------------------------------------------------------
;   Librerias incluidas
;-------------------------------------------------------
PROCESSOR 16F887
#include <xc.inc>
    
;-------------------------------------------------------
;   Palabras de configuración
;-------------------------------------------------------

; Palabra de configuración 1
    
  CONFIG  FOSC = XT		; Oscillator Selection bits (RC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
  CONFIG  WDTE = OFF            ; Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
  CONFIG  PWRTE = ON            ; Power-up Timer Enable bit (PWRT enabled)
  CONFIG  MCLRE = OFF           ; RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
  CONFIG  CP = OFF              ; Code Protection bit (Program memory code protection is disabled)
  CONFIG  CPD = OFF             ; Data Code Protection bit (Data memory code protection is disabled)
  CONFIG  BOREN = OFF           ; Brown Out Reset Selection bits (BOR disabled)
  CONFIG  IESO = OFF            ; Internal External Switchover bit (Internal/External Switchover mode is disabled)
  CONFIG  FCMEN = OFF           ; Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
  CONFIG  LVP = ON              ; Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

; Palabra de configuración 2
  CONFIG  BOR4V = BOR40V        ; Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
  CONFIG  WRT = OFF             ; Flash Program Memory Self Write Enable bits (Write protection off)
 
  
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
ORG 100h 	; Posicion para el código
    
;-------------------------------------------------------
;   Conf PIC
;-------------------------------------------------------
main:
    banksel ANSEL	;Selecciono el banco donde esta ANSEL
    clrf    ANSEL	;Entrada digital
    clrf    ANSELH	;Entrada analógica
    
    banksel TRISA	
    movlw   11111111B	;ENcendemos entradas en
    movwf   TRISA
    
 
    clrf   TRISB
    
    movlw   11110000B	;Declaramos el puerto C para ser de 4 bits de salida
    movwf   TRISC
    
    movlw   11110000B	;Declaramos el puerto D para ser de 4 bits de salida
    movwf   TRISD
    
    
    banksel PORTA       ;Se limpian los puertos
    clrf    PORTB
    clrf    PORTC
    clrf    PORTD
    
;-------------------------------------------------------
;   Loop principal
;-------------------------------------------------------
loop:			    
    btfsc	PORTA, 0    ;si deja de ser presionado llama
    call	inc_C	    ;incrementa en contador 1
    
    btfsc	PORTA, 1    ;si deja de ser presionado llamar 
    call	dec_C	    ;decrementa en contador 1
    
    btfsc	PORTA, 2    ;si deja de ser presionado llama
    call	inc_D	    ;incrementa en contador 2
    
    btfsc	PORTA, 3    ;si deja de ser presionado llama
    call	inc_D	    ;decrementa en contador 2
    
    btfsc	PORTA, 4    ;si deja de ser presionado llama
    call	sumar	    ;llama para sumar ambos contadores 
    
    goto	loop	    
 
 
;-------------------------------------------------------
;   Subrutinas
;-------------------------------------------------------
inc_C:			    ;Incrementa en contador 1 puerto C
    btfsc   PORTA, 0	    ;Antirebote
    goto    $-1		    ;si no regresa una linea para revisar
    incf    PORTC, F	    ;Incrementar 1 enel valor en el puerto C
    return
 
dec_C:			    ;decrementa en contador 1 puerto C
    btfsc   PORTA, 1	    ;revisa que se deje de presionar
    goto    $-1		    ;si no regresa una linea para revisar
    decf    PORTC, F	    ;decrementa 1 en el valor en el puerto C
    return
       
inc_D:			    ;Incrementa en contador 2 puerto D
    btfsc   PORTA, 2	    ;revisa que se deje de presionar
    goto    $-1		    ;si no regresa una linea para revisar
    incf    PORTD, F	    ;Incrementar 1en el valor en el puerto 
    return
    
dec_D:			    ;decrementa en contador 2 puerto D
    btfsc   PORTA, 3	    ;revisa que se deje de presionar
    goto    $-1		    ;si no regresa una linea para revisar
    decf    PORTD, F	    ;decrementa 1 en el valor en el puerto D
    return
 
sumar:			    ;Suma ambos contadores en puerto A
    btfsc   PORTA, 4	    ;revisa que se deje de presionar
    goto    $-1		    ;si no regresa una linea para revisar
    movf    PORTC, 0	    ;Mover el valor del puerto C a f
    addwf   PORTD, 0	    ;Sumar w y f
    movwf   PORTB		    ;Mover el valor de la suma al puerto B
    return
    
END