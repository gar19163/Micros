/*
 * Archivo:	lab07.c
 * Dispositivo:	PIC16f887
 * Autor:		Alejandro Garcia Aguirre
 * Compilador:	XC8 (v2.31), MPLABX V5.45
 * 
 * Programa:	contadores con push button
 * Hardware:	Push button PB0-1
 *              7seg PC0-7
 *              Transistores de PD0-2
 * 
Created on 13 de abril de 2021, 11:14 AM
Última modificación:  18 de abril de 2021
 */


//-------------------------------------------------------
//   Palabras de configuración
//-------------------------------------------------------

// PIC16F887 Configuration Bit Settings
// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

//-------------------------------------------------------
//   Librerias incluidas
//-------------------------------------------------------
#include <xc.h>
#include <stdint.h>
#include <stdio.h>

//-------------------------------------------------------
//   interrupciones
//-------------------------------------------------------
void setup(void);

void __interrupt() isr(void){
}

//-------------------------------------------------------
//   Main
//-------------------------------------------------------
void main(void){
    
    setup();       //Se llaman a las configuaraciones del PIC
    

    while (1) {     //Loop
        PORTD++;
    }
}
    
//-------------------------------------------------------
//   Conf PIC
//-------------------------------------------------------
void setup(void){ //Configuraciones del PIC
    
    TRISD   = 0x00;


    
    //Configuracion del oscilador
    OSCCONbits.IRCF2    = 1;     //4MHZ
    OSCCONbits.IRCF1    = 1;
    OSCCONbits.IRCF0    = 0;
    OSCCONbits.SCS      = 1;     //Oscilador interno    
}