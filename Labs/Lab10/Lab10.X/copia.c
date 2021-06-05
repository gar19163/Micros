/*
 * Archivo:     lab10.c
 * Dispositivo:	PIC16f887
 * Autor:		Alejandro Garcia Aguirre
 * Compilador:	XC8 (v2.31), MPLABX V5.45
 * 
 * Programa:	
 * Hardware:	Puerto A potenciometros 0/1
 *              CCP1 con servo en RC2
 *              CCP2 con servo en RC1
 *
 * Created on 2 de mayo de 2021, 09:38 PM
 * Última modificación:  09 de mayo de 2021
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
//  Constantes
//-------------------------------------------------------
#define _XTAL_FREQ 8000000  //Delay

//-------------------------------------------------------
//  Variables
//-------------------------------------------------------
int caracter;
int op   = 0;
int ban  = 1;
//-------------------------------------------------------
//  arreglos
//-------------------------------------------------------

//-------------------------------------------------------
//   Prototipo
//------------------------------------------------------
void setup(void);
void send_char (char dato);
void send_str  (char st[]);

//-------------------------------------------------------
//   Interrupciones
//------------------------------------------------------
void __interrupt()isr(void){
    if(PIR1bits.RCIF){                  //Interrupción de recepción
        op = RCREG;                     //Recepcion a variable

    }
}

//-------------------------------------------------------
//   Main
//-------------------------------------------------------
void main(void){
    setup();                            //Llamamos a nuestras configuraciones
    
    PORTA   =0x00;                      //Limpio puertos 
    PORTB   =0x00;
    
    while (1) {                         //Loop
        __delay_ms(500);
        if (ban==1){                    //Desplegar menu
            send_str("Que accion desas ejecutar?\r");
            send_str("Lado izquierdo\r");
            send_str("(1) Mover pie izquierdo a 0\r");
            send_str("(2) Mover pie izquierdo a 90 \r");
            send_str("(3) Mover pierna izquierdo a 0 \r");
            send_str("(4) Mover pierna izquierdo a 90 \r \r");
            send_str("Lado Derecho\r");
            send_str("(5) Mover pie derecho a 0\r");
            send_str("(6) Mover pie derecho a 90\r");
            send_str("(7) Mover pierna derecho a 0\r");
            send_str("(8) Mover pierna derecho a 90\r\r");
            send_str("Desplegar datos\r");
            send_str("(9) Leer los datos");
            ban = 0;                     //Bajo bendera de menu
            op  = 0;                     //Coloco en 0 opciones                  //Coloco en 0 opciones
        }
        
        switch(op){                      //Opciones
            case ('1'):
                __delay_ms(3000);       //Delay para limpiar
                TXREG = '\f';           //Limpio
                ban = 1;                //Muestro menu
                break;
            
            case ('2'):
   //             send_str(pa);
                while(PIR1bits.RCIF == 0);//Reviso bandera
                __delay_ms(1000);         //Delay para limpiar
                TXREG = '\f';             //Limpio terminal
                PORTA = 0x00;             //Limpio puerto
                ban = 1;                  //Muestro menu
                break;
                
            case ('3'):
 //               send_str(pa);
                while(PIR1bits.RCIF == 0);//Reviso bandera
                __delay_ms(1000);         //Delay para limpiar
                TXREG = '\f';             //Limpio
                PORTB = 0x00;             //Limpio puerto
                ban = 1;                  //Muestro menu
                break;
        }
    }
}

//-------------------------------------------------------
//   Conf PIC
//-------------------------------------------------------
void setup(void){               //Configuraciones del PIC
    //Entradas digitales y analogicas
    ANSEL   = 0x00;       
    ANSELH  = 0x00;          
    
    //Entradas y Salidas
    TRISA   = 0x00;             //salidas
    TRISB   = 0x00;
    
    //Configuracion del oscilador
    OSCCONbits.IRCF2    = 1;    //8MHZ
    OSCCONbits.IRCF1    = 1;
    OSCCONbits.IRCF0    = 1;
    OSCCONbits.SCS      = 1;    //Oscilador interno
    
    //Config TX y RX
    TXSTAbits.SYNC      = 0;    //Asincrono
    TXSTAbits.BRGH      = 0;    //High Speed 
    BAUDCTLbits.BRG16   = 1;    //usamos 16 bits
    SPBRG              = 51;    //Baud rate
    SPBRGH              = 0;    //9600
    RCSTAbits.SPEN      = 1;    //Enciendo modulo 
    RCSTAbits.RX9       = 0;    //8bits de recepción
    RCSTAbits.CREN      = 1;    //Recepción  
    TXSTAbits.TXEN      = 1;    //TRansmisión   
    
    //Configuraciones de interrupciones 
    INTCONbits.GIE      = 1;
    INTCONbits.PEIE     = 1;
    PIR1bits.RCIF       = 0; 
    
    PIE1bits.RCIE       = 1;
}
//-------------------------------------------------------
//   Subrutinas
//-------------------------------------------------------
void send_char (char dato){
    while(!TXIF);           //Transmisión
    TXREG = dato;       //Muestro caracter
}

void send_str(char st[]){
    int i = 0;              
    while (st[i] != 0){
        send_char(st[i]);
        i++;               
    }
}
