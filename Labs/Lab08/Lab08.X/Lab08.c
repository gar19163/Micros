/*
 * Archivo:	lab08.c
 * Dispositivo:	PIC16f887
 * Autor:		Alejandro Garcia Aguirre
 * Compilador:	XC8 (v2.31), MPLABX V5.45
 * 
 * Programa:	Lector de ADC
 * Hardware:	Puerto A potenciometros 0/1
 *              7seg PC0-7
 *              Transistores de PD0-2
 * 
 * Created on 19 de abril de 2021, 05:50 PM
 * Última modificación:  23 de abril de 2021
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
#define _XTAL_FREQ 4000000  //Delay

//-------------------------------------------------------
//  Variables
//-------------------------------------------------------
int     dec;                //Variable de la decena
int     cen;                //Variable de la centena
int     uni;                //Variable de la unidad
int     dig;                //Variable del digito completo del contador
int     segm;               //Display 7segmentos

//-------------------------------------------------------
//  Tabla
//-------------------------------------------------------
const char tabla[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66,  //0,1,2,3,4 
                        0x6D, 0x7D, 0x07, 0x7F, 0x6F}; //5,6,7,8,9

//-------------------------------------------------------
//   interrupciones
//-------------------------------------------------------
void setup(void);

void __interrupt() isr(void){
    if (T0IF==1){
        INTCONbits.T0IF = 0;        //Bajar bandera TMR0
        TMR0 = 247;                 //Reset TMR0
        PORTB = 0x00;               //Limpio puerto
        if (segm==0){               //Centena
            PORTC = tabla[cen];     //Despliego el valor de centena
            PORTB = 0x01;           //Enciendo pin 0
            segm = 1;               //Cambio a decena
        }
        else if (segm==1){          //Decena
            PORTC = tabla[dec];     //Despliego el valor de decena
            PORTB = 0x02;           //Enciendo pin 1
            segm = 2;               //Cambio a unidad
        }
        else if (segm==2){          //Unidad     
            PORTC = tabla[uni];     //Despliego el valor de decena
            PORTB = 0x04;           //Enciendo pin 2
            segm = 0;               //Cambio a decena
        }
    }
    
	if(PIR1bits.ADIF == 1){         //Interrupción ADC
        if(ADCON0bits.CHS == 5)
            PORTD = ADRESH;         //Muestra valor en LEDs    
        else
            dig = ADRESH;           //Valor a digito
        PIR1bits.ADIF = 0;          //Baja bandera 
    }
}
    
//-------------------------------------------------------
//   Main
//-------------------------------------------------------
void main(void){
    setup();                            //Llamamos a nuestras configuraciones
    
    while (1) {                         //Loop
        cen = dig/100;                  //El digito lo divido en 100;
        dec = (dig - (100  * cen))/10;  //Resto centena y divido en 10
        uni = dig - (100* cen) - (10*dec); //Resto dec y cen    
        
        if(ADCON0bits.GO == 0){         //Cambio de canal
            if(ADCON0bits.CHS == 5)     //Canal 5
                ADCON0bits.CHS = 6;     //CAmbio a canal 6
            
            else                        //Canal 6
            ADCON0bits.CHS = 5;         //Cambio a canal 5
            
            __delay_us(50);             //Delay
            ADCON0bits.GO = 1;          //GO
        }
   }
}

//-------------------------------------------------------
//   Conf PIC
//-------------------------------------------------------
void setup(void){ //Configuraciones del PIC
    
    //Entradas digitales y analogicas
    ANSEL  = 0b01100000;          //se utiliza canal 5 y 6
    ANSELH = 0x00;          
    
    //Entradas y Salidas
    TRISE   = 0x03;         //Entradas
    TRISB   = 0x00;         //salidas
    TRISC   = 0x00;
    TRISD   = 0x00;

    //Limpiar puertos 
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;

    //Configuracion del oscilador
    OSCCONbits.IRCF2    = 1;     //4MHZ
    OSCCONbits.IRCF1    = 1;
    OSCCONbits.IRCF0    = 0;
    OSCCONbits.SCS      = 1;     //Oscilador interno
    
    //TMR0
    OPTION_REGbits.T0CS = 0;     //Fosc/4
    OPTION_REGbits.PSA  = 0;     //Prescaler assigned TMR0
    OPTION_REGbits.PS2  = 1;     //1:128
    OPTION_REGbits.PS1  = 1;
    OPTION_REGbits.PS0  = 0;
    TMR0 = 247;                  //2ms
    
    //Configuracion del ADC
    ADCON1bits.ADFM     = 0;     //Justificación a la izquierda
    ADCON1bits.VCFG0    = 0;     //Voltaje de referencia VSS
    ADCON1bits.VCFG1    = 0;     //VDD
    ADCON0bits.ADCS     = 1;     //Fosc/8
    ADCON0bits.CHS      = 5;    //Canal 5
    __delay_us(100);
    ADCON0bits.ADON     = 1;    //Encender modulo
    
    //Configuraciones de interrupciones 
    INTCONbits.GIE  = 1;
    INTCONbits.T0IE = 1;
    INTCONbits.T0IF = 0;
    PIE1bits.ADIE   = 1;
    PIR1bits.ADIF   = 0;
    ADCON0bits.GO   = 1;
    INTCONbits.PEIE = 1;
}