/*
 * Archivo:     lab09.c
 * Dispositivo:	PIC16f887
 * Autor:		Alejandro Garcia Aguirre
 * Compilador:	XC8 (v2.31), MPLABX V5.45
 * 
 * Programa:	Lector de ADC a PWM
 * Hardware:	Puerto A potenciometros 0/1
 *              CCP1 con servo en RC2
 *              CCP2 con servo en RC1
 *
 * Created on 27 de abril de 2021, 12:56 PM
 * Última modificación:  02 de mayo de 2021
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

//-------------------------------------------------------
//  Tabla
//-------------------------------------------------------

//-------------------------------------------------------
//   Prototipo
//------------------------------------------------------
void setup(void);

//-------------------------------------------------------
//   Interrupciones
//------------------------------------------------------
void __interrupt()isr(void){
    
	if(PIR1bits.ADIF == 1){         //Interrupción ADC
        
        PIR1bits.ADIF = 0;          //Bajo bandera
        
        if(ADCON0bits.CHS == 5){    //PWM 1 
            CCPR1L = (ADRESH >> 1) + 124;   
            CCP1CONbits.DC1B1 = ADRESH & 0b01;
            CCP1CONbits.DC1B0 = (ADRESL>>7);         
        }
        else{                               //PWM 2
            CCPR2L = (ADRESH >> 1) + 124;   //Se rota 1 bit de 0 a 127
            CCP2CONbits.DC2B1 = ADRESH & 0b01;//LSB
            CCP2CONbits.DC2B0 = (ADRESL>>7);  //MSB analogico 
        }        
    }
}  
//-------------------------------------------------------
//   Main
//-------------------------------------------------------
void main(void){
    setup();                            //Llamamos a nuestras configuraciones
    ban=1;
    while (1) {                         //Loop
        if(ADCON0bits.GO == 0){         //Cambio de canal
            if(ADCON0bits.CHS == 5)     //Canal 5
                ADCON0bits.CHS = 6;     //Cambio a canal 6
            
            else                        //Canal 6
            ADCON0bits.CHS = 5;         //Cambio a canal 5
            
            __delay_us(50);             //Delay
            ADCON0bits.GO = 1;          //GO
        }
   }
    if(ban==1){
        menu;
        }
}
//-------------------------------------------------------
//   Conf PIC
//-------------------------------------------------------
void setup(void){               //Configuraciones del PIC
    //Entradas digitales y analogicas
    ANSEL   = 0b01100000;       //Se utiliza canal 5 y 6
    ANSELH  = 0x00;          
    
    //Entradas y Salidas
    TRISE   = 0x03;             //Entradas
    TRISC   = 0x00;             //salidas
            
    //Configuracion del oscilador
    OSCCONbits.IRCF2    = 1;    //8MHZ
    OSCCONbits.IRCF1    = 1;
    OSCCONbits.IRCF0    = 1;
    OSCCONbits.SCS      = 1;    //Oscilador interno
    
    //Configuracion de ADC
    ADCON1bits.ADFM     = 0;    //Justificado a la izquierda
    ADCON1bits.VCFG0    = 0;    //voltaje VSS
    ADCON1bits.VCFG1    = 0;    //voltaje VDD
    ADCON0bits.ADCS     = 0b10; //Fosc/32
    ADCON0bits.CHS      = 5;    //canal 5
    __delay_us(50);
    ADCON0bits.ADON     = 1;    //Enciendo
    
    //Configuracion de PWM
    TRISCbits.TRISC2    = 1;    //CCP1
    TRISCbits.TRISC1    = 1;    //CCP2    
    PR2 = 255;                  //PWM
    CCP1CONbits.P1M     = 0;    //Modo PWM
    CCP1CONbits.CCP1M   = 0b1100;   //Modo     
    CCP2CONbits.CCP2M   = 0b1100; 
    CCPR1L              = 0x0f; //Periodo incial
    CCPR2L              = 0x0f; 
    CCP1CONbits.DC1B    = 0;    //Bits menos significativos
    CCP2CONbits.DC2B0   = 0;        
    CCP2CONbits.DC2B1   = 0;        
    
    //TMR2
    PIR1bits.TMR2IF     = 0;    //Bajo bandera
    T2CONbits.T2CKPS    = 0b11; //1:16
    T2CONbits.TMR2ON    = 1;    //Enciendo TMR2
    
    while(PIR1bits.TMR2IF == 0);//Ciclo TMR2 Para levantar bandera
    PIR1bits.TMR2IF     = 0;    //Bajo bandera 
    TRISCbits.TRISC2    = 0;    //Salida PWM
    TRISCbits.TRISC1    = 0;    //Salida PWM    
    
    //Configuraciones de interrupciones 
    INTCONbits.GIE      = 1;
    INTCONbits.PEIE     = 1;
    PIE1bits.ADIE       = 1;
    PIR1bits.ADIF       = 0;
    
    ADCON0bits.GO       = 1;
}